#############################################################
# AVR Makefile
# GCC Application Builder
# Ilialex Research Lab
#############################################################

include cfg/env.in
# defines for system configuration
include cfg/hw.in
include cfg/swdef.in

# Directory Structure Definition
# Object Destination
# Error Log Files
# Listing Files
# Configuration Files location
# Source tree

DESTDIR			= build.obj
PRGDIR			= build.rom
ERRDIR			= build.err
LSTDIR			= build.lst
CFGDIR			= cfg
SRCDIR			= src
DEFHW			= src/includes/hwdef.h
DEPDIR 			= build.dep

df = $(DEPDIR)/$(*F)

include cfg/srcobj.in
########### change this lines according to your project ##################

#put the name of the target file here (without extension)
	TRG	= kernel

#compiler flags
#	CPFLAGS	= -g -O2 -Wall -Wstrict-prototypes -Wa,-ahlms=$(LSTDIR)/$(@:.o=.lst)
	CPFLAGS	= -g -O2 -Wall -Wstrict-prototypes -Wa,-ahlms=$(LSTDIR)/$(subst build.obj/,,$(@:.o=.lst)) 
	
#linker flags
	LDFLAGS = -Wl,-Map=$(LSTDIR)/$(TRG).map,--cref

########### you should not need to change the following lines #############
###########################################################################

###########################################################################
include cfg/compileflags.in
include cfg/compile.in

###########################################################################		  
###### dependecies, add any dependencies you need here ###################
-include $(SRC:%.c=$(DEPDIR)/%.P)

