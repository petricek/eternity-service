#
#
# Makefile for Receiver
#
# WARNING !!! - if you need update this file, MOST PROBABLY you will need
#               update just section between ************ lines.

include ./../../makefile.config
include ./../makefile.config

#**************************************************************************
# if you have another source files in current directory, update JUST 
# following lines (e.g. "SOURCES = $(BASE_NAME).cc $(BASE_NAME)Utils.cc" 
# etc.), definition of variable can continue on the next line if you 
# have character '\' at the end of line, *USE* variable BASE_NAME 
# whenever possible ! - you can then easily reuse this makefile for
# other directories just with modifiing BASE_NAME

# all object files are relinked together to one object file named 
# $(BASE_NAME)All.o for easier writing of higher makefiles
BASE_NAME = template
SOURCES = $(BASE_NAME).cc $(BASE_NAME)BlaBlaBla.cc
#**************************************************************************

# all powerfull stuff is here ;-)
include ./../../makefile.body

