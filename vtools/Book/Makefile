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

LIGHT_FORGE ?= ~/LightForge

TypeSet = $(LIGHT_FORGE)/build/ccore/CCore/DDLTypeSet/CCore-DDLTypeSet.exe

MakeString = $(LIGHT_FORGE)/build/ccore/CCore/MakeString/CCore-MakeString.exe

src/AppState.TypeDef.gen.h : src/AppState.ddl
	$(TypeSet) src/AppState.ddl src/AppState.TypeDef.gen.h src/AppState.TypeSet.gen.h

src/AppState.Pretext.gen.h : src/AppState.ddl
	$(MakeString) -s src/AppState.ddl src/AppState.Pretext.gen.h

#----------------------------------------------------------------------------------------
