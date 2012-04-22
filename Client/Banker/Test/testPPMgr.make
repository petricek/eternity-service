g++ -Wall -g \
./../../../Common/Payments/payRecordAll.o \
./../../../Common/LogFile/logFile.o \
./../../../Common/Debugable/debugable.o \
./../../../Common/GMessage/gMessageAll.o \
./../../../Common/RandomGenerator/randomGenerator.o \
./../../../Common/Utils/createTables.o \
./../pPlanMgrAll.o \
./testAux.cc \
./testPPMgr.cc \
-o testPPMgr