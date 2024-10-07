# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: PTPSecureServer 1.01
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2024 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

TypeSet = $(LIGHT_FORGE_BUILD)/CCore/DDLTypeSet/CCore-DDLTypeSet.exe

SRC = ../src

INC = ../inc

.PHONY: all

all: $(INC)/Config.type.h

$(INC)/Config.type.h : PTPSecureConfig.types.ddl
	$(TypeSet) PTPSecureConfig.types.ddl $(INC)/Config.type.h $(SRC)/Config.typeset.h

#----------------------------------------------------------------------------------------
