#
# This is a Makefile for project Eternity Service. See README for details.
#

include ./makefile.config

.PHONY:	all DoFirst Mix Eso Acs Client Bank DoLast clean
all:	DoFirst Mix Acs Eso Client Bank DoLast

SP=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SEP2=========================

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;
	-find . -name "*.core" -exec rm {} \;
	-find . -name "*.tgz" -exec rm {} \;
	-find . -name "err" -exec rm {} \;
	-rm -f Mix/mix
	-rm -f Acs/acs
	-rm -f Client/client
	-rm -f Bank/bank
	-rm -f Eso/eso
	-rm Eso/TCBWrapper/Test/genTCBKeys
	-rm Eso/TCBWrapper/Test/prepSaveFile
	-rm Eso/TCBWrapper/Test/testComputeMAC
	-rm Eso/TCBWrapper/Test/testDecryptByACPriv
	-rm Eso/TCBWrapper/Test/testDeleteFile
	-rm Eso/TCBWrapper/Test/testFAM
	-rm Eso/TCBWrapper/Test/testGetFile
	-rm Eso/TCBWrapper/Test/testKeywords
	-rm Eso/TCBWrapper/Test/testSaveFile
	-rm Eso/TCBWrapper/Test/testTCBTable
	-rm Common/Cipherer/Test/sym
	-rm Common/Cipherer/Test/asym
	-rm Common/Cipherer/Test/seal
	-rm Common/Cipherer/Test/digSig

DoFirst:
	@echo $(SP)
	@echo $(SEP2)==== Eternity Service $(SEP2)======
	@echo $(SEP2)= Student Project, MFF UK $(SEP2)==
	@echo $(SP)
	@echo
	@sleep 1

Mix:
	cd ./Mix; $(MAKE)

Acs:
	cd ./Acs; $(MAKE)

Eso:
	cd ./Eso; $(MAKE)
	:
Client:
	cd ./Client; $(MAKE)

Bank:
	cd ./Bank; $(MAKE)

DoLast:
	@echo $(SP)
	@echo $(SEP2)==== Everything DONE !! $(SEP2)====
	@echo $(SP)
