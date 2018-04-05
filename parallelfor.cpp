#include<iostream>
#include<vector>
#include<stdlib.h>
#include <papi.h>
#include<cilk/cilk.h>
using namespace std;

void multiply1(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply2(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply3(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int j=0;j<n;j++){
        for(int i=0;i<n;i++){
            for(int k=0;k<n;k++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply4(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int j=0;j<n;j++){
        for(int k=0;k<n;k++){
            for(int i=0;i<n;i++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply21(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    cout <<"hello"<<endl;
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            cilk_for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply23(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    cilk_for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply24(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    cilk_for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            cilk_for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply5(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    cilk_for(int k=0;k<n;k++){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}
void multiply52(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int k=0;k<n;k++){
        cilk_for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

void multiply53(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int k=0;k<n;k++){
        for(int i=0;i<n;i++){
            cilk_for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}
void multiply54(vector<vector<int> >a, vector<vector<int> >b){
    int n = a.size();
    vector<vector<int> >c(n, vector<int>(n, 0));
    for(int k=0;k<n;k++){
        cilk_for(int i=0;i<n;i++){
            cilk_for(int j=0;j<n;j++){
                c[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
}

int main(int argc,char **argv){
    int option=atoi(argv[1]);
    int n =atoi(argv[2]);
    vector<vector<int> >a(n, vector<int>(n, rand()%10));
    vector<vector<int> >b(n, vector<int>(n, 0));
    long long counters[3];
    int PAPI_events[] = {
        PAPI_L1_TCM,
        PAPI_L2_TCM,
        PAPI_L3_TCM 
    };

    PAPI_library_init(PAPI_VER_CURRENT);
    int i = PAPI_start_counters( PAPI_events, 3 );
    switch(option){
        case 1: multiply1(a,b);
        break;
        case 21: multiply21(a,b);
        break;
        case 23: multiply23(a,b);
        break;
        case 24: multiply24(a,b);
        break;
        case 3: multiply3(a,b);
        break;
        case 4: multiply4(a,b);
        break;
        case 52: multiply52(a,b);
        break;
        case 53: multiply53(a,b);
        break;
        case 54: multiply54(a,b);
        break;
        default: multiply6(a,b);
        break;
    }
    PAPI_read_counters( counters, 3 );
    cout<<counters[0]<<" "<<counters[1]<<" "<<counters[2]<<endl;
    cout<<"finished"<<endl;
}