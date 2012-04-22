#
# Makefile for Common
#

include ./../makefile.config
include ./makefile.config

EVERYTHING= \
        ACManager \
	CertificateRepository \
	Cipherer \
	ConfigFile \
	Debugable \
	GMessage \
	KeyManager \
	KeyManager \
	Killer \
	LogFile \
	MessageIDRepository \
	MessageQueue \
	Payments \
	RSAEuro \
	RandomGenerator \
	Receiver \
	Registerable \
	Rollbackable \
	Runable \
	Schedulable \
	Semaphore \
	Sender \
	Shutdownable \
	SignalManager \
	TransactionManager \
	UidGenerator \
	Utils 

.PHONY:	all clean DoFirst  $(EVERYTHING) DoLast 

all:	DoFirst $(EVERYTHING) DoLast 

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.core" -exec rm {} \;

SEP=------------------------------------
SEP2=======================
SEP3==================================

DoFirst:
	@echo $(SEP)--$(SEP)
	@echo $(SEP2) I am working on Common now... $(SEP2)
	@echo $(SEP)--$(SEP)
	@echo

ACManager:
	cd ./$(@); $(MAKE) 

CertificateRepository:
	cd ./$(@); $(MAKE) 

Cipherer:
	cd ./$(@); $(MAKE) 

ConfigFile:
	cd ./$(@); $(MAKE) 

GMessage:
	cd ./$(@); $(MAKE) 

Debugable:
	cd ./$(@); $(MAKE) 

KeyManager:
	cd ./$(@); $(MAKE)

Killer:
	cd ./$(@); $(MAKE)

LeakDetector:
	cd ./$(@); $(MAKE)

LogFile:
	cd ./$(@); $(MAKE) 

MessageIDRepository:
	cd ./$(@); $(MAKE) 

MessageQueue:
	cd ./$(@); $(MAKE) 

Payments:
	cd ./$(@); $(MAKE) 

RandomGenerator:
	cd ./$(@); $(MAKE) 

Receiver:
	cd ./$(@); $(MAKE) 

#Registerable:
#	cd ./$(@); $(MAKE) 

#Rollbackable:
#	cd ./$(@); $(MAKE) 

#RSAEuro:
#	cd ./$(@); $(MAKE) 

Runable:
	cd ./$(@); $(MAKE) 

RunningObject:
	cd ./$(@); $(MAKE) 

Semaphore:
	cd ./$(@); $(MAKE) 

Sender:
	cd ./$(@); $(MAKE) 

#Schedulable:
#	cd ./$(@); $(MAKE) 

Shutdownable:
	cd ./$(@); $(MAKE) 

SignalManager:
	cd ./$(@); $(MAKE) 

TransactionManager:
	cd ./$(@); $(MAKE) 

UidGenerator:
	cd ./$(@); $(MAKE) 

Utils:
	cd ./$(@); $(MAKE) 

DoLast: 
	@echo $(SEP)--$(SEP)
	@echo $(SEP2)======= Common done !! $(SEP2)=======
	@echo $(SEP)--$(SEP)

