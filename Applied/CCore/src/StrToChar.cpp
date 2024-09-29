/* StrToChar.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/StrToChar.h>

namespace CCore {

/* class StrToChar */

StrToChar::StrToChar(StrLen text)
 : buf(DoReserve,text.len)
 {
  while( +text )
    {
     Char ch=CutChar_guarded(text);

     buf.append_copy(ch);
    }
 }

/* class CharToStr */

CharToStr::CharToStr(PtrLen<const Char> text)
 : buf(DoReserve,2*text.len)
 {
  for(Char ch : text )
    {
     Symbol sym=ToSymbol(ch);

     buf.extend_copy(SymbolRange(sym));
    }
 }

} // namespace CCore

