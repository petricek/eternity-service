//*************************************************************************
// 
// File: auxiliary.cc
// Name: 
// Author: Pechy
// Last Modified: 28.1.1999
// 
// Description:
//
//*************************************************************************

  // has to be the *FIRST* include !
#include "./../../../Common/H/common.h"

#include <stdio.h>
#include <string>

//-------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
string displayStream(char *octetStream, int charsToPrint)
{

  string stream = string("");
  char tmpChar[50];

  for (int i = 0; i < charsToPrint; ++i) {
    unsigned int tmp = (unsigned char) octetStream[i];
    sprintf(tmpChar, "%u", tmp);
    stream = stream + "#" + tmpChar;
  }

  return stream;
}
