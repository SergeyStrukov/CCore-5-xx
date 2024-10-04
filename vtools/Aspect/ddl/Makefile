# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: Aspect 1.01
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

all: $(SRC)/AppState.TypeDef.gen.h $(SRC)/AppState.Pretext.gen.h $(SRC)/Aspect.TypeDef.gen.h $(SRC)/Aspect.Pretext.gen.h

$(SRC)/AppState.TypeDef.gen.h : AppState.ddl
	$(TypeSet) AppState.ddl $(SRC)/AppState.TypeDef.gen.h $(SRC)/AppState.TypeSet.gen.h

$(SRC)/AppState.Pretext.gen.h : AppState.ddl
	$(MakeString) -s AppState.ddl $(SRC)/AppState.Pretext.gen.h


$(SRC)/Aspect.TypeDef.gen.h : Aspect.ddl
	$(TypeSet) Aspect.ddl $(SRC)/Aspect.TypeDef.gen.h $(SRC)/Aspect.TypeSet.gen.h

$(SRC)/Aspect.Pretext.gen.h : Aspect.ddl
	$(MakeString) -s Aspect.ddl $(SRC)/Aspect.Pretext.gen.h

#----------------------------------------------------------------------------------------
