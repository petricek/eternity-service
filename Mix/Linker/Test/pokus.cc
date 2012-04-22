#include "../Common/H/common.h"
#include <string>
#include <list>


#define MAX_PARTIAL_MESSAGES 100
#define CHUNK_LENGTH 1000

#include <vector>
#include <map>


//-------------------------------------------------------------------------
// PartialMessage
// ~~~~~~~~~~~~~~
// Represents all by now received chunks from message
//-------------------------------------------------------------------------
class PartialMessage
{
  public:
    vector<int> * presentChunks;
    PartialMessage();
};
PartialMessage::PartialMessage()
{
  presentChunks = new vector<int>;
}

void main()
{
      map<ID, PartialMessage *, less<ID> > partialMessages;
    
      partialMessages[0] = new PartialMessage();
      if(partialMessages[0] == NULL)
        printf("SCHEIZE");
      partialMessages[0]->presentChunks[0] = 0;
      
}

