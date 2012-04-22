#include "./../Payments/H/payDefines.h"
#include "./H/createTables.h"

/**
 * Creates a table of payment plan sent by Eso to Client.
 *
 * Creates a table definition and then the table of payment
 * plan sent by Eso to Client.
 *
 *
 * @param   fileName  File name in which to store table.
 * @return  Pointer to generated table.
 * @author  Ivanka
 * @see     Banker, Eso, Client
 */
Table *CreateEsoToClientTable(Char *fileName)
{
  TableFieldQueue *fq;
  Table *t;

  if( fileName == NULL || (fq = new TableFieldQueue()) == NULL )
    return NULL;
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new IDField(TF_BANK_ACID, BAD_ID) );
  fq->Insert( new BytesField(TF_BANK_AC, NULL) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_SEAL_S_AUTH, NULL) );
  t = new Table(fileName, fq);
  DELETE(fq);
  return t;
}


/**
 * Creates a table of payment plan sent by Client to Bank.
 *
 * Creates a table definition and then the table of payment
 * plan sent by Client to Bank.
 *
 *
 * @param   fileName  File name in which to store table.
 * @return  Pointer to generated table.
 * @author  Ivanka
 * @see     Banker, Eso, Client
 */
Table *CreateClientToBankTable(Char *fileName)
{
  TableFieldQueue *fq;
  Table *t;

  if( fileName == NULL || (fq = new TableFieldQueue()) == NULL )
    return NULL;
  fq->Insert( new TimeField(TF_TIME) );
  fq->Insert( new IDField(TF_PAY_ID, BAD_ID) );
  fq->Insert( new StringField(TF_AMOUNT, NULL) );
  fq->Insert( new BytesField(TF_SEAL_S_AUTH, NULL) );
  fq->Insert( new BytesField(TF_O_AUTH, NULL) );
  fq->Insert( new BytesField(TF_MAC, NULL) );
  t = new Table(fileName, fq);
  DELETE(fq);
  return t;
}
