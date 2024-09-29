/* TextTools.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/TextTools.h>

namespace CCore {

/* struct TextPos */

void TextPos::update(StrLen text)
 {
  for(;;)
    {
     SplitLine split(text);

     if( split.eol )
       {
        nextLine();

        text=split.rest;
       }
     else
       {
        update(SymLen(split.line));

        break;
       }
    }
 }

/* class TextPosCounter */

void TextPosCounter::newLine()
 {
  pos.nextLine();
  det.reset();

  flag=false;
 }

void TextPosCounter::put(char ch)
 {
  if( ch=='\r' )
    {
     newLine();

     flag=true;
    }
  else if( ch=='\n' )
    {
     if( !flag ) newLine();

     flag=false;
    }
  else
    {
     if( det.put(ch) ) pos.nextPos();

     flag=false;
    }
 }

} // namespace CCore


