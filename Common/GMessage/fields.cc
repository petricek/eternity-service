#include <string.h>

#include "./H/fields.h"
#include "./H/myString.h"


/**
 * Constructor of Field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Field::Field(Char *aName, LogFile *aLogFile)
:Debugable(aLogFile)
{
  name = NULL;
  SetName(aName);
}


/**
 * Destructor of Field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Field::~Field()
{
  if(name != NULL)
    DELETE(name);
}


/**
 * Sets the name of Field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Void Field::SetName(Char *aName)
{
  if(aName != NULL)
  {
    if(name != NULL)
      DELETE(name);

    name = new Char[MyStrLen(aName)+1];
    strcpy(name, aName);
  }
}


/**
 * Returns name of Field.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Char* Field::GetName()
{
  Char *c;
  c = new Char[MyStrLen(name) + 1 ];
  strcpy(c, name);
  return c;
}


/**
 * Decides if the name string is equal to the parameter.
 *
 * Dlouhy_komentar
 *
 *
 * @param   parametr
 * @return  co
 * @author  Marek, Pavel
 * @see     co_se_toho_tyka
 */
Int Field::NameEqualTo(Char *aName)
{
  return (strcmp(aName, name) == 0);
}


