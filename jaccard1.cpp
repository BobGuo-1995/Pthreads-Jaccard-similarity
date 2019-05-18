#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include "etime.h"
#include <pthread.h>
//*****
//our include
#include <string>
#include <iomanip>
#include <unordered_set>
#include <algorithm>
using namespace std;
//the type of the row of the matrix.
typedef unordered_set<int> TRow;
typedef vector<TRow> TMatrix;
//input data
TMatrix matrix;

/*
0 2 5 9
1 3 4 9 12
2 1 6
3 1 5
4 3 4 5 15

0 is set id, 2 is number values, and in it is 5,9, But where and most importantly how are we storing in our matrix, so we know where to look it up?
matrix[0]=unordered_set{5,9}
matrix[1]=unordered_set{4,9,12}
matrix[2]=unordered_set{6}
matrix[3]=unordered_set{5}
matrix[4]=unordered_set{4,5,15}
*/

//The class of the result entry
struct TElement{
    //position in the matrix (compared rows)
	//row 1
	int i;
	//row 2
    int j;
    float JaccardValue;
    TElement(int i,int j)
    {
        this->i=i;
        this->j=j;
        JaccardValue=0;
    }
};
//We use this class (functor) to the sort function. When we sort the result.
//We use this struct in the code: sort(result.begin(),result.end(),rsort);
//The sort function can sort some objects if it can compare its.
//The compare function must return the result of the expression left<rigth true or false

struct RSort{
    bool operator()(const TElement& left,const TElement& right)
    {
    
    /*
    
    0 2 5 9
    1 3 4 9 12
    2 1 6
    3 1 5
    4 3 4 5 15
    */
	//Input data - vector of TElement. Not matrix. TElement is a result of compare 2 rows i and j
	//This is just random example, not based on the smalldata.txt
	//For example
	//[{i=2,j=1,JaccardValue=0.6}, {i=1,j=3,JaccardValue=0.7}]
	//We need order above by i
	//must be:
	//[{i=1,j=3,JaccardValue=0.7}, {i=2,j=1,JaccardValue=0.6}]
        if(left.i!=right.i) return left.i<right.i;
        return left.j<right.j;
    }
    
	//struct RSort{....}
	//RSort rsort;
} rsort;
//each thread writes data to its own vector
//thread 0 to the vResult[0]
//thread 1 to the vResult[1]
vector<vector<TElement>> vResult;

// This is the work function that each thread will execute
void* work(void* rank) {
	//Get thread id
int id=*((int*)rank);
for(auto &el:vResult[id])
{
	//compute the number of the intersecting elements
   /* 
    0 2 5 9
    1 3 4 9 12
    2 1 6
    3 1 5
    4 3 4 5 15
    stepping through it a bit 
   */ 
   /*
   For example we want thread 0 compare 2 rows: 0 and 1.
    0 2 5 9
    1 3 4 9 12
	1.
	matrix[0]=set{5,9}
	matrix[1]=set{4,9,12}
	2.
	we pass to the work function *rank=0 and push into vResult[0] TElement={i=0 - row number 0, j=1 - row number 1, JaccardValue=0 - we don't know it now}
	3.
	in the loop we calculate the number of intersections between set{5,9} - row 0 and set{4,9,12} - row 1
	after that we can calculate  Jaccard Value and store it in the TElement
   */
    int intersectNum=0;
    for(int a:matrix[el.i])
    {
        if(matrix[el.j].find(a)!=matrix[el.j].end())
        {
            ++intersectNum;
        }
    }
    el.JaccardValue=1.0*intersectNum/(matrix[el.i].size()+matrix[el.j].size()-intersectNum);
}
	return NULL;
}  


int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: jaccard data_file number_of_threads" << endl;
        exit(-1);
    }
    string fname=argv[1];
	string fnameOutput(fname);
	fnameOutput.insert(fnameOutput.size()-4,"ExpectedOutput");
    int threadCounts=stoi(argv[2]);
    //start the timer
    tic();
//read data file
    ifstream f1(fname);
    int rowCount;
    f1>>rowCount;
    for(int i=0;i<rowCount;++i){
        int num;
        f1>>num;
        if(num!=i){printf("data file error");exit(-1);}
        int columnCount;
        f1>>columnCount;
        TRow row;
        for(int i=0;i<columnCount;++i){
            int val;
            f1>>val;
            row.emplace(val);
        }
        matrix.emplace_back(row);
    }
//end read
//*******************
//we create result vectors for threads
//and write there which rows will be compare each thread 
vResult.resize(threadCounts);
int thi=0;
    int n=matrix.size();
    for(int i=0;i<n-1;++i)
    {
        for(int j=i+1;j<n;++j)
            {
                vResult[thi].emplace_back(i,j);
                if(++thi>=threadCounts)thi=0;
            }
    }
	//Create threads
    vector<pthread_t> vthreads;
    vector<int> vargs(threadCounts);
    for(int i=0;i<threadCounts;++i){
        pthread_t tid; /* thread id */
        pthread_attr_t attr; /* thread attributes */
        pthread_attr_init(&attr);
        vargs[i]=i;
        //this create the thread and use the work function we wrote to do the job 
		//We create the thread and pass i - number of the thread to the work function
        pthread_create(&tid,NULL,&work,&vargs[i]);
		//We just store the thread id (it's as pid for process) and use it later to wait for the thread to complete
        vthreads.push_back(tid);
    }
	//************
    for(int i=0;i<threadCounts;++i){
    pthread_join(vthreads[i],NULL);
    }
	//stop the timer
    toc();
	
    cout<<"time="<<etime()<<endl;
//Sort and out the result
    cout<<"Sorting result..."<<endl;
    vector<TElement> result;
    for(auto& v:vResult){
        copy(v.begin(),v.end(),back_inserter(result));
    }
    sort(result.begin(),result.end(),rsort);
    for(auto& el:result)
        {
            if (el.JaccardValue>=0.5)
                cout<<el.i<<","<<el.j<<","<<el.JaccardValue<<endl;
        }
	/*
	cout<<"Save result data to "<<fnameOutput<<endl;
     ofstream f2(fnameOutput);
        for(auto& el:result)
        {
            if (el.JaccardValue>=0.5)
                f2<<el.i<<","<<el.j<<","<<el.JaccardValue<<endl;
        }
		*/
    return 0;
}  
