/* SysUtf8.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/sys/SysUtf8.h>

#include <CCore/inc/MemBase.h>

#include <CCore/inc/win64/Win64.h>

namespace CCore {
namespace Sys {

/* functions */

const WChar * ZScan(const WChar *ztext) noexcept
 {
  for(; *ztext ;ztext++);

  return ztext;
 }

ulen ZLen(const WChar *ztext) noexcept
 {
  return Dist(ztext,ZScan(ztext));
 }

/* functions */

ulen Truncate(PtrLen<const WChar> text,PtrLen<char> out) noexcept
 {
  ulen start=out.len;

  FeedUnicode(text,CopySym(out));

  return start-out.len;
 }

ulen Full(PtrLen<const WChar> text,PtrLen<char> out) noexcept
 {
  ulen start=out.len;

  bool ok=FeedUnicode(text,CopySym(out));

  if( ok ) return start-out.len;

  return MaxULen;
 }

ulen Full(const WChar *ztext,PtrLen<char> out) noexcept
 {
  return Full(ZRange(ztext),out);
 }

/* struct StartTransform */

StartTransform::StartTransform(PtrLen<const WChar> text,PtrLen<char> out) noexcept
 {
  ulen start=out.len;

  CopySym copy(out);

  while( +text )
    {
     Unicode sym=CutUnicode(text);

     if( !copy(sym) )
       {
        len=start-out.len;
        next_sym=sym;
        rest=text;
        ok=false;

        return;
       }
    }

  len=start-out.len;
  ok=true;
 }

/* struct TransformCountLen */

TransformCountLen::TransformCountLen(ulen slen,Unicode sym,PtrLen<const WChar> text) noexcept
 {
  struct CountLen
   {
    ULenSat result;

    bool operator () (Unicode sym)
     {
      Utf8Code code=ToUtf8(sym);

      result+=code.getLen();

      return result.overflow;
     }
   };

  CountLen count{slen};

  if( count(sym) )
    {
     len=MaxULen;
     ok=false;

     return;
    }

  while( +text )
    if( count(CutUnicode(text)) )
      {
       len=MaxULen;
       ok=false;

       return;
      }

  len=count.result.value;
  ok=true;
 }

/* FinishTransform() */

ulen FinishTransform(PtrLen<char> out,PtrLen<const char> str,Unicode sym,PtrLen<const WChar> text) noexcept
 {
  ulen start=out.len;

  str.copyTo(out.ptr);

  out+=str.len;

  CopySym copy(out);

  copy(sym);

  while( +text ) copy(CutUnicode(text));

  return start-out.len;
 }

/* class WCharToUtf8Full */

bool WCharToUtf8Full::countLen(ulen slen,Unicode sym,PtrLen<const WChar> text)
 {
  TransformCountLen result(slen,sym,text);

  if( result.ok )
    {
     len=result.len;

     return true;
    }
  else
    {
     len=MaxULen;
     ptr=0;

     return false;
    }
 }

void WCharToUtf8Full::longText(ulen slen,Unicode sym,PtrLen<const WChar> text)
 {
  if( !countLen(slen,sym,text) ) return;

  ptr=PlaceAt(TryMemAlloc(len));

  if( !ptr ) return;

  len=FinishTransform(Range(ptr,len),Range(small,slen),sym,text);
 }

WCharToUtf8Full::WCharToUtf8Full(PtrLen<const WChar> text) noexcept
 {
  StartTransform result(text,Range(small));

  if( result.ok )
    {
     ptr=small;
     len=result.len;
    }
  else
    {
     longText(result.len,result.next_sym,result.rest);
    }
 }

WCharToUtf8Full::~WCharToUtf8Full()
 {
  if( ptr && ptr!=small ) MemFree(ptr);
 }

/* struct ToWChar */

PtrLen<WChar> ToWChar::feed(PtrLen<WChar> out,StrLen text)
 {
  while( +text )
    {
     Unicode ch=CutUtf8_unicode(text);

     if( ch==Unicode(-1) )
       {
        broken=true;

        break;
       }
     else
       {
        if( IsSurrogate(ch) )
          {
           if( out.len<2 )
             {
              overflow=true;

              break;
             }

           SurrogateCouple couple(ch);

           out[0]=couple.hi;
           out[1]=couple.lo;

           out+=2;
          }
        else
          {
           if( !out.len )
             {
              overflow=true;

              break;
             }

           *out=WChar(ch);

           ++out;
          }
       }
    }

  return out;
 }

void ToWChar::finish(PtrLen<WChar> out,bool zero)
 {
  if( zero )
    {
     if( !out )
       {
        overflow=true;
       }
     else
       {
        *out=0;
       }
    }
 }

ToWChar::ToWChar(PtrLen<WChar> out,StrLen text,bool zero) noexcept
 {
  ulen start=out.len;

  out=feed(out,text);

  finish(out,zero);

  len=start-out.len;
 }

ToWChar::ToWChar(PtrLen<WChar> out,StrLen text1,StrLen text2,bool zero) noexcept
 {
  ulen start=out.len;

  out=feed(out,text1);

  if( !overflow && !broken ) out=feed(out,text2);

  finish(out,zero);

  len=start-out.len;
 }

} // namespace Sys
} // namespace CCore

