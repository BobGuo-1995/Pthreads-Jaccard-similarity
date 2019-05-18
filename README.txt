# Pthreads-Jaccard-similarity
Using pthreads to compute Jaccard similarity of sets
//info 
The two files named smallData.txt and mediumData.txt are input files to use in executing your program.  
While developing your program, use smallData.txt for testing.  When you feel you have your program in a good place, 
test with mediumData.txt.  The correct answers that one should get for those files are in the files smallDataExpectedOutput.txt and 
mediumDataExpectedOutput.txt.

//compilation and running 
To compile the program, you should execute:
make

To run the program, you can execute the following command line, 
./jaccard data_file number_of_threads
replacing data_file and number_of_threads as appropriate.

Example of processing smallData.txt with one thread: ./jaccard smallData.txt 1
Example of processing mediumData.txt with four threads: ./jaccard mediumData.txt 4

//extra information 
All pairs Jaccard computation
Your program should compute the Jaccard values for all pairs of sets.  So, for the smallData.txt file being used as an example, that means a Jaccard value should be computed for the following pairs of sets:
0 and 1               1 and 3
0 and 2               1 and 4
0 and 3               2 and 3
0 and 4               2 and 4
1 and 2               3 and 4

As a sanity check, for N sets there should be (N*(N-1))/2 pairs of sets.  Since smallData.txt has 5 sets, there are (5*4)/2 → 10 pairs of sets to compute the Jaccard value for. 

Jaccard computation
Assume you have two sets (I’ve taken set 0 and set 1 from smallData.txt for my example below)
0 2 5 9
1 3 4 9 12

Remember that those descriptions say: 
Set 0 has two values: {5,9}
Set 1 has three values: {4,9,12}

The Jaccard value for two sets is the size of the intersection of the two sets divided by the size of the union of the two data sets.
The intersection of two sets is the set of values that are present in both data sets
For this data, the intersection of sets 0 and 1 is the set {9}, so the intersection set has size 1
The union of two sets is the set of values that are in either data set, but with only one instance of any repeats
For this data, the union of sets 0 and 1 is the set {5,9,4,12}, so the union set has size 4
The Jaccard value is the size of the intersection divided by the size of the union, so for this dataset it would be 1 / 4 → 0.25.
You can assume that each set in the input will have size of at least 1.



