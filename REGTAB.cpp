#include<bits/stdc++.h>
using namespace std;

struct regtab{
     char num;
     char exists;
     regtab(){
       num = 'F';
       exists='n';
     }
};

typedef map<string,regtab> REG_TABLE;
REG_TABLE REGTAB;

void createREGTAB(){
  REGTAB["A"].num='0';
  REGTAB["A"].exists='y';

  REGTAB["X"].num='1';
  REGTAB["X"].exists='y';

  REGTAB["L"].num='2';
  REGTAB["L"].exists='y';

  REGTAB["B"].num='3';
  REGTAB["B"].exists='y';

  REGTAB["S"].num='4';
  REGTAB["S"].exists='y';

  REGTAB["T"].num='5';
  REGTAB["T"].exists='y';

  REGTAB["F"].num='6';
  REGTAB["F"].exists='y';

  REGTAB["PC"].num='8';
  REGTAB["PC"].exists='y';

  REGTAB["SW"].num='9';
  REGTAB["SW"].exists='y';
}

