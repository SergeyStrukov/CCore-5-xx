/* ScanTools.h */
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

#ifndef CCore_inc_scanf_ScanTools_h
#define CCore_inc_scanf_ScanTools_h

#include <CCore/inc/TextTools.h>

namespace CCore {

/* concept CharStreamType<S> */

template <class S> // ref extended
concept CharStreamType = requires(S &obj,Meta::ToConst<S> &cobj)
 {
  Ground<bool>( +cobj );

  Ground<bool>( !cobj );

  Ground<char>( *cobj );

  ++obj;
 } ;

/* concept CharFailStreamType<S> */

template <class S> // ref extended
concept CharFailStreamType = CharStreamType<S> && requires(S &obj)
 {
  obj.fail();
 } ;

/* charBaseValue() */

int charBaseValue(CharCodeType auto ch,unsigned base)
 {
  int dig=charHexValue(ch);

  if( (unsigned)dig>=base ) return -1;

  return dig;
 }

/* SkipSpace() */

void SkipSpace(CharStreamType auto &inp)
 {
  for(; +inp && charIsSpace(*inp) ;++inp);
 }

/* SkipNonSpace() */

void SkipNonSpace(CharStreamType auto &inp)
 {
  for(; +inp && !charIsSpace(*inp) ;++inp);
 }

/* ProbeChar() */

bool ProbeChar(CharStreamType auto &inp,char ch)
 {
  if( +inp )
    {
     if( *inp==ch )
       {
        ++inp;

        return true;
       }
    }

  return false;
 }

/* PassChar() */

template <CharFailStreamType S>
void PassChar(S &inp,char ch)
 {
  if( +inp && (*inp)==ch )
    {
     ++inp;
    }
  else
    {
     inp.fail();
    }
 }

/* PassChars() */

template <CharFailStreamType S,class ... CC>
void PassChars(S &inp,CC ... cc)
 {
  ( PassChar(inp,cc) , ... );
 }

/* PassOneOfChar() */

void PassOneOfChar(CharFailStreamType auto &inp,FuncType<bool,char> auto func)
 {
  if( +inp && func(*inp) )
    {
     ++inp;
    }
  else
    {
     inp.fail();
    }
 }

/* SkipAllOfChar() */

void SkipAllOfChar(CharStreamType auto &inp,FuncType<bool,char> auto func)
 {
  for(; +inp && func(*inp) ;++inp);
 }

} // namespace CCore

#endif


