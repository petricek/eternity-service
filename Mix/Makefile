#
# Makefile for Mix
#

include ./../makefile.config
include ./makefile.config

EVERYTHING= DoFirst \
	AddressManager \
	Chopper \
	Linker \
	Majordomo \
	MessageCreator \
	Mix \
	MixOffspring \
	Padder \
	PaddingGenerator \
	Scheduler \
        Translator \
	../Common/CertificateRepository \
	../Common/Cipherer \
	../Common/ConfigFile \
	../Common/Debugable \
	../Common/GMessage \
	../Common/KeyManager \
	../Common/Killer \
	../Common/LogFile \
	../Common/MessageIDRepository \
	../Common/MessageQueue \
	../Common/Receiver \
	../Common/Runable \
	../Common/RunningObject \
	../Common/Semaphore \
	../Common/Sender \
	../Common/Shutdownable \
	../Common/SignalManager \
	../Common/Utils \
	mix

.PHONY: all $(EVERYTHING) clean

all: $(EVERYTHING)	

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.core" -exec rm {} \;
	-rm ./mix

SEP=--------------------------------------
SEP2=========================
SEP3==================================

DoFirst:
	@echo $(SEP)$(SEP)
	@echo $(SEP2) I am working on Mix now... $(SEP2)
	@echo $(SEP)$(SEP)
	@echo

../Common/Shutdownable:
	cd ./$(@); $(MAKE)

../Common/KeyManager:
	cd ./$(@); $(MAKE) 

../Common/Killer:
	cd ./$(@); $(MAKE)

../Common/LogFile:
	cd ./$(@); $(MAKE)

../Common/ConfigFile:
	cd ./$(@); $(MAKE)

../Common/Semaphore:
	cd ./$(@); $(MAKE) 

../Common/SignalManager:
	cd ./$(@); $(MAKE) 

../Common/Utils:
	cd ./$(@); $(MAKE) 

../Common/RunningObject:
	cd ./$(@); $(MAKE) 


../Common/Receiver:
	cd ./$(@); $(MAKE) 

../Common/Sender:
	cd ./$(@); $(MAKE) 

../Common/Cipherer:
	cd ./$(@); $(MAKE) 

../Common/MessageQueue:
	cd ./$(@); $(MAKE) 

../Common/GMessage:
	cd ./$(@); $(MAKE) 

../Common/CertificateRepository:
	cd ./$(@); $(MAKE) 

../Common/Runable:
	cd ./$(@); $(MAKE) 

../Common/MessageIDRepository:
	cd ./$(@); $(MAKE) 

../Common/Debugable:
	cd ./$(@); $(MAKE) 

AddressManager:
	cd ./$(@); $(MAKE) 

MessageCreator:
	cd ./$(@); $(MAKE) 

MixOffspring:
	cd ./$(@); $(MAKE) 

Chopper:
	cd ./$(@); $(MAKE) 

Linker:
	cd ./$(@); $(MAKE) 

Majordomo:
	cd ./$(@); $(MAKE) 

Mix:
	cd ./$(@); $(MAKE) 

PaddingGenerator:
	cd ./$(@); $(MAKE) 

Padder:
	cd ./$(@); $(MAKE) 

Translator:
	cd ./$(@); $(MAKE) 

mix	: ./main.cc \
	./Mix/mixAll.o \
	./Padder/padderAll.o \
	./PaddingGenerator/paddingGeneratorAll.o \
	./Translator/translatorAll.o \
	./AddressManager/addressManagerAll.o \
	./Chopper/chopperAll.o \
	./MessageCreator/messageCreatorAll.o \
	./../Common/GMessage/gMessageAll.o \
	./../Common/Receiver/receiverAll.o \
	./../Common/Sender/senderAll.o \
	./../Common/LogFile/logFileAll.o \
	./../Common/ConfigFile/configFileAll.o \
	./../Common/SignalManager/signalManagerAll.o \
	./../Common/Cipherer/ciphererAll.o \
	./../Common/CertificateRepository/certificateRepositoryAll.o \
	./../Common/MessageIDRepository/messageIDRepositoryAll.o \
	./../Common/Runable/runableAll.o \
	./../Common/RunningObject/runningObjectAll.o \
	./../Common/Shutdownable/shutdownableAll.o \
	./../Common/Debugable/debugableAll.o \
	./../Common/MessageQueue/messageQueueAll.o \
	./../Common/Semaphore/semaphoreAll.o \
	./../Common/Utils/utilsAll.o \
	./../Common/KeyManager/keyManagerAll.o \
	./../Common/Killer/killerAll.o \
	./MixOffspring/mixOffspringAll.o \
	./Chopper/chopperAll.o \
	./Linker/linkerAll.o ./Majordomo/majordomoAll.o

	@echo $(SEP)$(SEP)
	@echo $(SEP2)  I am creating Mix NOW !!!  $(SEP2)=
	@echo $(SEP)$(SEP)
	$(CXX) $(CXXFLAGS) -o $(@) $(^) \
	  -L/home/eternity/Source/Common/RSAEuro \
          -l$(STD) -l$(MATH) -l$(RSAEURO) -l$(THREADS)
	@echo $(SEP)$(SEP)
	@echo $(SEP2)======= Mix done !!! $(SEP2)=======
	@echo $(SEP)$(SEP)

# end of Makefile for Mix
