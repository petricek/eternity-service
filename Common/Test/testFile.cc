#include "../GMessage/H/file.h"

#define TEXT "Cau, vole. Jak se ti vede? U me je to celkem fajn, jen kdyby uz to bylo odladeny - to by se mi ted libilo asi ze vseho nejvic."

int main()
{
	File *F;
  char *Buff;

	Buff = ( char* )malloc(250);
  strcpy( Buff, "123456789012345678901234567890" );
//  strcpy( Buff, TEXT );

	F = new File( "Pepa.txt", "r+" );

/*	if( F->Rename( "Pepa1.txt") )
		printf( "Rename OK\n" );
  printf( "  Error status: %d\n", F->GetError() );
 */
	if( F->Open() )
		printf( "Open OK\n" );
  printf( "  Error status: %d\n", F->GetError() );

  F->ReadBuffer(Buff, 10);
/*
	if( F->WriteBuffer( TEXT, strlen(TEXT) ) )
		printf( "WriteBuffer OK\n" );
  printf( "  Error status: %d\n", F->GetError() );

  F->ReadBuffer(Buff, 10);
*/
/*
	if( F->SeekStart() )
		printf( "Seek OK\n" );
  printf( "  Error status: %d\n", F->GetError() );
*/
  printf( "Copying first 10 bytes to Pepina.txt...\n" );
  File *P = new File("Pepina.txt","w");
  P->Open();
  F->CopyStartToPos(P, 10);
  F->SeekEnd(-20);
  P->WriteBuffer("xxx",3);
  F->AppendTheRestTo(P);
  DELETE(P);


  F->ReadBuffer(Buff, 10);

	if( F->ReadBuffer( Buff, 10 ) )
		printf( "ReadBuffer <%s> OK\n", Buff );
  printf( "  Error status: %d\n", F->GetError() );

	if( F->Close() )
		printf( "Close OK\n" );
  printf( "  Error status: %d\n", F->GetError() );

  free(Buff);
  DELETE(F);

	return 0;
}

