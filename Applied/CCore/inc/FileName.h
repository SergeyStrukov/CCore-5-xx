/* FileName.h */
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

#ifndef CCore_inc_FileName_h
#define CCore_inc_FileName_h

#include <CCore/inc/TextTools.h>
#include <CCore/inc/Array.h>
#include <CCore/inc/Path.h>

namespace CCore {

/* classes */

class FileName;

/* class FileName */

class FileName : NoCopy
 {
   DynArray<char> buf;
   ulen off = 0 ;
   bool ok = false ;

  private:

   struct PathUp;

   struct InspectPath;

   class Out;

   void setAbs(StrLen file_name,SplitPath split_dev,SplitPathName split_file);

   void setRel(StrLen path,SplitPathName split_file);

  public:

   // constructors

   FileName() noexcept {}

   FileName(NothingType) {}

   explicit FileName(StrLen file_name);

   FileName(StrLen path,StrLen file_name); // path ends (if non-empty) with '/' or '\\' or ':'

   ~FileName() {}

   // std move

   FileName(FileName &&obj) noexcept
    : buf(std::move(obj.buf)),
      off(Replace(obj.off,0)),
      ok(Replace(obj.ok,false))
    {
    }

   FileName & operator = (FileName &&obj) noexcept
    {
     if( this!=&obj )
       {
        buf=std::move(obj.buf);
        off=Replace(obj.off,0);
        ok=Replace(obj.ok,false);
       }

     return *this;
    }

   // methods

   bool operator + () const { return ok; }

   bool operator ! () const { return !ok; }

   StrLen getStr() const { return Range(buf); }

   StrLen getPath() const { return getStr().prefix(off); }

   StrLen getFile() const { return getStr().part(off); }

   void printPos(PrinterType auto &out,TextPos pos) { Putobj(out,getFile(),pos); }

   // swap/move objects

   void objSwap(FileName &obj)
    {
     Swap(buf,obj.buf);
     Swap(off,obj.off);
     Swap(ok,obj.ok);
    }

   explicit FileName(ToMoveCtor<FileName> obj)
    : buf(ToMoveCtor(obj->buf)),
      off(obj->off),
      ok(obj->ok)
    {
    }
 };

} // namespace CCore

#endif


