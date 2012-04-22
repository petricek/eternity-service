/* R_RANDOM.C - random objects for RSAREF
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

  // added by Pechy, 29.1.1999 because of R_RandomCreate addition
#include <time.h>
#include <stdlib.h>

#include "global.h"
#include "rsaref.h"
#include "r_random.h"
#include "md5.h"

#define RANDOM_BYTES_NEEDED 256

int R_RandomInit (randomStruct)
R_RANDOM_STRUCT *randomStruct;                      /* new random structure */
{
  randomStruct->bytesNeeded = RANDOM_BYTES_NEEDED;
  R_memset ((POINTER)randomStruct->state, 0, sizeof (randomStruct->state));
  randomStruct->outputAvailable = 0;
  
  return (0);
}

int R_RandomUpdate (randomStruct, block, blockLen)
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
unsigned char *block;                          /* block of values to mix in */
unsigned int blockLen;                                   /* length of block */
{
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int i, x;
  
  MD5Init (&context);
  MD5Update (&context, block, blockLen);
  MD5Final (digest, &context);

  /* add digest to state */
  x = 0;
  for (i = 0; i < 16; i++) {
    x += randomStruct->state[15-i] + digest[15-i];
    randomStruct->state[15-i] = (unsigned char)x;
    x >>= 8;
  }
  
  if (randomStruct->bytesNeeded < blockLen)
    randomStruct->bytesNeeded = 0;
  else
    randomStruct->bytesNeeded -= blockLen;
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)digest, 0, sizeof (digest));
  x = 0;
  
  return (0);
}

int R_GetRandomBytesNeeded (bytesNeeded, randomStruct)
unsigned int *bytesNeeded;                 /* number of mix-in bytes needed */
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
{
  *bytesNeeded = randomStruct->bytesNeeded;
  
  return (0);
}

int R_GenerateBytes (block, blockLen, randomStruct)
unsigned char *block;                                              /* block */
unsigned int blockLen;                                   /* length of block */
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
{
  MD5_CTX context;
  unsigned int available, i;
  
  if (randomStruct->bytesNeeded)
    return (RE_NEED_RANDOM);
  
  available = randomStruct->outputAvailable;
  
  while (blockLen > available) {
    R_memcpy
      ((POINTER)block, (POINTER)&randomStruct->output[16-available],
       available);
    block += available;
    blockLen -= available;

    /* generate new output */
    MD5Init (&context);
    MD5Update (&context, randomStruct->state, 16);
    MD5Final (randomStruct->output, &context);
    available = 16;

    /* increment state */
    for (i = 0; i < 16; i++)
      if (randomStruct->state[15-i]++)
        break;
  }

  R_memcpy 
    ((POINTER)block, (POINTER)&randomStruct->output[16-available], blockLen);
  randomStruct->outputAvailable = available - blockLen;

  return (0);
}

void R_RandomFinal (randomStruct)
R_RANDOM_STRUCT *randomStruct;                          /* random structure */
{
  R_memset ((POINTER)randomStruct, 0, sizeof (*randomStruct));
}

/*---------------------------------------------------------------------------
 * WARNING
 *
 * This function is not a part of RSARef, was added by Pechy from RSAEuro
 *-------------------------------------------------------------------------*/

/* Create Random object, seed ready for use.
         Requires ANSI Standard time routines to provide seed data.
*/

#define RANDOM_BYTES_RQINT 512

void R_RandomCreate(random)
R_RANDOM_STRUCT *random;                                /* random structure */
{
        unsigned int bytes;
        clock_t cnow;
        time_t t;
        struct tm *gmt;

                        /* clear and setup object for seeding */
        R_memset((POINTER)random->state, 0, sizeof(random->state));
        random->outputAvailable = 0;
        random->bytesNeeded = RANDOM_BYTES_RQINT;  /* using internal value */

                /* Add data to random object */
        while(random->bytesNeeded) {
                t = time(NULL);                 /* use for seed data */
                gmt = gmtime(&t);
                cnow = clock();

                R_RandomUpdate(random, (POINTER)gmt, sizeof(struct tm));
        R_RandomUpdate(random, (POINTER)&cnow, sizeof(clock_t));
        }

        /* Clean Up time data */
        R_memset((POINTER)gmt, 0, sizeof(struct tm));
        cnow = 0;
        t = 0;
}

