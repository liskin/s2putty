#*    puttyicons.mk
#*
#* Extension makefile to create PuTTY icons
#*
#* Copyright 2008 Petteri Kangaslampi
#*
#* See license.txt for full copyright and license information.

# Determine target and source file name
ifeq (WINS,$(findstring WINS, $(PLATFORM)))
TARGET=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z\resource\apps\putty_aif.mif
else
TARGET=$(EPOCROOT)epoc32\data\z\resource\apps\putty_aif.mif
endif

SOURCE = ..\..\ui\s60v3\putty.svg

MAKMAKE :

BLD :

CLEAN :

LIB :

CLEANLIB :

RESOURCE : $(TARGET)

$(TARGET) : $(SOURCE)
	mifconv $(TARGET) /c32 $(SOURCE)

FREEZE :

SAVESPACE :

RELEASABLES :
	@echo $(TARGET)

FINAL :
