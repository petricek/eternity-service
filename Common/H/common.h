#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

  // *NEEDED* !!! when we use errno in threads, it is needed to use a
  // special makro errno instead of normal variable errno (all
  // treads could access errno -> race condition)
  /// We want to be safe in the threaded environment
#define _THREAD_SAFE

  /// this will be the first field in the message as it goes through the Net
#define MIX_MIX_PROTOCOL_VERSION "Mix-Mix-alfa"

#ifndef NULL
  /// Invalid or uninitialized pointer
#define NULL 0
#endif

  /// Extern errno variable
extern int errno;

  /// Numeric equivalent to NULL
#define NO_PARAM -1
  /// Maximum length of count of certificates in GMessage
#define DIGITS_IN_NUMBER 3

  /**@name Boolean values */
  /*@{*/
  /// Positive answer
#define YES 1
  /// Negative answer
#define NO 0
  /*@}*/

  // use Unix semantics for error notification
  /**@name Error codes */
  /*@{*/
  // Ok
#define OK 0
  // Not ok
#define KO 1
  /*@}*/


  /** Safe delete. 
   *  For debugging purpose, delete pointer and set it to NULL
   *  Complain to stderr if it is already NULL 
   */
#define DELETE(x) {if (x) {delete(x); x = NULL;} \
                  else {fprintf(stderr, "NULL deleted, file "\
		  __FILE__", line %d!\n", __LINE__);}}

  // it is not good place for it... (chopper, translator, 
  // messageCreator are using it)
  /// Length of symetric key encrypted using asymetric cipher
#define ASYMETRICALLY_ENCODED_SYMETRIC_KEY_LEN 64 

  /**@name Generic types for portability */
/*@{*/
  /// Number
#define Int int
  /// character
#define Char char
  /// Real number
#define Double double
  /// Long number
#define Long long
  /// Empty type
#define Void void
/*@}*/

  /**@name Some special types */
/*@{*/
  /// Size of something, length, count
typedef unsigned long Size;
  /// Error code type
typedef int Err;
/*@}*/

  /**@name Generic types various identification numbers */
/*@{*/
  /// ID
typedef unsigned long ID;
  /// Transaction ID
typedef unsigned long TID;
  /// Access certificate ID
typedef unsigned long ACID;
/*@}*/

#endif
