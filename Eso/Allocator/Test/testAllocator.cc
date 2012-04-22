#include "./../H/allocator.h"


void PrintResult(Allocator *alctr, char *str, Err result)
{
  if(alctr == NULL)
    return;

  alctr->WriteString(llDebug, "%s: %s", str, result == KO ? "KO" : "OK");
}

void main()
{
  Err result;
  Allocator *alctr = new Allocator();

/*  result = alctr->AllocSpace(1L, 512L);
  PrintResult(alctr, "AllocSpace(1, 512)", result);
  result = alctr->AllocSpace(2L, 1023L);
  PrintResult(alctr, "AllocSpace(2, 1023)", result);
*/
  result = alctr->AllocValid(1L, 512L);
  PrintResult(alctr, "AllocValid(1L, 512L)", result);
/*  result = alctr->AllocValid(1L, 513L);
  PrintResult(alctr, "AllocValid(1L, 513L)", result);
  result = alctr->FreeSpace(1L);
  PrintResult(alctr, "FreeSpace(1L)", result);
*/
//  alctr->Test();

  DELETE(alctr);
}
