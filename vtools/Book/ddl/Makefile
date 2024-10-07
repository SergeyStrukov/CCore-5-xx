# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: Book 1.00
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2024 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

TypeSet = $(LIGHT_FORGE_BUILD)/CCore/DDLTypeSet/CCore-DDLTypeSet.exe

MakeString = $(LIGHT_FORGE_BUILD)/CCore/MakeString/CCore-MakeString.exe

SRC = ../src

.PHONY: all

all: $(SRC)/AppState.TypeDef.gen.h $(SRC)/AppState.Pretext.gen.h

$(SRC)/AppState.TypeDef.gen.h : AppState.ddl
	$(TypeSet) AppState.ddl $(SRC)/AppState.TypeDef.gen.h $(SRC)/AppState.TypeSet.gen.h

$(SRC)/AppState.Pretext.gen.h : AppState.ddl
	$(MakeString) -s AppState.ddl $(SRC)/AppState.Pretext.gen.h

#----------------------------------------------------------------------------------------
