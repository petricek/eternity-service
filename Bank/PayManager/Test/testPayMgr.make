g++ -Wall -g \
./../../../Common/Payments/payRecordAll.o \
./../../../Common/LogFile/logFile.o \
./../../../Common/Debugable/debugable.o \
./../../../Common/GMessage/gMessageAll.o \
./../../../Common/Utils/createTables.o \
./../payManagerAll.o \
./testAux.cc \
./testPayMgr.cc \
-o testPayMgr