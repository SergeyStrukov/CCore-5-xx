/* CharProp.h */
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

#ifndef CCore_inc_CharProp_h
#define CCore_inc_CharProp_h

#include <CCore/inc/Printf.h>

namespace CCore {

/* concept CharCodeType<Char> */

template <class Char>
concept CharCodeType = Meta::OneOf<Char,char,signed char,unsigned char> ;

/* CutLine() */

StrLen CutLine(StrLen &text);

/* classes */

struct SplitLine;

class ASCIICode;

class ExtCharCode;

class PrintCString;

/* struct SplitLine */

struct SplitLine
 {
  StrLen line;
  StrLen rest;
  bool eol;

  explicit SplitLine(StrLen text);
 };

/* class ASCIICode */

class ASCIICode
 {
  public:

   using CodeType = uint8 ;

   static char InverseMap(CodeType code) { return char(code); }

   //
   // (CodeType)InverseMap(code) == code
   //

  private:

   CodeType code;

    //
    // ASCII code table
    //
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |     I  00 |  01 |  02 |  03 |  04 |  05 |  06 |  07 |  08 |  09 |  0A |  0B |  0C |  0D |  0E |  0F |
    // +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    // |  00 I     |     |     |     |     |     |     |     | \b  | \t  | \n  | \v  | \f  | \r  |     |     |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  10 I     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |     |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  20 I     |  !  |  "  |  #  |  $  |  %  |  &  |  '  |  (  |  )  |  *  |  +  |  ,  |  -  |  .  |  /  |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  30 I  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  :  |  ;  |  <  |  =  |  >  |  ?  |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  40 I  @  |  A  |  B  |  C  |  D  |  E  |  F  |  G  |  H  |  I  |  J  |  K  |  L  |  M  |  N  |  O  |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  50 I  P  |  Q  |  R  |  S  |  T  |  U  |  V  |  W  |  X  |  Y  |  Z  |  [  |  \  |  ]  |  ^  |  _  |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  60 I  `  |  a  |  b  |  c  |  d  |  e  |  f  |  g  |  h  |  i  |  j  |  k  |  l  |  m  |  n  |  o  |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    // |  70 I  p  |  q  |  r  |  s  |  t  |  u  |  v  |  w  |  x  |  y  |  z  |  {  |  |  |  }  |  ~  |     |
    // +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
    //

   static const bool IsSpaceTable[256];

   static const bool IsPunctTable[256];

   static const bool IsSpaceOrPunctTable[256];

   static const signed char DecValueTable[256];

   static const signed char HexValueTable[256];

  public:

   // constructors

   ASCIICode() : code(0) {}

   explicit ASCIICode(CharCodeType auto ch) : code(ch) {}

   // properties

   bool isSpecial() const { return code<32 || code==127 ; }

        // special -- represents no character

   bool isVisible() const { return code>32 && code!=127 ; }

        // visible -- represents non-space character

   bool isPrintable() const { return code>=32 && code!=127 ; }

        // printable -- not a special char

   bool isSpace() const { return IsSpaceTable[code]; }

        // space-like -- " \t\n\v\f\r"

   bool isPunct() const { return IsPunctTable[code]; }

        // punctuation characters

   bool isSpaceOrPunct() const { return IsSpaceOrPunctTable[code]; }

        // space or punct

   int decValue() const { return DecValueTable[code]; }

       // decimal digit value or -1

   int hexValue() const { return HexValueTable[code]; }

       // hexadecimal digit value or -1

   char getChar() const { return InverseMap(code); }

   // print object

   void print(PrinterType auto &out) const
    {
     if( code>=32 && code!=127 )
       {
        switch( char ch=getChar() )
          {
           case '\\' : case '"' : out.put('\\'); [[fallthrough]];

           default: out.put(ch);
          }
       }
     else if( code>=8 && code<=13 )
       {
        out.put('\\');
        out.put("btnvfr"[code-8]);
       }
     else
       {
        Printf(out,"\\x#2.16i;",code);
       }
    }
 };

/* type CharCode */

using CharCode = ASCIICode ;

/* class PrintCString */

class PrintCString
 {
   StrLen str;

  public:

   explicit PrintCString(StrLen str_) : str(str_) {}

   using PrintOptType = StrPrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     if( opt.quoted ) out.put('"');

     for(auto r=str; +r ;++r) Putobj(out,CharCode(*r));

     if( opt.quoted ) out.put('"');
    }
 };

/* property functions */

bool charIsSpecial(CharCodeType auto ch) { return CharCode(ch).isSpecial(); }

bool charIsVisible(CharCodeType auto ch) { return CharCode(ch).isVisible(); }

bool charIsPrintable(CharCodeType auto ch) { return CharCode(ch).isPrintable(); }

bool charIsSpace(CharCodeType auto ch) { return CharCode(ch).isSpace(); }

bool charIsPunct(CharCodeType auto ch) { return CharCode(ch).isPunct(); }

bool charIsSpaceOrPunct(CharCodeType auto ch) { return CharCode(ch).isSpaceOrPunct(); }


int charDecValue(CharCodeType auto ch) { return CharCode(ch).decValue(); }

int charHexValue(CharCodeType auto ch) { return CharCode(ch).hexValue(); }


bool charIsEOL(CharCodeType auto ch) { return ch=='\r' || ch=='\n' ; }

bool charIsBin(CharCodeType auto ch) { return ch=='0' || ch=='1' ; }

bool charIsDec(CharCodeType auto ch) { return ch>='0' && ch<='9' ; }

bool charIsHex(CharCodeType auto ch) { return charHexValue(ch)>=0; }


int charBinValue(CharCodeType auto ch)
 {
  switch( ch )
    {
     case '0' : return 0;
     case '1' : return 1;
     default:   return -1;
    }
 }

/* ParseSpace() */

template <CharPeekType Dev>
void ParseSpace(Dev &dev)
 {
  for(;;++dev)
    {
     typename Dev::Peek peek(dev);

     if( !peek || !charIsSpace(*peek) ) break;
    }
 }

/* char sets */

const char * GetSpaceChars();

const char * GetPunctChars();

const char * GetDigitChars();

const char * GetHexDigitChars();

const char * GetCLetterChars();

} // namespace CCore

#endif


