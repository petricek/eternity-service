CXXFLAGS= -Wall -g

#          ../../../Common/Utils/utilsAll.o \


schALL=testScheduler.cc \
          ../../../Common/Semaphore/semaphore.o \
          ../../../Common/GMessage/estime.o \
          ../../../Common/MessageQueue/messageQueue.o \
          ../../../Common/GMessage/msgField.o \
          ../../../Common/GMessage/myString.o \
          ../../../Common/GMessage/queue.o \
          ../../../Common/GMessage/record.o \
          ../../../Common/GMessage/fieldQueue.o \
          ../../../Common/GMessage/fields.o \
	  ../../../Common/GMessage/timeField.o \
	  ../../../Common/GMessage/bytesField.o \
	  ../../../Common/GMessage/intField.o \
	  ../../../Common/GMessage/idField.o \
	  ../../../Common/GMessage/stringField.o \
          ../../../Common/GMessage/file.o \
          ../../../Common/GMessage/gMessage.o \
          ../../../Common/GMessage/table.o \
          ../../../Common/LogFile/logFile.o \
          ../../../Common/Killer/killer.o \
          ../../../Common/ConfigFile/configFile.o \
          ../../../Common/Debugable/debugable.o \
          ../../../Common/Runable/runable.o \
          ../../../Common/Shutdownable/shutdownable.o \
	  ../../TimeSynchronizer/timesynch.o \
	  ../../TimeSynchronizer/squeue.o \
	  ../../UidGenerator/uidGenerator.o \
	  ../../../Common/ACManager/acManager.o \
	  ../stable.o \
	  ../scheduler.o \
          ../../../Common/RSAEuro/librsaeuro.a

schALLfirst=testScheduler.cc \
          ../../../Common/Semaphore/semaphore.cc \
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
          ../../../Common/Killer/killer.cc \
          ../../../Common/ConfigFile/configFile.cc \
          ../../../Common/Debugable/debugable.cc \
          ../../../Common/Runable/runable.cc \
          ../../../Common/Shutdownable/shutdownable.cc \
	  ../../TimeSynchronizer/timesynch.cc \
	  ../../TimeSynchronizer/squeue.cc \
	  ../../UidGenerator/uidGenerator.cc \
	  ../../../Common/ACManager/acManager.cc \
	  ../stable.cc \
	  ../scheduler.cc \
          ../../../Common/RSAEuro/librsaeuro.a

STABLE=testSTable.cc \
	../../../Common/Debugable/debugable.o \
	../../../Common/LogFile/logFile.o \
	../../../Common/GMessage/estime.o \
	../../../Common/GMessage/file.o \
	../../../Common/GMessage/fields.o \
	../../../Common/GMessage/fieldQueue.o \
	../../../Common/GMessage/queue.o \
	../../../Common/GMessage/myString.o \
	../../../Common/GMessage/record.o \
	../../../Common/GMessage/table.o \
	../../../Common/GMessage/timeField.cc \
	../../../Common/GMessage/stringField.cc \
	../stable.o \
	../../../Common/RSAEuro/librsaeuro.a 

sch: $(schALL)
	g++ $(CXXFLAGS) -o testSched $(schALL) -lc_r

schf: $(schALLfirst)
	g++ $(CXXFLAGS) -o testSched $(schALLfirst) -lc_r

stable:
	g++ $(CXXFLAGS) -o testSTable $(STABLE)

clear:
	rm *.tx *.idx *.log

