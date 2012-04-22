#ifndef CREATE_TABLES_H
#define CREATE_TABLES_H

#include "./../../GMessage/H/table.h"

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
Table *CreateEsoToClientTable(Char *fileName);

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
Table *CreateClientToBankTable(Char *fileName);

#endif
