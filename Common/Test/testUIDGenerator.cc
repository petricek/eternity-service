#include "./../../Eso/UidGenerator/H/uidGenerator.h"

void main()
{
  UniqueIDGenerator *myIDGen;
  ID id;

  myIDGen = new UniqueIDGenerator("myIDTable", 10);
  if(myIDGen != NULL)
  {
    if(myIDGen->GetID(&id) == KO)
      printf("Cannot get ID.\n");
    else
      printf("Got ID %lu.\n", id);

    if(myIDGen->GetID(&id) == KO)
      printf("Cannot get ID.\n");
    else
      printf("Got ID %lu.\n", id);

    DELETE(myIDGen);
  }
}
