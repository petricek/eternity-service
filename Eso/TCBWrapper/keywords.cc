#include "./../../Common/H/labels.h"
#include "./H/keywords.h"

/**
 * Constructor.
 *
 * Simply calls from ArrayGMessage inherited constructer.
 *
 *
 * @param   aLogFile log file for debugging messages
 * @author  Pavel
 * @see     ArrayGMessage
 */
Keywords::Keywords(LogFile *aLogFile)
: ArrayGMessage(aLogFile)
{
}


/**
 * Gets i-th keyword label.
 *
 * Creates and returns i-th label for keyword.
 *
 *
 * @param   i position of label to return
 * @return  created label
 * @author  Pavel
 * @see     
 */
Char *
Keywords::Keyword(Int i)
{
  MsgField *kwdFld = GetField(GM_KEYWORD, i);
  if(kwdFld == NULL)
    return NULL;

  Char *kwd = kwdFld->GetAsString();
  DELETE(kwdFld);

  return kwd;
}


/**
 * Decides wheter pattern matches keywords.
 *
 * This version accepts only a single word and looks, if
 * there is an exactly same keyword stored. This method is 
 * virtual and expects further modifications to accept
 * even more complex queries.
 *
 *
 * @param   pattern string with a query
 * @return  0 if successfull, non 0 otherwise
 * @author  Pavel
 * @see     
 */
Int 
Keywords::MatchesPattern(Char *pattern)
{
  if(pattern == NULL)
    return 0;
  
  Char *kwd = Keyword(1);
  Int i = 1;
  while((kwd != NULL) && (strcmp(kwd, pattern) != 0))
  {
    i++;
    DELETE(kwd);
    kwd = Keyword(i);
  }

  if(kwd == NULL)
  {
    fprintf(stderr, "K E Y W O R D S: MatchesPattern(): Pattern [%s] NOT FOUND\n", pattern);
    return 0;
  }
  else
  {
    fprintf(stderr, "K E Y W O R D S: MatchesPattern(): Pattern [%s] FOUND\n", pattern);
    DELETE(kwd);
    return 1;
  }
}


/**
 * Adds a keyword.
 *
 * Simply calls ArrayGMessage::AddField with converted
 * arguments.
 *
 *
 * @param   keyword keyword to add
 * @return  error code
 * @author  Pavel
 * @see     
 */
Err 
Keywords::AddKeyword(Char *keyword)
{
  if(keyword == NULL)
    return KO;

  MsgField *kwdFld = new BytesMsgField();
  kwdFld->SetAsString(keyword);
  Err result = AddField(GM_KEYWORD, kwdFld);
  DELETE(kwdFld);
  
  return result;
}
