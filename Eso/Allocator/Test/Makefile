CXXFLAGS= -g -Wall

allocator: testAllocator.o ../allocator.o
	g++ -g -Wall -o allocator testAllocator.o \
	  ../allocator.o \
	  ../../../Common/debugable.o \
	  ../../../Common/fields.o \
	  ../../../Common/msgField.o \
	  ../../../Common/gMessage.o \
	  ../../../Common/fieldQueue.o \
	  ../../../Common/queue.o \
	  ../../../Common/table.o \
	  ../../../Common/record.o \
	  ../../../Common/file.o \
	  ../../../Common/logFile.o \
	  ../../../Common/myString.o \
	  ../../../Common/randomGenerator.o \
	  ../../../Common/esotime.o \
          ../../../Mix/Cipherer/librsaeuro.a

clean:
	rm testAllocator.o
