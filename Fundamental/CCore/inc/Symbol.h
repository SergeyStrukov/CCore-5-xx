/* Symbol.h */
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

#ifndef CCore_inc_Symbol_h
#define CCore_inc_Symbol_h

#include <CCore/inc/CharProp.h>
#include <CCore/inc/Utf8.h>

namespace CCore {

/* type Symbol */

using Symbol = Utf8Code ;

/* const MaxSymbolLen */

inline constexpr unsigned MaxSymbolLen = 4 ;

/* property functions */

inline bool CharIsSpecial(Symbol sym) { return (sym.getLen()==1)? charIsSpecial(sym[0]) : false ; }

inline bool CharIsVisible(Symbol sym) { return (sym.getLen()==1)? charIsVisible(sym[0]) : true ; }

inline bool CharIsPrintable(Symbol sym) { return (sym.getLen()==1)? charIsPrintable(sym[0]) : true ; }

inline bool CharIsSpace(Symbol sym) { return (sym.getLen()==1)? charIsSpace(sym[0]) : false ; }


inline int CharDecValue(Symbol sym) { return (sym.getLen()==1)? charDecValue(sym[0]) : (-1) ; }

inline int CharHexValue(Symbol sym) { return (sym.getLen()==1)? charHexValue(sym[0]) : (-1) ; }


inline bool CharIsEOL(Symbol sym) { return (sym.getLen()==1)? charIsEOL(sym[0]) : false ; }

inline bool CharIsBin(Symbol sym) { return (sym.getLen()==1)? charIsBin(sym[0]) : false ; }

inline bool CharIsDec(Symbol sym) { return (sym.getLen()==1)? charIsDec(sym[0]) : false ; }

inline bool CharIsHex(Symbol sym) { return (sym.getLen()==1)? charIsHex(sym[0]) : false ; }


inline int CharBinValue(Symbol sym) { return (sym.getLen()==1)? charBinValue(sym[0]) : (-1) ; }

/* functions */

inline StrLen TrimText(StrLen text)
 {
  TrimUtf8End(text);

  return text;
 }

inline ulen PopSymbol(const char *ptr,ulen len) { return PopUtf8(ptr,len); }

/* functions */

inline int ToBasicChar(Symbol sym) { return (sym.getLen()==1)? sym[0] : (-1) ; }

inline ulen SymbolLen(Symbol sym) { return sym.getLen(); }

inline StrLen SymbolRange(const Symbol &sym) { return sym.getRange(); }

inline Symbol PeekSymbol(StrLen text) // +text
 {
  return PeekUtf8(text);
 }

inline Symbol CutSymbol(StrLen &text) // +text
 {
  return CutUtf8(text);
 }

inline Symbol PeekSymbol_guarded(StrLen text) // +text
 {
  return PeekUtf8_guarded(text);
 }

inline Symbol CutSymbol_guarded(StrLen &text) // +text
 {
  return CutUtf8_guarded(text);
 }

using DetectSymbol = DetectUtf8 ;

//----------------------------------------------------------------------------------------

/* type Char */

using Char = Unicode ;

/* property functions */

bool CharIsSpecial(IsType<Char> auto ch) { return (ch<128)? charIsSpecial((char)ch) : false ; }

bool CharIsVisible(IsType<Char> auto ch) { return (ch<128)? charIsVisible((char)ch) : true ; }

bool CharIsPrintable(IsType<Char> auto ch) { return (ch<128)? charIsPrintable((char)ch) : true ; }

bool CharIsSpace(IsType<Char> auto ch) { return (ch<128)? charIsSpace((char)ch) : false ; }


int CharDecValue(IsType<Char> auto ch) { return (ch<128)? charDecValue((char)ch) : (-1) ; }

int CharHexValue(IsType<Char> auto ch) { return (ch<128)? charHexValue((char)ch) : (-1) ; }


bool CharIsEOL(IsType<Char> auto ch) { return ch=='\r' || ch=='\n' ; }

bool CharIsBin(IsType<Char> auto ch) { return ch=='0' || ch=='1' ; }

bool CharIsDec(IsType<Char> auto ch) { return (ch<128)? charIsDec((char)ch) : false ; }

bool CharIsHex(IsType<Char> auto ch) { return (ch<128)? charIsHex((char)ch) : false ; }


int CharBinValue(IsType<Char> auto ch)
 {
  switch( ch )
    {
     case '0' : return 0;
     case '1' : return 1;
     default:   return -1;
    }
 }

/* functions */

int ToBasicChar(IsType<Char> auto ch) { return (ch<128)? int(ch) : (-1) ; }

Symbol ToSymbol(IsType<Char> auto ch) { return ToUtf8(ch); }

inline Char FromSymbol(Symbol sym) { return sym.toUnicode(); }

inline Char PeekChar(StrLen text) // +text
 {
  return PeekUtf8_unicode(text);
 }

inline Char CutChar(StrLen &text) // +text
 {
  return CutUtf8_unicode(text);
 }

inline Char PeekChar_guarded(StrLen text) // +text
 {
  return PeekUtf8_unicode_guarded(text);
 }

inline Char CutChar_guarded(StrLen &text) // +text
 {
  return CutUtf8_unicode_guarded(text);
 }

} // namespace CCore

#endif

