/* Utf8.cpp */
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

#include <CCore/inc/Utf8.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* GuardUtf8Broken() */

void GuardUtf8Broken(const char *name)
 {
  Printf(Exception,"#; : broken UTF8 sequence",name);
 }

/* functions */

Utf8Code ToUtf8(Unicode unicode)
 {
  if( unicode<0x80u )
    {
     uint8 b1=uint8(unicode);

     return Utf8Code(b1);
    }

  if( unicode<0x800u )
    {
     uint8 b1=0b1100'0000u|((unicode>>6)&0b01'1111u); // 5 bit
     uint8 b2=0b1000'0000u|((unicode   )&0b11'1111u); // 6 bit

     return Utf8Code(b1,b2);
    }

  if( unicode<0x10000u )
    {
     uint8 b1=0b1110'0000u|((unicode>>12)&0b00'1111u); // 4 bit
     uint8 b2=0b1000'0000u|((unicode>> 6)&0b11'1111u); // 6 bit
     uint8 b3=0b1000'0000u|((unicode    )&0b11'1111u); // 6 bit

     return Utf8Code(b1,b2,b3);
    }

  uint8 b1=0b1111'0000u|((unicode>>18)&0b00'0111u); // 3 bit
  uint8 b2=0b1000'0000u|((unicode>>12)&0b11'1111u); // 6 bit
  uint8 b3=0b1000'0000u|((unicode>> 6)&0b11'1111u); // 6 bit
  uint8 b4=0b1000'0000u|((unicode    )&0b11'1111u); // 6 bit

  return Utf8Code(b1,b2,b3,b4);
 }

/* parse functions */

Utf8Code PeekUtf8(StrLen text)
 {
  struct Ret : Utf8Code
   {
    Ret(char) : Utf8Code() {}

    Ret(char b1,Unicode) : Utf8Code(b1) {}

    Ret(char b1,char b2,Unicode) : Utf8Code(b1,b2) {}

    Ret(char b1,char b2,char b3,Unicode) : Utf8Code(b1,b2,b3) {}

    Ret(char b1,char b2,char b3,char b4,Unicode) : Utf8Code(b1,b2,b3,b4) {}
   };

  return PeekUtf8_gen<Ret>(text);
 }

Utf8Code CutUtf8(StrLen &text)
 {
  auto ret=PeekUtf8(text);

  text+=ret.getLen();

  return ret;
 }

Utf8Code PeekUtf8_guarded(StrLen text)
 {
  struct Ret : Utf8Code
   {
    Ret(char) : Utf8Code() { GuardUtf8Broken("CCore::PeekUtf8_guarded(...)"); }

    Ret(char b1,Unicode) : Utf8Code(b1) {}

    Ret(char b1,char b2,Unicode) : Utf8Code(b1,b2) {}

    Ret(char b1,char b2,char b3,Unicode) : Utf8Code(b1,b2,b3) {}

    Ret(char b1,char b2,char b3,char b4,Unicode) : Utf8Code(b1,b2,b3,b4) {}
   };

  return PeekUtf8_gen<Ret>(text);
 }

Utf8Code CutUtf8_guarded(StrLen &text)
 {
  auto ret=PeekUtf8_guarded(text);

  text+=ret.getLen();

  return ret;
 }

Unicode PeekUtf8_unicode(StrLen text)
 {
  struct Ret
   {
    Unicode ch;

    Ret(char) : ch(-1) {}

    Ret(char,Unicode ch_) : ch(ch_) {}

    Ret(char,char,Unicode ch_) : ch(ch_) {}

    Ret(char,char,char,Unicode ch_) : ch(ch_) {}

    Ret(char,char,char,char,Unicode ch_) : ch(ch_) {}

    operator Unicode() const { return ch; }
   };

  return PeekUtf8_gen<Ret>(text);
 }

Unicode CutUtf8_unicode(StrLen &text)
 {
  struct Ret
   {
    Unicode ch;
    unsigned len;

    Ret(char) : ch(-1),len(1) {}

    Ret(char,Unicode ch_) : ch(ch_),len(1) {}

    Ret(char,char,Unicode ch_) : ch(ch_),len(2) {}

    Ret(char,char,char,Unicode ch_) : ch(ch_),len(3) {}

    Ret(char,char,char,char,Unicode ch_) : ch(ch_),len(4) {}
   };

  Ret ret=PeekUtf8_gen<Ret>(text);

  text+=ret.len;

  return ret.ch;
 }

Unicode PeekUtf8_unicode_guarded(StrLen text)
 {
  struct Ret
   {
    Unicode ch;

    Ret(char) : ch(-1) { GuardUtf8Broken("CCore::PeekUtf8_unicode_guarded(...)"); }

    Ret(char,Unicode ch_) : ch(ch_) {}

    Ret(char,char,Unicode ch_) : ch(ch_) {}

    Ret(char,char,char,Unicode ch_) : ch(ch_) {}

    Ret(char,char,char,char,Unicode ch_) : ch(ch_) {}

    operator Unicode() const { return ch; }
   };

  return PeekUtf8_gen<Ret>(text);
 }

Unicode CutUtf8_unicode_guarded(StrLen &text)
 {
  struct Ret
   {
    Unicode ch;
    unsigned len;

    Ret(char) : ch(-1),len(1) { GuardUtf8Broken("CCore::CutUtf8_unicode_guarded(...)"); }

    Ret(char,Unicode ch_) : ch(ch_),len(1) {}

    Ret(char,char,Unicode ch_) : ch(ch_),len(2) {}

    Ret(char,char,char,Unicode ch_) : ch(ch_),len(3) {}

    Ret(char,char,char,char,Unicode ch_) : ch(ch_),len(4) {}
   };

  Ret ret=PeekUtf8_gen<Ret>(text);

  text+=ret.len;

  return ret.ch;
 }

/* functions */

void TrimUtf8End(StrLen &text)
 {
  auto cur=RangeReverse(text);
  unsigned count=0;

  for(; +cur && count<2 && Utf8Ext(*cur) ;++cur,count++);

  unsigned len;

  if( +cur && (len=Utf8Len(*cur))!=0 && count<len-1 )
    {
     // trim

     text.len-=count+1;
    }
 }

ulen PopUtf8(const char *ptr,ulen len)
 {
  while( len && Utf8Ext(ptr[len-1]) ) len--;

  if( len ) len--;

  return len;
 }

ulen Utf8Len(StrLen text)
 {
  ulen ret=0;

  while( +text )
    {
     struct Ret
      {
       unsigned len;

       Ret(char) : len(1) {}

       Ret(char,Unicode) : len(1) {}

       Ret(char,char,Unicode) : len(2) {}

       Ret(char,char,char,Unicode) : len(3) {}

       Ret(char,char,char,char,Unicode) : len(4) {}

       operator unsigned() const { return len; }
      };

     unsigned len=PeekUtf8_gen<Ret>(text);

     text+=len;

     ret++;
    }

  return ret;
 }

ulen Utf8Len_guarded(StrLen text)
 {
  ulen ret=0;

  while( +text )
    {
     struct Ret
      {
       unsigned len;

       Ret(char) : len(1) { GuardUtf8Broken("CCore::Utf8Len_guarded(...)"); }

       Ret(char,Unicode) : len(1) {}

       Ret(char,char,Unicode) : len(2) {}

       Ret(char,char,char,Unicode) : len(3) {}

       Ret(char,char,char,char,Unicode) : len(4) {}

       operator unsigned() const { return len; }
      };

     unsigned len=PeekUtf8_gen<Ret>(text);

     text+=len;

     ret++;
    }

  return ret;
 }

StrLen Utf8Move(StrLen text,ulen count)
 {
  while( +text && count )
    {
     struct Ret
      {
       unsigned len;

       Ret(char) : len(1) {}

       Ret(char,Unicode) : len(1) {}

       Ret(char,char,Unicode) : len(2) {}

       Ret(char,char,char,Unicode) : len(3) {}

       Ret(char,char,char,char,Unicode) : len(4) {}

       operator unsigned() const { return len; }
      };

     unsigned len=PeekUtf8_gen<Ret>(text);

     text+=len;
     count--;
    }

  return text;
 }

StrLen Utf8Move_guarded(StrLen text,ulen count)
 {
  while( +text && count )
    {
     struct Ret
      {
       unsigned len;

       Ret(char) : len(1) { GuardUtf8Broken("CCore::Utf8Move_guarded(...)"); }

       Ret(char,Unicode) : len(1) {}

       Ret(char,char,Unicode) : len(2) {}

       Ret(char,char,char,Unicode) : len(3) {}

       Ret(char,char,char,char,Unicode) : len(4) {}

       operator unsigned() const { return len; }
      };

     unsigned len=PeekUtf8_gen<Ret>(text);

     text+=len;
     count--;
    }

  return text;
 }

/* struct DetectUtf8 */

DetectUtf8::DetectUtf8(StrLen text)
 {
  if( +text )
    {
     char b1=*text;

     switch( Utf8Len(b1) )
       {
        case 0 : default:
         {
          delta=1;
          status=Broken;
         }
        break;

        case 1 :
         {
          delta=1;
          status=Done;
         }
        break;

        case 2 :
         {
          if( text.len<2 )
            {
             delta=0;
             status=None;
            }
          else
            {
             char b2=text[1];

             if( !Utf8Ext(b2) )
               {
                delta=1;
                status=Broken;
               }
             else
               {
                Unicode ch=Utf8Code::ToUnicode(b1,b2);

                if( ch<0x80u )
                  {
                   delta=2;
                   status=Broken;
                  }
                else
                  {
                   delta=2;
                   status=Done;
                  }
               }
            }
         }
        break;

        case 3 :
         {
          if( text.len<3 )
            {
             delta=0;
             status=None;
            }
          else
            {
             char b2=text[1];
             char b3=text[2];

             if( !Utf8Ext(b2) )
               {
                delta=1;
                status=Broken;
               }
             else if( !Utf8Ext(b3) )
               {
                delta=2;
                status=Broken;
               }
             else
               {
                Unicode ch=Utf8Code::ToUnicode(b1,b2,b3);

                if( ch<0x800u )
                  {
                   delta=3;
                   status=Broken;
                  }
                else
                  {
                   delta=3;
                   status=Done;
                  }
               }
            }
         }
        break;

        case 4 :
         {
          if( text.len<4 )
            {
             delta=0;
             status=None;
            }
          else
            {
             char b2=text[1];
             char b3=text[2];
             char b4=text[3];

             if( !Utf8Ext(b2) )
               {
                delta=1;
                status=Broken;
               }
             else if( !Utf8Ext(b3) )
               {
                delta=2;
                status=Broken;
               }
             else if( !Utf8Ext(b4) )
               {
                delta=3;
                status=Broken;
               }
             else
               {
                Unicode ch=Utf8Code::ToUnicode(b1,b2,b3,b4);

                if( ch<0x10000u )
                  {
                   delta=4;
                   status=Broken;
                  }
                else
                  {
                   delta=4;
                   status=Done;
                  }
               }
            }
         }
        break;
       }
    }
  else
    {
     delta=0;
     status=None;
    }
 }

} // namespace CCore

