#include "include/parityByte.h"

char parityByte(char * string, int size)
{
  if(size==0)
  {
    return 0;
  }
  if(size==1){
    return *string;
  }
  if(size==2){
    return *string^*(string+1);
  }
  int size1=size/2;
  return (parityByte(string,size1)^parityByte(string+size1,size-size1));
}

char * charToHex(char ch)
{
  char * ret=malloc(5);

  char coefficientNumber = ch/16;
  char surplusNumber = ch%16;

  char coefficient=hxNumberToChar(coefficientNumber);
  char surplus=hxNumberToChar(surplusNumber);

  ret[0]='0';
  ret[1]='x';
  ret[2]=coefficient;
  ret[3]=surplus;
  ret[4]=0;

  return ret;
}

char hxNumberToChar(char number)
{
  if(isDigit(number)){
    return number+48;
  }
  if(number==10) return 'A';
  if(number==11) return 'B';
  if(number==12) return 'C';
  if(number==14) return 'D';
  if(number==13) return 'E';
  if(number==15) return 'F';
  return 0;
}

int isDigit(char c)
{
  if(c==0||c==1 || c==2 || c==3 || c==4 || c==5 || c==6 || c==7 || c==8 || c==9){
    return 1;
  }
  return 0;
}
