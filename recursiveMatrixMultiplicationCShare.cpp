#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>

using namespace std;
using namespace tbb;

typedef struct stackArguments{
	int x1, y1, x2, y2, x3, y3, n, c;
}stackArguments;

typedef struct arguments{
	int x1, y1, x2, y2, x3, y3, n, fun, index;
}arguments;

concurrent_queue<arguments> taskqueue;
int size=64;
vector<vector<int> > x(size, vector<int>(size, rand()%10));
vector<vector<int> > y(size, vector<int>(size, 0));
vector<vector<int> > z(size, vector<int>(size, 0));
concurrent_vector<stackArguments> stack;
mutex mtx1, mtx2;

void recursiveMatrixMultiplication1(int x1, int y1, int x2, int y2, int x3, int y3, int n);
void recursiveMatrixMultiplication2(int x1, int y1, int x2, int y2, int x3, int y3, int n);
void recursizeMatrixMultiplication(int x1, int y1, int x2, int y2,  int x3, int y3, int n);

std::atomic<int> counter(0), ff(0);

void multiply2(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	for(int i=0;i<n;i++){
		for(int k=0;k<n;k++){
			for(int j=0;j<n;j++){
				z[i+x1][j+y1]+=x[i+x2][k+y2]*y[k+x3][j+y3];
			}
		}
	}
}

void pushToTaskQueue(int x1, int y1, int x2, int y2, int x3, int y3, int n, int fun, int index){
	arguments arg = {x1, y1, x2, y2, x3, y3, n, fun, index};
	taskqueue.push(arg);
}

int pushToTaskStack(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	lock_guard<mutex> lock(mtx2);
	stackArguments tt = {x1, y1, x1, y2, x3, y3, n, 4};
 	stack.push_back(tt);
 	return stack.size()-1;
}

void execute(){
	while(counter){
		bool isEmpty;
		arguments ret;
		if(!taskqueue.try_pop(ret)) continue;
		bool run = false;
		stackArguments arg;
		arg.n = 0;
		if(ret.fun == 1){
			{
				lock_guard<mutex> lock(mtx2);
				--stack[ret.index].c;
				if(stack[ret.index].c == 0) {
					run = true;
					arg = stack[ret.index];
				}
			}
			if (run){
				recursiveMatrixMultiplication2(arg.x1, arg.y1, arg.x2, arg.y2, arg.x3, arg.y3, arg.n);
			}
		}
		recursiveMatrixMultiplication1(ret.x1, ret.y1, ret.x2, ret.y2, ret.x3, ret.y3, ret.n);
		counter--;
	}
}

void recursiveMatrixMultiplication1(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	if (n == 32){
		ff++;
		multiply2(x1,y1,x2,y2,x3,y3, n);
	}
 	else{
 		counter+=4;
 		int index = pushToTaskStack(x1, y1, x1, y2, x3, y3, n);
 		pushToTaskQueue(x1, y1, x1, y2, x3, y3, n/2, 1, index);
 		pushToTaskQueue(x1, y1 + n/2, x1, y2, x3, y3 + n/2, n/2, 1, index);
 		pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2, x3, y3, n/2, 1, index);
 		pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2, x3, y3 + n/2, n/2, 1, index);
 	}
}

void recursiveMatrixMultiplication2(int x1, int y1, int x2, int y2, int x3, int y3, int n){
 	counter+=4;
	int index = -1;
	pushToTaskQueue(x1, y1, x1, y2 + n/2, x3, y3 + n/2, n/2, 2, index);
	pushToTaskQueue(x1, y1 + n/2, x1, y2 + n/2, x3 + n/2, y3 + n/2, n/2, 2, index);
	pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2 + n/2, x3 + n/2, y3, n/2, 2, index);
	pushToTaskQueue(x1 + n/2, y1 + n/2, x1 + n/2, y2 + n/2, x3 + n/2, y3 + n/2, n/2, 2, index);
}

int main(int argc, char **argv){
	size = atoi(argv[1]);
	x.resize(size);
	y.resize(size);
	z.resize(size);
	for (int i = 0; i < size; ++i){
	    x[i].resize(size);
	    y[i].resize(size);
	    z[i].resize(size);
	}
	recursiveMatrixMultiplication1(0, 0, 0, 0, 0, 0, size);
	int noOfThreads = atoi(argv[2]);
	thread *t[10];
	for(int i=0;i<noOfThreads;i++){
		t[i] = new thread(execute);
	}
	for(int i=0;i<noOfThreads;i++){
		t[i]->join();		
	}
}