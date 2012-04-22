#
# Makefile for Mix
#

include ./../makefile.config
include ./makefile.config

.PHONY:	all \
	clean \
	Acs \
	../Common/Receiver \
	Majordomo \
	../Common/Sender \
	AcsOffspring \
	AddressManager \
	../Common/CertificateRepository \
	DoFirst \
	../Common/Killer \
	../Common/RunningObject \
	../Common/MessageQueue \
	../Common/Debugable \
	../Common/Runable \
	../Common/LogFile \
	../Common/ConfigFile \
	../Common/Semaphore \
	../Common/Shutdownable \
	../Common/GMessage \
	acs

all:	DoFirst \
	../Common/CertificateRepository \
	../Common/ConfigFile \
 	../Common/Debugable \
	../Common/GMessage \
	../Common/Killer \
	../Common/LogFile \
	../Common/MessageQueue \
	../Common/Receiver \
	../Common/Runable \
	../Common/RunningObject \
	../Common/Semaphore \
	../Common/Sender \
	../Common/Shutdownable \
	Acs \
	AcsOffspring \
	AddressManager \
	Majordomo \
	acs

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.core" -exec rm {} \;



SEP=----------------------------------------------------------------------------
SP=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SEP2=========================
SEP3==================================

DoFirst:
	@echo $(SEP)
	@echo $(SEP2) I am working on Acs now... $(SEP2)
	@echo $(SEP)
	@echo
	@sleep 1

Majordomo:
	cd ./$(@); $(MAKE) 

AcsOffspring:
	cd ./$(@); $(MAKE) 

AddressManager:
	cd ./$(@); $(MAKE) 

Acs:
	cd ./$(@); $(MAKE) 

../Common/RunningObject:
	cd ./../Common/$(@); $(MAKE) 

../Common/Receiver:
	cd ./../Common/$(@); $(MAKE) 

../Common/Semaphore:
	cd ./../Common/$(@); $(MAKE) 

../Common/GMessage:
	cd ./../Common/$(@); $(MAKE) 

../Common/Debugable:
	cd ./../Common/$(@); $(MAKE) 

../Common/LogFile:
	cd ./../Common/$(@); $(MAKE) 

../Common/ConfigFile:
	cd ./../Common/$(@); $(MAKE) 

../Common/Shutdownable:
	cd ./../Common/$(@); $(MAKE) 

../Common/Killer:
	cd ./../Common/$(@); $(MAKE) 

../Common/Sender:
	cd ./../Common/$(@); $(MAKE) 

../Common/MessageQueue:
	cd ./../Common/$(@); $(MAKE) 

../Common/Runable:
	cd ./../Common/$(@); $(MAKE) 

../Common/CertificateRepository:
	cd ./../Common/$(@); $(MAKE) 

acs	: ./main.cc \
	./Acs/acsAll.o  \
        ./Majordomo/majordomoAll.o \
        ./AcsOffspring/acsOffspringAll.o \
        ./AddressManager/addressManagerAll.o \
        ./../Common/Receiver/receiverAll.o \
        ./../Common/CertificateRepository/certificateRepositoryAll.o \
	./../Common/Sender/senderAll.o \
	./../Common/GMessage/gMessageAll.o \
	./../Common/Killer/killerAll.o \
	./../Common/RunningObject/runningObjectAll.o \
	./../Common/Shutdownable/shutdownableAll.o \
	./../Common/Runable/runableAll.o \
	./../Common/MessageQueue/messageQueueAll.o \
	./../Common/LogFile/logFileAll.o \
	./../Common/Semaphore/semaphoreAll.o \
	./../Common/ConfigFile/configFileAll.o \
        ./../Common/Debugable/debugableAll.o

	@echo $(SEP)
	@echo $(SEP2)  I am creating ACS NOW !!!  $(SEP2)=
	@echo $(SEP)
	$(CXX) $(CXXFLAGS) -o $(@) $(^) \
	  -L/home/eternity/Source/Common/RSAEuro \
          -l$(STD) -l$(RSAEURO) -l$(MATH) -l$(THREADS)
	@echo $(SEP)
	@echo $(SEP2)======= Acs done !!! $(SEP2)=======
	@echo $(SEP)

