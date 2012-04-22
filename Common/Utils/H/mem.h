#ifndef MEM_H
#define MEM_H

  // has to be the FIRST include !
#include "./../../H/common.h"

/**
 * Join two memory areas into one.
 *
 * Allocates memmory and copies the two  regions in it.
 *
 * @author  Pechy
 *
 * @param one first region of memory
 * @param oneLen its length
 * @param two second memory region
 * @param twoLe its lengthn
 *
 * @return allocated memory that contains copies of the two regions
 *
 * @see     MessageCreator, Cipherer, Translator
 */
int mixMemCat( char *one, int oneLen, char *two, int twoLen, 
               char **result);

/**
 * Encapsulates data in a siple way.
 *
 * Take the data and add a header indicating the lenght of data part,
 * the header is of type Size.
 *
 * @author  Pechy
 *
 * @param data data we want to act on 
 * @param dataLen lenght of data 
 *
 * @return allocated memory that contains the header and the data
 *
 * @see MessageCreator, Cipherer, Translator, 
 * ExtractDataFromSizeHeaderAnnotatedStream()
 *
 */
char *AddSizeHeaderAsDataSize(char *data, Size dataLen);

/**
 * Extracts data.
 *
 * Extracts data from the construct made by AddSizeHeaderAsDataSize()
 *
 * @author  Pechy
 *
 * @param dataIn data we want to extract something from
 * @param dataInLen lenght of dataIn
 * @param dataOutLen the len of returned data
 *
 * @return allocated memory that contains the data of header length
 *
 * @see MessageCreator, Cipherer, Translator, AddSizeHeaderAsDataSize()
 */
char *ExtractDataFromSizeHeaderAnnotatedStream(char *dataIn, Size dataInLen,
                                               Size *dataOutLen);

#endif
