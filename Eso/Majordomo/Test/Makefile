CXXFLAGS= -g -Wall

MAJORDOMOALL=../majordomoAll.o

ESO=../../Eso/esoAll.o \
    ../../EsoOffspring/esoOffspring.o \
    ../../Allocator/allocatorAll.o \
    ../../ACManager/acManagerAll.o \
    ../../TCBWrapper/tcbWrapperAll.o \
    ../../Finder/finderAll.o \
    ../../Banker/bankerAll.o \
    ../../Scheduler/scheduler.o

COMMON=../../../Common/Debugable/debugableAll.o \
       ../../../Common/GMessage/gMessageAll.o \
       ../../../Common/RandomGenerator/randomGenerator.o \
       ../../../Common/LogFile/logFile.o \
       ../../../Common/ConfigFile/configFile.o \
       ../../../Common/Runable/runable.o \
       ../../../Common/MessageQueue/messageQueue.o \
       ../../../Common/Cipherer/cipherer.o \
       ../../../Common/RSAEuro/librsaeuro.a  


majordomo: testMajordomo.o ../majordomoAll.o
	g++ -g -Wall -o testMajordomo testMajordomo.o \
          $(MAJORDOMOALL) \
	  $(ESO) \
	  $(COMMON)
clean:
	rm testACManager.o
