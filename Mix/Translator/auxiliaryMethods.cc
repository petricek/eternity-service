  // has to be the FIRST include !
#include "./../../Common/H/common.h"

#include "./H/translator.h"

/**
 * Padds rest of path field to the original length.
 *
 * When the chunk goes through the chain of Mixes, the rest of path
 * field is downsizing. This fact could be used to try the onion
 * length attack and recognize, how many hops the chunk is to go yet.
 * The enemy then could try to chose what is easier to break (the
 * Mixes that are close).
 *
 * @param   path Path to be padded.
 * @param   pathLen Current path length.
 * @param   peelLen Length to which to padd the path.
 * @return  Padded rest of path.
 * @author  Pechy
 * @see     Translator, onion length attack discussion
 */
char *
Translator::PaddRestOfPath(char *path, Size pathLen, Size peelLen)
{
  char *newRestOfPath = new char[pathLen];

  memcpy(newRestOfPath, path + peelLen + sizeof(Size), 
         pathLen - peelLen - sizeof(Size)); 

  for (Size i = 0; i < peelLen + sizeof(Size); ++i)
    newRestOfPath[pathLen + sizeof(Size) - peelLen + i] = 
      mix->cipherer->GenerateRandomByte();

  return newRestOfPath; 
}
