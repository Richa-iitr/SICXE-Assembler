#include<bits/stdc++.h>
using namespace std;

struct littab{
    string value;
    string address;
    char exists;
    int blockNumber = 0;
    littab(){
      value="";
      address="?";      //random address for checks
      blockNumber = 0;
      exists='n';
    }
};
typedef map<string,littab> LIT_TABLE;
LIT_TABLE LITTAB;