#
# Makefile for Mix
#

include ./../makefile.config
include ./makefile.config

EVERYTHING= DoFirst \
	Six \
	SixMajordomo \
	SixOffspring \
	DoLast

.PHONY: all $(EVERYTHING) clean

all: $(EVERYTHING)	

clean:
	-find . -name "*.o" -exec rm {} \;
	-find . -name "*.d" -exec rm {} \;

SEP=--------------------------------------
SEP2=========================
SEP3==================================

DoFirst:
	@echo $(SEP)$(SEP)
	@echo $(SEP2) I am working on Six now... $(SEP2)
	@echo $(SEP)$(SEP)
	@echo

DoLast:
	@echo $(SEP)$(SEP)
	@echo $(SEP2)======= Six Done !!! =======$(SEP2)
	@echo $(SEP)$(SEP)
	@echo

SixOffspring:
	cd ./$(@); $(MAKE) 

SixMajordomo:
	cd ./$(@); $(MAKE) 

Six:
	cd ./$(@); $(MAKE) 

# end of Makefile for Six
