#
# Makefile for Mix
#

include ./../makefile.config
include ./makefile.config

ALLSOURCES= doFirst \
            Majordomo \
	    Client \
	    Banker \
	    ../Common/ACManager \
	    ../Common/CertificateRepository \
	    ../Common/Cipherer \
	    ../Common/ConfigFile \
	    ../Common/Debugable \
            ../Common/GMessage \
	    ../Common/Killer \
	    ../Common/LogFile \
	    ../Common/MessageQueue \
	    ../Common/Payments \
	    ../Common/RandomGenerator \
	    ../Common/Receiver \
	    ../Common/Runable \
	    ../Common/RunningObject \
	    ../Common/Semaphore \
	    ../Common/Sender \
	    ../Common/Shutdownable \
	    ../Common/TransactionManager \
	    ../Common/UidGenerator \
	    ../Common/Utils \
	    ../Six \
	    client

.PHONY:	$(ALLSOURCES) clean
all:	$(ALLSOURCES) 

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.core" -exec rm {} \;
	-rm client

SEP=--------------------------------------
SP=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SEP2========================
SEP3==================================

DoFirst:
	@echo $(SEP)$(SEP)
	@echo $(SEP2) I am working on Client now... $(SEP2)
	@echo $(SEP)$(SEP)
	@echo
	@sleep 1

Client:
	cd ./$(@); $(MAKE) 

Majordomo:
	cd ./$(@); $(MAKE) 

Banker:
	cd ./$(@); $(MAKE) 

../Common/ACManager:
	cd ./$(@); $(MAKE) 

../Common/CertificateRepository:
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

../Six:
	cd ./$(@); $(MAKE) 

client	: ./main.cc \
	./../Common/ACManager/acManagerAll.o \
	./../Common/CertificateRepository/certificateRepositoryAll.o \
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
	./../Six/Six/sixAll.o \
	./../Six/SixMajordomo/sixMajordomoAll.o \
	./../Six/SixOffspring/sixOffspringAll.o \
	./Majordomo/majordomoAll.o \
	./Banker/bankerAll.o \
	./Client/clientAll.o 
	
	@echo $(SEP)$(SEP)
	@echo $(SEP2) I am creating Client NOW !!! $(SEP2)
	@echo $(SEP)$(SEP)
	$(CXX) $(CXXFLAGS) -o $(@) $(^) \
	  -L/home/eternity/Source/Common/RSAEuro \
          -l$(STD) -l$(MATH) -l$(RSAEURO) -l$(THREADS)
	@echo $(SEP)$(SEP)
	@echo $(SEP2)====== Client done !!! $(SEP2)=======
	@echo $(SEP)$(SEP)

