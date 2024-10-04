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

TypeSet = $(LIGHT_FORGE_BUILD)/CCore/DDLTypeSet/CCore-DDLTypeSet.exe

MakeString = $(LIGHT_FORGE_BUILD)/CCore/MakeString/CCore-MakeString.exe

SRC = ../src

all: $(SRC)/Contour.TypeDef.gen.h $(SRC)/Contour.Pretext.gen.h

$(SRC)/Contour.TypeDef.gen.h : Contour.ddl
	$(TypeSet) Contour.ddl $(SRC)/Contour.TypeDef.gen.h $(SRC)/Contour.TypeSet.gen.h

$(SRC)/Contour.Pretext.gen.h : Contour.ddl
	$(MakeString) -s Contour.ddl $(SRC)/Contour.Pretext.gen.h

#----------------------------------------------------------------------------------------
