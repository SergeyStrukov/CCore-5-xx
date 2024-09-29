/* CharUtils.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_CharUtils_h
#define CCore_inc_CharUtils_h

#include <CCore/inc/Printf.h>
#include <CCore/inc/Scanf.h>
#include <CCore/inc/Symbol.h>

namespace CCore {

/* classes */

struct FillCharBuf;

class PrintCharBuf;

class ScanCharBuf;

/* struct FillCharBuf */

struct FillCharBuf
 {
  ulen len;
  bool overflow;

  FillCharBuf(PtrLen<Char> out,StrLen text);
 };

/* class PrintCharBuf */

class PrintCharBuf : public PrintBase
 {
   static constexpr ulen Len = 2*Max_cast(MaxSymbolLen,16) ;

   PtrLen<Char> start;
   PtrLen<Char> out;
   bool overflow = false ;

   char buf[Len];
   ulen off = 0 ;
   ulen len = 0 ;

  private:

   void shift();

   void push(Char ch);

   void pushSymbol();

   void push();

   void pushAll();

   virtual PtrLen<char> do_provide(ulen hint_len);

   virtual void do_flush(char *ptr,ulen len);

  public:

   explicit PrintCharBuf(PtrLen<Char> out);

   ~PrintCharBuf();

   PtrLen<const Char> close(bool guard_overflow=true);
 };

/* class ScanCharBuf */

class ScanCharBuf : public ScanBase
 {
   static constexpr ulen Len = 2*Max_cast(MaxSymbolLen,16) ;

   PtrLen<const Char> str;

   char buf[Len];

  private:

   static ulen Put(PtrLen<char> out,PtrLen<const Char> &str);

   virtual PtrLen<const char> underflow();

  public:

   explicit ScanCharBuf(PtrLen<const Char> str);

   ~ScanCharBuf();
 };

} // namespace CCore

#endif

