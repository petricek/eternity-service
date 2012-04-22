g++ -Wall -g \
./../../../Common/Debugable/debugableAll.o \
./../../../Common/LogFile/logFileAll.o \
./../../../Common/GMessage/gMessageAll.o \
./../../../Common/Payments/payRecordAll.o \
./../../../Common/RandomGenerator/randomGeneratorAll.o \
./../../../Common/Utils/createTables.o \
./../../UidGenerator/uidGeneratorAll.o \
./../bankerAll.o \
./testAux.cc \
./testBank.cc \
-o testBank