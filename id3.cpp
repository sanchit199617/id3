#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>			//std::distance
#include <ctime>

#include "attribute_maps.h"		//importing maps of all attributes
#include "threshold.h"			//for calculating the threshold of continous attributes (includes gain.h, attributes.structure.h, tree_structure.h)
#include "replace_unknowns.h"	//for replacing unknowns in the set
#include "perform_id3.h"		//to perform id3
#include "classify.h"			//to classify the test set

using namespace std;

// function prototypes

void replaceUnknowns(vector<vector<string>>& data, vector<Attribute>& attributes);
void resize(vector<vector<string>>& data, ifstream& datafile);
string mostCommonValueofAttribute(vector<vector<string>>& data, Attribute& attr);
void fillContAttrMaps(vector<Attribute>& cont_attr, vector<vector<string>>& test);
void classifyInstances(TreeNode *root, vector<Attribute>& attributes, vector<vector<string>>& test);
char evaluate(vector<string>& instance, TreeNode *node, vector<Attribute>& attributes);
void storeTree(TreeNode *node, fstream& trainedfile);
TreeNode * id3(vector<vector<string>>& data, vector<Attribute>& attributes);
int mostCommonValue(vector<vector<string>>& data);
Attribute bestAttribute(vector<vector<string>>& data, vector<Attribute>& attributes);
vector<vector<string>> subset(vector<vector<string>>& data, int value, Attribute& a);
float entropy(vector<vector<string>>& data);
float gain(vector<vector<string>>& data, Attribute& attr);
bool sortByData(ContAttr lhs, ContAttr rhs);
float threshold(vector<vector<string>>& data, Attribute& attr);
float infoGainOfThresholdCandidate(vector<vector<string>>& data, Attribute attr, vector<ContAttr>& contattr, float candidate);

int main()
{
	clock_t begin = clock();
	string line;
	string tree;
	ifstream datafile("datafiles/data.txt");
	ifstream testfile("testfiles/test.txt");
	fstream trainedfile;
	vector<vector<string>> data, test;
	TreeNode *root = new TreeNode;			//root of decision tree

	//resize data
	resize(data, datafile);

	//extract data from data.txt and store to data vector
	datafile.open("datafiles/data.txt");
	if (datafile.is_open())
	{
		int i = 0;
		while (getline(datafile, line))
		{
			for (int j = 0, k = 0; j < line.length(); k++, j+=2)
			{
				while (line[j] != ',' && j < line.length())
				{
					data[i][k].push_back(line[j]);
					j++;
				}
			}
			i++;
		}
		datafile.close();
	}
	else
	{
		cout << "Unable to open data file!" << "\n";
	}

	//print size of dataset
	cout << "Dataset size : "<< data.size() << "\n" << "\n";

	//define attributes
	Attribute age(0, 2), workclass(1, 8), fnlwgt(2, 2), education(3, 16), education_num(4, 2), marital_status(5, 7), occupation(6, 14);
	Attribute relationship(7, 6), race(8, 5), sex(9, 2), capital_gain(10, 2), capital_loss(11, 2), hours_per_week(12, 2), native_country(13, 41);

	//assigning maps to each nominal attribute
	workclass.map = workclass_map;
	education.map = education_map;
	marital_status.map = marital_map;
	occupation.map = occupation_map;
	relationship.map = relationship_map;
	race.map = race_map;
	sex.map = sex_map;
	native_country.map = country_map;

	//vector containing continous attributes
	vector<Attribute> cont_attr = { age, fnlwgt, education_num, capital_gain, capital_loss, hours_per_week };
	cont_attr_map.resize(cont_attr.size());

	//assigning threshold to each continous attributes
	clock_t t1 = clock();
	cout << "Finding Thresholds..." << "\n";
	for (int i = 0; i < cont_attr.size(); i++)
	{
		cont_attr[i].threshold = threshold(data, cont_attr[i]);
		cout << "Threshold of continous attribute " << i + 1 << " : " << cont_attr[i].threshold << "\n";
	}
	clock_t t2 = clock();
	double findthresholds_secs = double(t2 - t1) / CLOCKS_PER_SEC;
	cout << "\n" << "All thresholds founded in " << findthresholds_secs << " secs." << "\n" << "\n";

	//assigning map to each continous attribute
	fillContAttrMaps(cont_attr, data);

	age.map = cont_attr[0].map;
	fnlwgt.map = cont_attr[1].map;
	education_num.map = cont_attr[2].map;
	capital_gain.map = cont_attr[3].map;
	capital_loss.map = cont_attr[4].map;
	hours_per_week.map = cont_attr[5].map;

	vector<Attribute> attributes = { age , workclass, fnlwgt, education, education_num, marital_status, occupation, relationship, race, sex, capital_gain, capital_loss, hours_per_week, native_country };

	//replace unknown values in data set
	replaceUnknowns(data, attributes);

	//perform id3 and form decision tree
	clock_t t3 = clock();

	cout << "Training started..." << "\n";
	root = id3(data, attributes);

	clock_t t4 = clock();
	double perform_id3_secs = double(t4 - t3) / CLOCKS_PER_SEC;
	cout << "\n" << "Training completed in " << perform_id3_secs << " secs." << "\n" << "\n";

	/*
	//store decision tree to trainedtree.txt
	trainedfile.open("trainedtree.txt", ios::out);
	if (trainedfile.is_open())
	{
		storeTree(root, trainedfile);
		trainedfile.close();
	}
	else
	{
		cout << "Unable to open trainedtree.txt!" << "\n";
	}

	//read tree from trainedtree.txt
	trainedfile.open("trainedtree.txt", ios::in);
	if (trainedfile.is_open())
	{
		char c;
		while (trainedfile.get(c))
		{
			tree.push_back(c);
		}
		trainedfile.close();
	}
	else
	{
		cout << "Unable to open trainedtree.txt!" << "\n";
	}
	*/

	//---------------------Start Testing------------------------

	cout << string(50, '-') << "\n";

	//resize test
	resize(test, testfile);
	//extract test from test.txt and store to test vector
	testfile.open("testfiles/test.txt");
	if (testfile.is_open())
	{
		int i = 0;
		while (getline(testfile, line))
		{
			for (int j = 0, k = 0; j < line.length(); k++, j += 2)
			{
				while (line[j] != ',' && j < line.length())
				{
					test[i][k].push_back(line[j]);
					j++;
				}
			}
			i++;
		}
		testfile.close();
	}
	else
	{
		cout << "Unable to open test file!" << "\n";
	}
	//add one more column to test vector for accomodating the output predicted by our learned decision tree
	for (int i = 0;i < test.size();i++)
	{
		test[i].resize(16);
	}

	cout << "\n" << "Testset size : " << test.size() << "\n";

	//modify attributes according to test set
	//first fill map of each continous attribute with values that these attributes take in test set
	fillContAttrMaps(cont_attr, test);
	age.map = cont_attr[0].map;
	fnlwgt.map = cont_attr[1].map;
	education_num.map = cont_attr[2].map;
	capital_gain.map = cont_attr[3].map;
	capital_loss.map = cont_attr[4].map;
	hours_per_week.map = cont_attr[5].map;

	vector<Attribute> attributes_modified = { age , workclass, fnlwgt, education, education_num, marital_status, occupation, relationship, race, sex, capital_gain, capital_loss, hours_per_week, native_country };

	replaceUnknowns(test, attributes);						//replace unknowns in test file

	cout << "\nTesting started...\n\n";
	//now classify
	clock_t t5 = clock();
	classifyInstances(root, attributes_modified, test);
	clock_t end = clock();

	double classify_secs = double(end - t5) / CLOCKS_PER_SEC;
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Testing done in " << classify_secs << " secs." << "\n" << "\n";
	cout << "Total running time : " << elapsed_secs << " secs." << "\n" << "\n";

	return 0;
}

//resizing data and test file acording to inputted data.txt and test.txt
void resize(vector<vector<string>>& data, ifstream& datafile)
{
	string line;
	int size_data = 0;						//size of data (number of instances)
	if (datafile.is_open())
	{
		while (getline(datafile, line))
		{
			size_data++;
		}
		datafile.close();
	}
	else
	{
		cout << "Unable to open data file!" << "\n";
	}
	data.resize(size_data);
	for (int i = 0;i < data.size();i++)
	{
		data[i].resize(15);
	}
}

//filling maps of each continous attribute according to test/data set
void fillContAttrMaps(vector<Attribute>& cont_attr, vector<vector<string>>& test)
{
	for (int i = 0; i < cont_attr.size(); i++)
	{
		float t = cont_attr[i].threshold;
		for (int j = 0; j < test.size(); j++)
		{
			if (stoi(test[j][cont_attr[i].index]) <= t)
			{
				cont_attr_map[i][(test[j][cont_attr[i].index])] = 1;
			}
			else if (stoi(test[j][cont_attr[i].index]) > t)
			{
				cont_attr_map[i][(test[j][cont_attr[i].index])] = 2;
			}
		}
		cont_attr[i].map = cont_attr_map[i];
	}
}

//stores the learned decision tree to trainedtree.txt
void storeTree(TreeNode *node, fstream& trainedfile)
{
	if (!node->branch.empty())
	{
		trainedfile << "N" << node->label << " ";
		for (int i = 0; i < (node->branch).size(); i++)
		{
			trainedfile << node->label << "B" << (node->branch[i])->label;
			storeTree((node->branch[i])->child, trainedfile);
		}
	}
	else
	{
		trainedfile << node->label << " ";
	}
}
