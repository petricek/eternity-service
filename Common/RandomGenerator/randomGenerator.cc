#include <stdlib.h>
#include "./../GMessage/H/gMessage.h"
#include "./../GMessage/H/file.h"
#include "./H/randomGenerator.h"

/**
 * Random generator constructor.
 *
 * It opens device /dev/random and initializes its seed using the
 * random data from it. /dev/random should provide us with suitable
 * strength in randomness - if evene stronger randomness is required
 * or bigger indenpendence on platform, we can modify this class to
 * cooperate with some hardware random generator. 
 *
 * @param   aLogFile   Log file for debug messages.
 * @return  
 * @author  Pavel
 * @see     KeyManager
 */
RandomGenerator::RandomGenerator(LogFile *aLogFile)
:Debugable(aLogFile)
{
  File *randomFile;
  long int seed;

  randomFile = new File("/dev/urandom", "r");
  if(randomFile == NULL)
    return;
  if(randomFile->Open() != OK)
    ;//WriteString(llWarning, __FILE__":%d:Cannot open /dev/urandom", __LINE__);
  else if(randomFile->ReadBuffer(&seed, sizeof(long int)) != sizeof(long int))
    ;//WriteString(llWarning, __FILE__":%d:Cannot read from /dev/urandom", __LINE__);

  /// work around 2.2.6 release bug
  time(&seed);
  DELETE(randomFile);

  srand((unsigned int)seed);
}


/**
 * Generate random bytes.
 *
 * Generates count bytes of random data and returns it in a MsgField.
 * Used to generate data for padding size of messages and also for
 * generating symetric and asymetric keys.
 *
 * @param   count   Number of bytes to generate
 * @return  MsgField filled with number random bytes
 * @author  Pavel
 * @see     KeyManager, Cipherer
 */
MsgField *RandomGenerator::RandomBytes(Size count)
{
  Size sum = 0;
  Int randomInt;
  MsgField *aMsgField;
  Char *buffer;
  Char *tempName;
  File *tempFile;

#define HOW_MUCH ((sum + sizeof(Int)) > count ? sizeof(char) : sizeof(Int))
#define TAKE_BYTES_FIELD (MAX_BYTE_MSG_FIELD_SIZE >= count)

  // choose memory or disk storage
  if(TAKE_BYTES_FIELD)
  {
    buffer = new Char[count];
    if(buffer == NULL)
      return NULL;
  }
  else
  {
    tempName = CreateTempFileName(RANDOM_PATH);
    if(tempName == NULL)
      return NULL;
    tempFile = new File(tempName, "wb");
    if(tempFile == NULL)
    {
      DELETE(tempName);
      return NULL;
    }
    if(tempFile->Open() == KO)
    {
      WriteString(llError, "RandomGenerator::RandomBytes(): cannot create temp file %s", tempName);
      DELETE(tempFile);
      DELETE(tempName);
      return NULL;
    }
  }

    // generate enoug bytes
  while(sum < count)
  {
    randomInt = rand();
    if(TAKE_BYTES_FIELD)
      memcpy(buffer + sum, &randomInt, HOW_MUCH);
    else
      if((size_t)tempFile->WriteBuffer(&randomInt, HOW_MUCH) != HOW_MUCH)
      {
        WriteString(llError, "RandomGenerator::RandomBytes(): cannot write to file %s", tempName);
        WriteString(llDebug, "  randomInt: %u", randomInt);
        WriteString(llDebug, "  HOW_MUCH: %d", HOW_MUCH);
        WriteString(llDebug, "  sum: %u", sum);
        tempFile->Remove();
        DELETE(tempFile);
        DELETE(tempName);
        return NULL;
      }
    sum += HOW_MUCH;
  }

    // get the information into a MsgField
  if(TAKE_BYTES_FIELD)
  {
    aMsgField = new BytesMsgField("random", buffer, count);
    DELETE(buffer);
  }
  else
  {
    tempFile->Close();
    aMsgField = new FileMsgField("random", tempName);
    tempFile->Remove();
    DELETE(tempName);
    DELETE(tempFile);
  }

  return aMsgField;

#undef TAKE_BYTES_FIELD
#undef HOW_MUCH
}


/**
 * Generate random Size.
 *
 * Generates Size of random data and returns it.
 * Used to generate random and unique ID's.
 *
 * @return  Size filled with random data
 * @author  Pavel
 * @see     KeyManager, Cipherer, UniqueIDGenerator
 *
 */
Size RandomGenerator::RandomSize()
{
  Size aSize;
  MsgField *tmpFld = RandomBytes(sizeof(Size));
  Void *tmp = tmpFld->GetAsBytes(&aSize);
  aSize = *(Size *)tmp;
  DELETE(tmp);
  DELETE(tmpFld);

  return aSize;
}
