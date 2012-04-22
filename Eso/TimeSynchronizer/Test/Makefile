CXXFLAGS= -Wall -g

SQUEUE=testSQ.cc \
	../../../Common/GMessage/queue.o \
	../../../Common/GMessage/myString.o \
	../../../Common/GMessage/estime.o \
	../squeue.o

#	./Banker/bankerAll.o \
#	./Eso/esoAll.o \
#	./Majordomo/majordomoAll.o \
#	./UidGenerator/uidGeneratorAll.o \
#	./../../../Common/Payments/paymentsAll.o \
#	./Finder/finderAll.o \
#	./Allocator/allocatorAll.o \
#	./TCBWrapper/tcbWrapperAll.o \

tsALL=testTimeSynch.cc \
	./../../../Common/Sender/senderAll.o \
	./../../../Common/Cipherer/ciphererAll.o \
	./../../../Common/RandomGenerator/randomGeneratorAll.o \
	./../../../Common/Utils/utilsAll.o \
	./../../../Common/Receiver/receiverAll.o \
	./../../../Common/RunningObject/runningObjectAll.o \
	  ../../../Common/ACManager/acManagerAll.o \
          ../../../Common/GMessage/gMessageAll.o \
          ../../../Common/MessageQueue/messageQueueAll.o \
          ../../../Common/LogFile/logFileAll.o \
          ../../../Common/Semaphore/semaphoreAll.o \
          ../../../Common/Killer/killerAll.o \
          ../../../Common/ConfigFile/configFileAll.o \
          ../../../Common/Debugable/debugableAll.o \
          ../../../Common/Runable/runableAll.o \
          ../../../Common/Shutdownable/shutdownableAll.o \
	  ../../../Six/SixMajordomo/sixMajordomoAll.o \
	  ../../../Six/SixOffspring/sixOffspringAll.o \
	  ../../../Six/Six/sixAll.o \
	  ../../UidGenerator/uidGeneratorAll.o \
	  ../../Finder/finderAll.o \
	  ../../Allocator/allocator.o \
	  ../../Scheduler/schedulerAll.o \
	  ../../Majordomo/majordomoAll.o \
	  ../timesynch.o \
	  ../squeue.o \
          ../../../Common/RSAEuro/librsaeuro.a

tsALLfirst=testTimeSynch.cc \
          ../../../Common/GMessage/estime.cc \
          ../../../Common/MessageQueue/messageQueue.cc \
          ../../../Common/GMessage/msgField.cc \
          ../../../Common/GMessage/myString.cc \
          ../../../Common/GMessage/queue.cc \
          ../../../Common/GMessage/record.cc \
          ../../../Common/GMessage/fieldQueue.cc \
          ../../../Common/GMessage/fields.cc \
	  ../../../Common/GMessage/timeField.cc \
	  ../../../Common/GMessage/bytesField.cc \
	  ../../../Common/GMessage/intField.cc \
	  ../../../Common/GMessage/idField.cc \
	  ../../../Common/GMessage/stringField.cc \
          ../../../Common/GMessage/file.cc \
          ../../../Common/GMessage/gMessage.cc \
          ../../../Common/GMessage/table.cc \
          ../../../Common/LogFile/logFile.cc \
	  ../timesynch.cc \
	  ../squeue.cc \
	  ../../UidGenerator/uidGenerator.cc \
	  ../../../Common/ACManager/acManager.cc \
	  ../../Scheduler/scheduler.cc \
	  ../../Scheduler/stable.cc \
	  ../../Majordomo/majordomo.cc \
          ../../../Common/Semaphore/semaphore.cc \
          ../../../Common/Killer/killer.cc \
          ../../../Common/ConfigFile/configFile.cc \
          ../../../Common/Debugable/debugable.cc \
          ../../../Common/Runable/runable.cc \
          ../../../Common/Shutdownable/shutdownable.cc \
          ../../../Common/RSAEuro/librsaeuro.a

squeue:
	g++ $(CXXFLAGS) -o testSQueue $(SQUEUE)

ts: $(tsAll)
	g++ $(CXXFLAGS) -o testTS $(tsALL) -lc_r

tsf: $(tsALLfirst)
	g++ $(CXXFLAGS) -o testTS $(tsALLfirst) -lc_r

clear:
	rm *.tx *.log *.idx
