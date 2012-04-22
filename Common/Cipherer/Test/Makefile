CXXFLAGS= -g -Wall

ALL=all

.PHONY: $(ALL) clean

COMMON=../../../Common/Debugable/debugable.o \
       ../../../Common/LogFile/logFile.o \
       ../../../Common/Utils/mem.o \
       ../../../Common/GMessage/gMessage.o \
       ../../../Common/GMessage/file.o \
       ../../../Common/GMessage/fields.o \
       ../../../Common/GMessage/msgField.o \
       ../../../Common/GMessage/record.o \
       ../../../Common/GMessage/table.o \
       ../../../Common/GMessage/queue.o \
       ../../../Common/GMessage/myString.o \
       ../../../Common/GMessage/fieldQueue.o \
       ../../../Common/GMessage/estime.o \
       ../../../Common/RSAEuro/librsaeuro.a

AUXILIARY=auxiliary.o

CIPHERER_ALL=./../ciphererAll.o

TEST_SYMETRIC=testSymetric.o
TEST_ASYMETRIC=testAsymetric.o
TEST_SEALING=testSealing.o
TEST_DIGEST_AND_SIGNATURE=testDigestAndSignature.o

SYM=sym
ASYM=asym
SEAL=seal
DIG_SIG=digSig

$(ALL): $(SYM) $(ASYM) $(SEAL) $(DIG_SIG)

$(SYM): $(TEST_SYMETRIC) $(AUXILIARY)
	g++ -g -Wall -o $(SYM) \
	  $(TEST_SYMETRIC) \
	  $(AUXILIARY) \
	  $(CIPHERER_ALL) \
	  $(COMMON)

$(ASYM): $(TEST_ASYMETRIC) $(AUXILIARY)
	g++ -g -Wall -o $(ASYM) \
	  $(TEST_ASYMETRIC) \
	  $(AUXILIARY) \
	  $(CIPHERER_ALL) \
	  $(COMMON)

$(SEAL): $(TEST_SEALING) $(AUXILIARY)
	g++ -g -Wall -o $(SEAL) \
	  $(TEST_SEALING) \
	  $(AUXILIARY) \
	  $(CIPHERER_ALL) \
	  $(COMMON)

$(DIG_SIG): $(TEST_DIGEST_AND_SIGNATURE) $(AUXILIARY)
	g++ -g -Wall -o $(DIG_SIG) \
	  $(TEST_DIGEST_AND_SIGNATURE) \
	  $(AUXILIARY) \
	  $(CIPHERER_ALL) \
	  $(COMMON)

clean:
	rm $(TEST_SEALING)
	rm $(TEST_ASYMETRIC)
	rm $(TEST_SYMETRIC)
	rm $(TEST_DIGEST_AND_SIGNATURE)
	rm $(AUXILIARY)
	rm $(SEAL)
	rm $(ASYM)
	rm $(SYM)
	rm $(DIG_SIG)
