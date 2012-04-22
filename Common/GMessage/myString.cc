#include "./H/myString.h"

/**
 * Safe strlen().
 *
 * Modifies strlen() to accept NULL argument (and return 0 in such a 
 * case)
 * <p>
 * <em>USAGE STRONGLY RECOMMENDED</em>
 *
 * @param   string pointer to string that we want to measure.
 * @author  Marek
 * @see     DELETE()
 */
int MyStrLen(const char *string)
{
  return ((string == NULL) ? 0 : strlen(string));
}


/**
 * Modified strcpy().
 *
 * WARNING!! Doesn't check the sizes of c1/c2
 *
 * @param c1           destination
 * @param c2           source
 * @param len          number of bytes to copy
 * @param firstPos_c1  index of the first byte to copy (in c1)
 * @param firstPos_c2  index of the first position being overwritten (in c2)
 *
 * @author  Marek
 * @see     co_se_toho_tyka
 *
 */
Char *MyStrCpy(Char *c1, Char *c2, Size len,
               Int firstPos_c1, Int firstPos_c2)
{
  Size i;
  for(i = 0; len > i; i++){
    c1[i + firstPos_c1] = c2[i + firstPos_c2];
  }
  return c1;
}

/**
 * Modified MyStrCmp().
 *
 * WARNING!! Doesn't check the sizes of c1/c2
 *
 * @author  Marek
 * @see     
 * @return 1, if the first len bytes of c1 and c2 are identical,
 *         0 otherwise
 *
 */
Int MyStrCmp(Char *c1, Char *c2, Size len)
{
  Size i, j;

  j = 1;
  for(i = 0; len > i; i++){
    j &= (c1[i] == c2[i]);
  }
  return j;
}


/**
 * Find char in string.
 *
 * Finds the first occurance of aChar in first len bytes of aBuffer &
 * returns it as Size. 0 otherwise
 *
 * @return  position of aChar inside first len bytes of aBuffer or 0
 *          if not found.
 * @param   aBuffer String that should be examined.
 * @param   aChar   Charcter to serch for.
 * @param   len     Number of bytes to search.
 * @author  Marek
 * @see     
 */
Size MyStrChar(Char *aBuffer, Char aChar, Size len)
{
  Size i;

  for(i = 0; i < len; i++){
    if(aBuffer[i] == aChar)
      return i;
  }

  return 0;
}
