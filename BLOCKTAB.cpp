#include<bits/stdc++.h>
using namespace std;
struct blocktab{
     string startAddress;
     string name;
     string LOCCTR;
     int number;
     char exists;
     blocktab(){
       name="undefined";
       startAddress="?";
       exists='n';
       number = -1;
       LOCCTR = "0";
     }
};




typedef map<string,blocktab> BLOCK_TABLE_TYPE;
BLOCK_TABLE_TYPE BLOCKS;


void createBlocks(){
  BLOCKS["DEFAULT"].exists = 'y';
  BLOCKS["DEFAULT"].name = "DEFAULT";
  BLOCKS["DEFAULT"].startAddress = "00000";
  BLOCKS["DEFAULT"].number=0;
  BLOCKS["DEFAULT"].LOCCTR = "0";
}