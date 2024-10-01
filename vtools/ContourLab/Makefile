# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: ContourLab 1.00
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

all: src/Contour.TypeDef.gen.h src/Contour.Pretext.gen.h

src/Contour.TypeDef.gen.h : src/Contour.ddl
	$(TypeSet) src/Contour.ddl src/Contour.TypeDef.gen.h src/Contour.TypeSet.gen.h

src/Contour.Pretext.gen.h : src/Contour.ddl
	$(MakeString) -s src/Contour.ddl src/Contour.Pretext.gen.h

#----------------------------------------------------------------------------------------
