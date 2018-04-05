#include <iostream>
#include <vector>
#include <stdlib.h>
#include<cilk/cilk.h>
#include <papi.h>
#include<sys/time.h>
#include<cilk/cilk_api.h>
int m;
using namespace std;
void multiply2(vector<vector<int> > &z, int x1, int y1, vector<vector<int> > &x,
    int x2, int y2, vector<vector<int> > &y, int x3, int y3, int n){
    for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            for(int j=0;j<n;j++){
                z[i+x1][j+y1]+=x[i+x2][k+x2]*y[k+x3][j+y3];
            }
        }
    }
}

void recursizeMatrixMultiplication(vector<vector<int> > &z, int x1, int y1, vector<vector<int> > &x,
    int x2, int y2, vector<vector<int> > &y, int x3, int y3, int n){
    if (n == m){
        multiply2(z, x1, y1, x, x2, y2, y, x3, y3, n);
    }
    else{
        cilk_spawn recursizeMatrixMultiplication(z, x1, y1, x, x2, y2, y, x3, y3, n/2);
        cilk_spawn recursizeMatrixMultiplication(z, x1, y1 + n/2, x, x2, y2, y, x3, y3 + n/2, n/2);
        cilk_spawn recursizeMatrixMultiplication(z, x1 + n/2, y1, x, x2 + n/2, y2, y, x3, y3, n/2);
        recursizeMatrixMultiplication(z, x1 + n/2, y1, x, x2 + n/2, y2, y, x3, y3 + n/2, n/2);
        cilk_sync;
        cilk_spawn recursizeMatrixMultiplication(z, x1, y1, x, x2, y2 + n/2, y, x3, y3 + n/2, n/2);
        cilk_spawn recursizeMatrixMultiplication(z, x1, y1 + n/2, x, x2, y2 + n/2, y, x3 + n/2, y3 + n/2, n/2);
        cilk_spawn recursizeMatrixMultiplication(z, x1 + n/2, y1, x, x2 + n/2, y2 + n/2, y, x3 + n/2, y3, n/2);
        recursizeMatrixMultiplication(z, x1 + n/2, y1 + n/2, x, x2 + n/2, y2 + n/2, y, x3 + n/2, y3 + n/2, n/2);
        cilk_sync;
    }

}

int main(int argc,char*argv[]){
    int n =atoi(argv[1]);
    m=32;
    char * no_of_workers_enabled;
    if(argc==3)
        no_of_workers_enabled=argv[2];

    vector<vector<int> > x(n, vector<int>(n, 1));
    vector<vector<int> > y(n, vector<int>(n, 2));
    vector<vector<int> > z(n, vector<int>(n, 0));
    long long counters[3];
    int PAPI_events[] = {
    PAPI_L1_TCM,
    PAPI_L2_TCM,
    PAPI_L3_TCM };

    PAPI_library_init(PAPI_VER_CURRENT);
    int i = PAPI_start_counters( PAPI_events, 3 );
    struct timeval t,t1;
    gettimeofday(&t,NULL);
    if(argc==3)
        __cilkrts_set_param("nworkers",no_of_workers_enabled);
    recursizeMatrixMultiplication(z, 0, 0, x, 0, 0, y, 0, 0, n);
    gettimeofday(&t1,NULL);
    PAPI_read_counters( counters, 3 );
    cout<<"time taken:"<<((t1.tv_sec-t.tv_sec)*1000000L +(t1.tv_usec-t.tv_usec))<<endl;
    cout<<counters[0]<<" "<<counters[1]<<" "<<counters[2]<<endl;
}