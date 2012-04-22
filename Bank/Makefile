#
# Makefile for bank
#

include ./../makefile.config
include ./makefile.config

VALIDONLY= DoFirst \
	   Bank \
	   KeyManager \
	   PayManager \
	   Majordomo \
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
	   ../Common/RunningObject \
	   ../Common/TransactionManager \
	   ../Common/UidGenerator \
	   ../Common/Utils \
	   bank
ALLSOURCES= $(VALIDONLY) \
           TransactionManager 

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
	@echo $(SEP2) I am working on Bank now... $(SEP2)
	@echo $(SEP)$(SEP)
	@echo
	@sleep 1
Bank:
	cd ./$(@); $(MAKE) 

KeyManager:
	cd ./$(@); $(MAKE) 
	
PayManager:
	cd ./$(@); $(MAKE) 

Majordomo:
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

bank	: ./main.cc \
	./Bank/bankAll.o \
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
	./KeyManager/keyManagerAll.o \
	./PayManager/payManagerAll.o \

	@echo $(SEP)$(SEP)
	@echo $(SEP2)  I am creating Bank NOW !!!  $(SEP2)=
	@echo $(SEP)$(SEP)
	$(CXX) $(CFLAGS) -o $(@) $(^) \
	  -L/home/eternity/Source/Common/RSAEuro \
          -l$(STD) -l$(MATH) -l$(RSAEURO) -l$(THREADS)
	@echo $(SEP)$(SEP)
	@echo $(SEP2)======= Bank done !!! $(SEP2)=======
	@echo $(SEP)$(SEP)

