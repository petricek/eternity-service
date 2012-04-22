CXXFLAGS= -g -Wall

finder: testFinder.o ../finder.o
	g++ -g -Wall -o finder testFinder.o \
	  ../finder.o \
	  ../../../Common/Debugable/debugable.o \
	  ../../../Common/GMessage/gMessageAll.o \
          ../../../Common/RandomGenerator/randomGenerator.o \
	  ../../../Common/LogFile/logFile.o \
          ../../../Common/RSAEuro/librsaeuro.a

clean:
	rm testFinder.o
