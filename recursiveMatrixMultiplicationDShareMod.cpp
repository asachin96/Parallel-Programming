#include <iostream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <map>
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

concurrent_vector<concurrent_queue<arguments>> taskqueue;
int size=64;
map<thread::id, int> threadToQueue;
vector<vector<int> > x(size, vector<int>(size, rand()%20));
vector<vector<int> > y(size, vector<int>(size, rand()%20));
vector<vector<int> > z(size, vector<int>(size, 0));
concurrent_vector<stackArguments>stack;
mutex mtx1, mtx2;
int noOfThreads = 1;
void recursiveMatrixMultiplication1(int x1, int y1, int x2, int y2, int x3, int y3, int n);
void recursiveMatrixMultiplication2(int x1, int y1, int x2, int y2, int x3, int y3, int n);
void recursizeMatrixMultiplication(int x1, int y1, int x2, int y2,  int x3, int y3, int n);

std::atomic<int> flag(0);

void pushToTaskQueue(int x1, int y1, int x2, int y2, int x3, int y3, int n, int fun, int index, bool isRand){
	arguments arg = {x1, y1, x2, y2, x3, y3, n, fun, index};
	if(isRand){
		int p = rand()%noOfThreads, q = rand()%noOfThreads;
		if(taskqueue[p].unsafe_size()<taskqueue[q].unsafe_size()){
			taskqueue[p].push(arg);
		}else{
			taskqueue[q].push(arg);
		}
	}else{
		taskqueue[threadToQueue[this_thread::get_id()]].push(arg);
	}
}

int pushToTaskStack(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	lock_guard<mutex> lock(mtx1);
	stackArguments tt = {x1, y1, x1, y2, x3, y3, n, 4};
 	stack.push_back(tt);
 	return stack.size()-1;
}

void execute(int i){
	threadToQueue[this_thread::get_id()] = i;
	int retry = 1000;
	while(flag){
		arguments ret;
		if(!taskqueue[i].try_pop(ret)) continue;
		bool run = false;
		stackArguments arg;
		arg.n = 0;
		if(ret.fun == 1){
			{
				lock_guard<mutex> lock(mtx1);
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
		--flag;
	}
}

void init(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	flag += 4;
	int index = pushToTaskStack(x1, y1, x1, y2, x3, y3, n);
	pushToTaskQueue(x1, y1, x1, y2, x3, y3, n/2, 1, index, true);
	pushToTaskQueue(x1, y1 + n/2, x1, y2, x3, y3 + n/2, n/2, 1, index, true);
	pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2, x3, y3, n/2, 1, index, true);
	pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2, x3, y3 + n/2, n/2, 1, index, true);
}

void recursiveMatrixMultiplication1(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	if (n == 1){
		z[x1][y1] += x[x2][y2]*y[x3][y3];
	}
 	else{
 		flag += 4;
 		int index = pushToTaskStack(x1, y1, x1, y2, x3, y3, n);
 		pushToTaskQueue(x1, y1, x1, y2, x3, y3, n/2, 1, index, true);
 		pushToTaskQueue(x1, y1 + n/2, x1, y2, x3, y3 + n/2, n/2, 1, index, true);
 		pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2, x3, y3, n/2, 1, index, true);
 		pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2, x3, y3 + n/2, n/2, 1, index, false);
 	}
}

void recursiveMatrixMultiplication2(int x1, int y1, int x2, int y2, int x3, int y3, int n){
	int index = -1;
	flag += 4; 
	pushToTaskQueue(x1, y1, x1, y2 + n/2, x3, y3 + n/2, n/2, 2, index, true);
	pushToTaskQueue(x1, y1 + n/2, x1, y2 + n/2, x3 + n/2, y3 + n/2, n/2, 2, index, true);
	pushToTaskQueue(x1 + n/2, y1, x1 + n/2, y2 + n/2, x3 + n/2, y3, n/2, 2, index, true);
	pushToTaskQueue(x1 + n/2, y1 + n/2, x1 + n/2, y2 + n/2, x3 + n/2, y3 + n/2, n/2, 2, index, false);
}

int main(int argc, char **argv){
	size = atoi(argv[1]);
	x.resize(size);
	y.resize(size);
	z.resize(size);
	taskqueue.resize(size);
	for (int i = 0; i < size; ++i){
	    x[i].resize(size);
	    y[i].resize(size);
	    z[i].resize(size);
	}

	init(0, 0, 0, 0, 0, 0, size);
	noOfThreads = atoi(argv[2]);
	thread *t[10];
	for(int i=0;i<noOfThreads;i++){
		t[i] = new thread(execute, i);
	}
	for(int i=0;i<noOfThreads;i++){
		t[i]->join();		
	}
}