#include <stdio.h>
#include "../GMessage/H/fieldQueue.h"

void main()
{
  TableFieldQueue *fq, *fq2;
  Field *f;

  printf("Creating queue...\n");
	fq = new TableFieldQueue();

  printf("Fillng queue with fields...\n");
  fq->Insert( new IntField("Prvni int",1) );
  fq->Insert( new StringField("Prvni str","2") );
  fq->Insert( new IntField("Druhy int",3) );
  fq->Insert( new StringField("Druhy str","4") );

  printf("Copying queue...\n");
  fq2 = new TableFieldQueue(*fq);
  
	printf("Deleting original queue...\n");
	DELETE(fq);
  
	printf("Getting fields from new...\n");
  f = fq2->First();
  while( f != NULL )
  {
    printf("%s\n", f->GetName());
    f = fq2->Next();
  }

  printf("Deleting new queue...\n");
  DELETE(fq2);
}

