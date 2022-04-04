#include<bits/stdc++.h>
using namespace std;

struct symtab{
     string address;
     string name;
     int relative;
     int blockNumber;
     char exists;
     symtab(){
       name="undefined";
       address="0";
       relative = 0;
       blockNumber = 0;
       exists='n';
     }
};

typedef map<string,symtab> SYMBOL_TABLE;
SYMBOL_TABLE SYMTAB;

