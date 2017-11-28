## Implementing the id3 algorithm in C++

This is a naive implementation of the [id3](https://en.wikipedia.org/wiki/ID3_algorithm) (decision tree) algorithm in C++.  
This version can handle continous and missing value attributes.   
The repository contains only the necessary cpp and header files required for successful compilation.  

Running
------
To test the algorithm, **compile and run id3.cpp**

Dataset Information
------
[Census Income Dataset](http://archive.ics.uci.edu/ml/datasets/Adult dataset) was used for training and testing.  
[datafiles/data.txt](https://github.com/sanchit199617/id3/blob/master/datafiles/data.txt) contains the training instances.  
[testfiles/test.txt](https://github.com/sanchit199617/id3/blob/master/testfiles/test.txt) contains the testing instances. These instances were used calculate the algorithm's accuracy.  

The code was written specifically for this dataset and will require significant changes before running on some other dataset. 

Built With
------
* [Visual Studio 2015](https://www.visualstudio.com/)

Additional Notes
------
This project was made only for understanding purpose, as a part of my course assignment, 
so no performance or memory improvements were made whatsoever.  
