#include "./H/payRecord.h"

/**
 * Payment Record constructor.
 *
 * Payment Record constructor initializes class variables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     
 */
PayRecord::PayRecord()
{
  eCode = OK;
  time.SetTime(0,0,0);
  payId = BAD_ID;
  amount = NULL;
  sealSAuth = NULL;
}


/**
 * Payment Record destructor.
 *
 * Payment Record destructor deletes class variables.
 *
 *
 * @return  
 * @author  Ivana
 * @see     
 */
PayRecord::~PayRecord()
{
  if( amount != NULL ){ DELETE(amount); }
  if( sealSAuth != NULL ){ DELETE(sealSAuth); }
}


/**
 * Returns eCode.
 *
 * Returns eCode - wheather the instance is consistent or not.
 *
 *
 * @return  eCode 
 * @author  Ivana
 * @see     
 */
Err PayRecord::IsOK()
{
  return eCode;
}


/**
 * Clears data.
 *
 * Sets class variables to initial values (including eCode).
 *
 *
 * @return  void
 * @author  Ivana
 * @see     
 */
Void PayRecord::Clear()
{
  eCode = OK;
  time.SetTime(0,0,0);
  payId = BAD_ID;
  if( amount != NULL ){ DELETE(amount); }
  if( sealSAuth != NULL ){ DELETE(sealSAuth); }
}


/**
 * Fetches Data From Table.
 *
 * Fetches data from given table which corresponds to given ID.
 * If whichId is not specified (is NULL) returns first record.
 *
 *
 * @param   t table in which we will search
 * @param   whichId name of ID we want to search
 * @param   value value of ID
 * @return  error code 
 * @author  Ivana
 * @see     
 */
Err PayRecord::FetchFromTable(Table *t, Char *whichId, ID value)
{
  Record *r=NULL;
  Err e;

  if( t==NULL || t->Open()!=OK ) return eCode = KO;

  if( whichId ) r = t->FindFirst(whichId, &value);
  else r = t->First();
  t->Close();

  e = FetchFromRecord(r);
  if( r ){ DELETE(r); }

  return ( e == OK ) ? OK : (eCode = KO);
}


/**
 * Fetches And Deletes Data From Table.
 *
 * Fetches and deletes data from given table which corresponds to given ID.
 * If whichId is not specified (is NULL) returns first record.
 *
 *
 * @param   t table in which we will search
 * @param   whichId name of ID we want to search
 * @param   value value of ID
 * @return  error code 
 * @author  Ivana
 * @see     
 */
Err PayRecord::FetchAndDeleteFromTable(Table *t, Char *whichId, ID value)
{
  Record *r=NULL;
  Err e;

  if( t==NULL || t->Open()!=OK ) return eCode = KO;

  if( whichId ) r = t->FindFirst(whichId, &value);
  else r = t->First();
  if( r == NULL ){
    t->Close();
    return eCode = KO;
  }
  t->DeleteRecord();
  t->Close();

  e = FetchFromRecord(r);
  DELETE(r); 

  if( e != OK ) return eCode = KO;
  return OK;
}


/**
 * Writes a record to given table.
 *
 * Writes a record to given table.
 *
 *
 * @param   t table into which we will write 
 * @return  error code 
 * @author  Ivana
 * @see     
 */
Err PayRecord::WriteToTable(Table *t)
{
  Record *r=NULL;
  Err e;

  if( t==NULL || t->Open()!=OK ) return KO;

  if( (r = t->NewRecord()) == NULL ){
    t->Close();
    return KO;
  }

  (Void)FillRecord(r);
  e = t->InsertRecord(r);
  e = (t->Close() == OK) ? e : KO;
  DELETE(r);

  return e;
}


/**
 * Updates a record in given table.
 *
 * Updates a record in given table.
 *
 *
 * @param   t table in which we will update
 * @return  error code 
 * @author  Ivana
 * @see     
 */
Err PayRecord::UpdateTable(Table *t)
{
  Record *r=NULL;
  Err e;

  if( t==NULL || t->Open()!=OK ) return KO;

  if( (r = t->FindFirst(TF_PAY_ID, &payId)) == NULL ){
    t->Close();
    return KO;
  }

  (Void)FillRecord(r);
  e = t->UpdateRecord(r);
  e = (t->Close() == OK) ? e : KO;
  DELETE(r);

  return e;
}
