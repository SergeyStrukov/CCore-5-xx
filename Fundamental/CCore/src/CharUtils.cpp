/* CharUtils.cpp */
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

#include <CCore/inc/CharUtils.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* struct FillCharBuf */

FillCharBuf::FillCharBuf(PtrLen<Char> out,StrLen text)
 {
  ulen start=out.len;

  while( +text )
    {
     Char ch=CutChar_guarded(text);

     if( !out )
       {
        overflow=true;
        len=start;

        return;
       }

     *out=ch;

     ++out;
    }

  overflow=false;
  len=start-out.len;
 }

/* class PrintCharBuf */

void PrintCharBuf::shift()
 {
  if( off==0 ) return;

  for(ulen i=0; i<len ;i++) buf[i]=buf[i+off];

  off=0;
 }

void PrintCharBuf::push(Char ch)
 {
  if( !out )
    {
     overflow=true;
    }
  else
    {
     *out=ch;

     ++out;
    }
 }

void PrintCharBuf::pushSymbol()
 {
  Symbol sym=PeekSymbol_guarded(Range(buf+off,len));
  ulen delta=SymbolLen(sym);

  push(FromSymbol(sym));

  off+=delta;
  len-=delta;
 }

void PrintCharBuf::push()
 {
  while( len>=MaxSymbolLen ) pushSymbol();

  shift();
 }

void PrintCharBuf::pushAll()
 {
  while( len ) pushSymbol();

  off=0;
 }

PtrLen<char> PrintCharBuf::do_provide(ulen)
 {
  return Range(buf).part(off+len);
 }

void PrintCharBuf::do_flush(char *,ulen len_)
 {
  len+=len_;

  push();
 }

PrintCharBuf::PrintCharBuf(PtrLen<Char> out_)
 : start(out_),out(out_)
 {
 }

PrintCharBuf::~PrintCharBuf()
 {
 }

PtrLen<const Char> PrintCharBuf::close(bool guard_overflow)
 {
  flush();

  pushAll();

  if( guard_overflow && overflow )
    {
     Printf(Exception,"CCore::PrintCharBuf::close(true) : overflow");
    }

  return start.prefix(out);
 }

/* class ScanCharBuf */

ulen ScanCharBuf::Put(PtrLen<char> out,PtrLen<const Char> &str)
 {
  ulen start=out.len;

  for(; +str ;++str)
    {
     Symbol sym=ToSymbol(*str);
     StrLen r=SymbolRange(sym);

     if( r.len>out.len ) break;

     r.copyTo(out.ptr);

     out+=r.len;
    }

  return start-out.len;
 }

PtrLen<const char> ScanCharBuf::underflow()
 {
  ulen len=Put(Range(buf),str);

  return Range(buf,len);
 }

ScanCharBuf::ScanCharBuf(PtrLen<const Char> str_)
 : str(str_)
 {
  pump();
 }

ScanCharBuf::~ScanCharBuf()
 {
 }

} // namespace CCore

