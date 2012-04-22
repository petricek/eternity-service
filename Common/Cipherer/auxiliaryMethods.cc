  // has to be the FIRST include !
#include "./../H/common.h"

  // contains just #include of "rsaeuro.h"
#include "./../RSAEuro/H/rsaref.h"

#include "./H/cipherer.h"
#include "../H/labels.h"

/**
 * Returns GMessage full of symetric keys.
 *
 * When we need to create a new onion of a chunk, we have to use some
 * symetric keys. So this method gives them to us.
 *
 *
 * @param   hopCount How many keys do we need.
 * @param   parameters What types of keys do we need.
 * @return  GMessage full of symetric keys:
 * <pre>
 *   GM_SYMETRIC_KEY_COUNT
 *   GM_SYMETRIC_KEY_INFO_1
 *   ...
 *   ...
 *   GM_SYMETRIC_KEY_INFO_"hopCount"
 * </pre>
 *  
 * @author  Pechy
 * @see     other cryptography methods.
 */
GMessage *Cipherer::ReturnGMsgFullOfSymKeys(Size hopCount, 
                                            GMessage *parameters = NULL)
{
    // to shut up a warning message
  parameters = NULL;

  GMessage *symKeys = new GMessage();
  GMessage *oneSymKey;

  symKeys->SetAsBytes(GM_SYMETRIC_KEY_COUNT, (void *) &hopCount, sizeof(Size));

  for (unsigned int i = 0; i < hopCount; ++i) {

    string * symKeyName = CreateString(GM_SYMETRIC_KEY_INFO,
                              i + 1, NO_PARAM);
    symKeys->SetAsGMessage((char *) symKeyName->c_str(),
                           oneSymKey = GenerateSymetricKey());

    DELETE(symKeyName); DELETE(oneSymKey);
  }

  return symKeys;
}

/**
 * Very simple method for generation of one byte.
 *
 * Very simple method, uses standard random number generator, should be 
 * replaced in future by R_GenerateBytes(rsaeuro) and /dev/random at
 * least. Relies * on the fact that somebody has already seeded generator
 * with srandom(). For now, we seed srandom() with randomness got from
 * /dev/urandom (it's good !)
 *
 * @return  One random byte.
 * @author  Pechy
 * @see     other cryptography methods.
 */
char unsigned Cipherer::GenerateRandomByte()
{
  return ( (unsigned char) (random() & 255));
}

