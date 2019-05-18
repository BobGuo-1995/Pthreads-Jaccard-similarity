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
struct RSort{
    bool operator()(const TElement& left,const TElement& right)
    {
        if(left.i!=right.i) return left.i<right.i;
        return left.j<right.j;
    }
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
        pthread_create(&tid,NULL,&work,&vargs[i]);
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
    cout<<"Save result data to "<<fnameOutput<<endl;
     ofstream f2(fnameOutput);
        for(auto& el:result)
        {
            if (el.JaccardValue>=0.5)
                f2<<el.i<<","<<el.j<<","<<el.JaccardValue<<endl;
        }
    return 0;
}  




