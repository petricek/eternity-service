CXXFLAGS= -g -Wall

allocator: testACManager.o ../acManager.o
	g++ -g -Wall -o acManager testACManager.o \
	  ../acManager.o \
	  ../../../Common/Debugable/debugable.o \
	  ../../../Common/GMessage/gMessageAll.o \
          ../../../Common/RandomGenerator/randomGenerator.o \
	  ../../../Common/LogFile/logFile.o \
          ../../../Common/RSAEuro/librsaeuro.a

clean:
	rm testACManager.o
