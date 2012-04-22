//*************************************************************************
// 
// File: esoOffspring.cc
// Name: EsoOffspring
// Author: Venca
// Last Modified: 2.1.1999 V
// 
// Description:
// Just remembers pointer to its father.
//
//*************************************************************************

#include "H/esoOffspring.h"

//-------------------------------------------------------------------------
// EsoOffspring
// ~~~~~~~~~~~~
// remembers pointer to its father
//-------------------------------------------------------------------------
EsoOffspring::EsoOffspring(Eso * eso)
{
  this->eso = eso;
}
