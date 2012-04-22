#
# Makefile for Mix
#

include ./../makefile.config
include ./makefile.config

VALIDONLY= DoFirst \
	   Allocator \
           Banker \
           Finder \
	   Eso \
	   Majordomo \
           Scheduler \
           TimeSynchronizer \
	   TCBWrapper \
	   ../Six \
	   ../Common/ACManager \
	   ../Common/Debugable \
	   ../Common/Cipherer \
	   ../Common/Payments \
	   ../Common/Utils \
	   ../Common/RandomGenerator \
	   ../Common/GMessage \
	   ../Common/MessageQueue \
	   ../Common/Shutdownable \
	   ../Common/Killer \
	   ../Common/ConfigFile \
	   ../Common/LogFile \
	   ../Common/Semaphore \
	   ../Common/Sender \
	   ../Common/Receiver \
	   ../Common/Runable \
	   ../Common/RunningObject\
	   ../Common/TransactionManager \
	   ../Common/UidGenerator \
	   eso
ALLSOURCES= $(VALIDONLY) 

.PHONY:	all clean DoFirst $(ALLSOURCES) 

valid:	$(VALIDONLY)

all:	$(ALLSOURCES)

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.core" -exec rm {} \;
	-rm eso

SEP=--------------------------------------
SP=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SEP2=========================
SEP3==================================

DoFirst:
	@echo $(SEP)$(SEP)
	@echo $(SEP2) I am working on Eso now... $(SEP2)
	@echo $(SEP)$(SEP)
	@echo
	@sleep 1

Allocator:
	cd ./$(@); $(MAKE) 

Banker:
	cd ./$(@); $(MAKE) 

Majordomo:
	cd ./$(@); $(MAKE) 

Eso:
	cd ./$(@); $(MAKE) 

Finder:
	cd ./$(@); $(MAKE) 

Scheduler:
	cd ./$(@); $(MAKE) 

TimeSynchronizer:
	cd ./$(@); $(MAKE) 

TCBWrapper:
	cd ./$(@); $(MAKE) 

TransactionManager:
	cd ./$(@); $(MAKE) 

../Six:
	cd ./$(@); $(MAKE) 

../Common/ACManager:
	cd ./$(@); $(MAKE) 

../Common/Cipherer:
	cd ./$(@); $(MAKE) 

../Common/ConfigFile:
	cd ./$(@); $(MAKE) 

../Common/Debugable:
	cd ./$(@); $(MAKE) 

../Common/GMessage:
	cd ./$(@); $(MAKE) 

../Common/Killer:
	cd ./$(@); $(MAKE) 

../Common/LogFile:
	cd ./$(@); $(MAKE) 

../Common/MessageQueue:
	cd ./$(@); $(MAKE) 

../Common/Payments:
	cd ./$(@); $(MAKE) 

../Common/RandomGenerator:
	cd ./$(@); $(MAKE) 

../Common/Receiver:
	cd ./$(@); $(MAKE) 

../Common/Runable:
	cd ./$(@); $(MAKE) 

../Common/RunningObject:
	cd ./$(@); $(MAKE) 

../Common/Semaphore:
	cd ./$(@); $(MAKE) 

../Common/Sender:
	cd ./$(@); $(MAKE) 

../Common/Shutdownable:
	cd ./$(@); $(MAKE) 

../Common/TransactionManager:
	cd ./$(@); $(MAKE) 

../Common/UidGenerator:
	cd ./$(@); $(MAKE) 

../Common/Utils:
	cd ./$(@); $(MAKE) 

eso	: ./main.cc \
	./Banker/bankerAll.o \
	./Eso/esoAll.o \
	./Majordomo/majordomoAll.o \
	./../Six/Six/sixAll.o \
	./../Six/SixMajordomo/sixMajordomoAll.o \
	./../Six/SixOffspring/sixOffspringAll.o \
	./../Common/ACManager/acManagerAll.o \
	./../Common/Cipherer/ciphererAll.o \
	./../Common/ConfigFile/configFileAll.o \
	./../Common/Debugable/debugableAll.o \
	./../Common/GMessage/gMessageAll.o \
	./../Common/Killer/killerAll.o \
	./../Common/LogFile/logFileAll.o \
	./../Common/MessageQueue/messageQueueAll.o \
	./../Common/Payments/paymentsAll.o \
	./../Common/RandomGenerator/randomGeneratorAll.o \
	./../Common/Receiver/receiverAll.o \
	./../Common/Runable/runableAll.o \
	./../Common/RunningObject/runningObjectAll.o \
	./../Common/Semaphore/semaphoreAll.o \
	./../Common/Sender/senderAll.o \
	./../Common/Shutdownable/shutdownableAll.o \
	./../Common/TransactionManager/transactionManagerAll.o \
	./../Common/UidGenerator/uidGeneratorAll.o \
	./../Common/Utils/utilsAll.o \
	./Finder/finderAll.o \
	./Allocator/allocatorAll.o \
	./TCBWrapper/tcbWrapperAll.o \
	./TimeSynchronizer/timesynchAll.o \
	./Scheduler/schedulerAll.o 

	@echo $(SEP)$(SEP)
	@echo $(SEP2)  I am creating Eso NOW !!!  $(SEP2)=
	@echo $(SEP)$(SEP)
	$(CXX) $(CFLAGS) -o $(@) $(^) \
	  -L/home/eternity/Source/Common/RSAEuro \
          -l$(STD) -l$(MATH) -l$(RSAEURO) -l$(THREADS)
	@echo $(SEP)$(SEP)
	@echo $(SEP2)======= Eso done !!! $(SEP2)=======
	@echo $(SEP)$(SEP)

