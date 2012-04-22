#
# Makefile for Mix
#

include ./makefile.config

.PHONY:	all clean Cipherer KeyManager Mix Padder Receiver Sender \
        Translator Common ../Common mix DoFirst

all:	DoFirst main

clean:
	-find . -name "*.o" -exec rm {} \;

SEP=----------------------------------------------------------------------------
SP=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SEP2=========================
SEP3==================================

DoFirst:
	@echo $(SEP)
	@echo $(SEP2) I am working on Mix now... $(SEP2)
	@echo $(SEP)
	@echo
	@sleep 1



main	: ./main.cc ./../../Common/commonAll.o

	@echo $(SEP)
	@echo $(SEP2)  I am creating executable NOW !!!  $(SEP2)=
	@echo $(SEP)
	$(CC) $(CFLAGS) -o $(@) $(^) -l$(THREADS)
	@echo $(SEP)
	@echo $(SEP2)======= Mix done !!! $(SEP2)=======
	@echo $(SEP)

