#ifndef ARRAY_GMESSAGE_H
#define ARRAY_GMESSAGE_H

#include "./gMessage.h"

/**
 * GMessage containing array.
 *
 * Descendand of GMessage that is able to handle arrays of similar values
 *
 * @author  Pavel
 * @see     GMessage
 */
class ArrayGMessage: public GMessage
{
protected:
  /**@name methods */
  /*@{*/
  Char *FieldName(Char *prefix, Int i);
  Char *CountName(Char *prefix);
  /*@}*/
public:
  /**@name methods */
  /*@{*/
  /// constructor
  ArrayGMessage(LogFile *aLogFile = NULL);
  Err SetField(Char *prefix, Int i, MsgField *contents);
  MsgField *GetField(Char *prefix, Int i);
  Err AddField(Char *prefix, MsgField *contents);
  Err LoadFromLines(char *prefix, Char *str);
  GMessage *GetAsGMessage(Char *prexif, Int i);
  /*@}*/
};

#endif
