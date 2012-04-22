#include <stdexcept>
#include <stdio.h>

// -------------------------------------------------------------------------
// ExceptionThrower()
// ~~~~~~~~~~~~~~~~~~
// This function throws an exception
// -------------------------------------------------------------------------
static void ExceptionThrower()  { 
  
  // throws an exception of type runtime_error
  throw runtime_error("Chyba nechyba"); 

}

// -------------------------------------------------------------------------
// main()
// ~~~~~~
// Calls ExceptionThrower and catches the exception that it throws
// -------------------------------------------------------------------------
main()
{
  try{

    // Throws exception
    ExceptionThrower();

  }
  // This statement catches al exceptions of type exception and of all
  // subtypes runtime_error
  catch(const exception &e){

    // Displays the string that was used to create the axception
    printf("Exceptiona: %s\n", e.what());

  }
}
