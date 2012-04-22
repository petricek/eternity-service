#ifndef KEYWORDS_H
#define KEYWORDS_H

#include "./../../../Common/GMessage/H/arrayGMessage.h"

/**
 * GMessage used for storing array of keywords.
 *
 * This class simply extends the functionality of ArrayGMessage,
 * making it simple to work with an array of keywords. In a fact
 * an ArrayGMessage using prefix GM_KEYWORD.
 *
 *
 * @author  Pavel
 * @see     ArrayGMessage, TCBWrapper, Client
 */
class Keywords: public ArrayGMessage
{
public:
  /**@name methods */
  /*@{*/
    /// Constructor.
  Keywords(LogFile *aLogFile = NULL);

    /// Method returning i-th keyword from an array.
  Char *Keyword(Int i);
    /// Given a pattern (search string) decides whether keywords match it.
  Int MatchesPattern(Char *pattern);
    /// Adds a keyword.
  Err AddKeyword(Char *keyword);
  /*@}*/
};

#endif
