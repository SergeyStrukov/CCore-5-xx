/* FileName.cpp */
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

#include <CCore/inc/FileName.h>

#include <CCore/inc/OutBuf.h>

namespace CCore {

/* class FileName */

struct FileName::PathUp : PathBase
 {
  StrLen path;
  ulen up;
  bool ok;

  PathUp(StrLen path,ulen up) // path ends (if non-empty) with '/' or '\\' or ':'
   : ok(false)
   {
    SplitPath split_dev(path);

    ulen len=split_dev.path.len;

    for(; len && up ;up--)
      {
       SplitName split({split_dev.path.ptr,len-1});

       switch( split.getNameType() )
         {
          case Name :
           {
            len=split.path.len;
           }
          break;

          case DotDotName :
           {
            set(path.prefix(split_dev.dev.len+len),up);
           }
          return;

          default: return;
         }
      }

    set(path.prefix(split_dev.dev.len+len),up);
   }

  void set(StrLen path_,ulen up_)
   {
    path=path_;
    up=up_;
    ok=true;
   }
 };

struct FileName::InspectPath
 {
  DynArray<StrLen> list;
  ulen up = 0 ;
  bool root = false ;
  bool ok = false ;

  explicit InspectPath(SplitPathName split_file)
   : list(DoReserve,10)
   {
    list.append_copy(split_file.name);

    StrLen path=split_file.path;

    for(;;)
      {
       SplitPathName split_path(path);

       switch( split_path.getNameType() )
         {
          case SplitPathName::Name :
           {
            if( up )
              up--;
            else
              list.append_copy(split_path.name);
           }
          break;

          case SplitPathName::EmptyName :
           {
            if( !!split_path || up ) return;

            root=true;
           }
          break;

          case SplitPathName::DotName :
           {
            // skip
           }
          break;

          case SplitPathName::DotDotName :
           {
            up++;
           }
          break;
         }

       if( !split_path ) break;

       path=split_path.path;
      }

    ok=true;
   }

  bool shrink(StrLen &path)
   {
    if( up )
      {
       PathUp path_up(path,up);

       if( !path_up.ok ) return false;

       path=path_up.path;
       up=path_up.up;
      }

    return true;
   }

  template <class T,class S>
  void put(T &out,S set) const
   {
    if( root ) out('/');

    for(ulen count=up; count ;count--) out('.','.','/');

    auto r=RangeReverse(list);

    for(; r.len>1 ;++r) out(*r,'/');

    set();

    out(*r);
   }

  ulen countLen(ulen &off,ulen len) const
   {
    OutCount<ulen,char> counter(len);

    put(counter, [&] () { off=counter; } );

    return counter;
   }

  ULenSat countLenSat(ulen &off,ulen len) const
   {
    OutCount<ULenSat,char> counter(len);

    put(counter, [&] () { off=ULenSat(counter).value; } );

    return counter;
   }

  void put(OutBuf<char> &out) const
   {
    put(out, [] () {} );
   }
 };

class FileName::Out : public OutBuf<char>
 {
  public:

   Out(DynArray<char> &buf,ulen len) : OutBuf<char>(buf.extend_raw(len).ptr) {}
 };

void FileName::setAbs(StrLen file_name,SplitPath split_dev,SplitPathName split_file)
 {
  if( !split_file )
    {
     buf.extend_copy(file_name);

     off=split_dev.dev.len;
    }
  else
    {
     // inspect

     InspectPath inspect(split_file);

     if( !inspect.ok ) return;

     // count len

     ulen len=inspect.countLen(off,split_dev.dev.len);

     // build

     Out out(buf,len);

     out(split_dev.dev);

     inspect.put(out);
    }

  ok=true;
 }

void FileName::setRel(StrLen path,SplitPathName split_file)
 {
  if( !split_file )
    {
     ulen len=LenAdd(path.len,split_file.name.len);

     Out out(buf,len);

     out(path,split_file.name);

     off=path.len;
    }
  else
    {
     // inspect

     InspectPath inspect(split_file);

     if( !inspect.ok || inspect.root ) return;

     if( !inspect.shrink(path) ) return;

     // count len

     ulen off_;

     ULenSat len=inspect.countLenSat(off_,path.len);

     if( !len ) return;

     off=off_;

     // build

     Out out(buf,len.value);

     out(path);

     inspect.put(out);
    }

  ok=true;
 }

FileName::FileName(StrLen file_name)
 {
  SplitPath split_dev(file_name);

  SplitPathName split_file(split_dev.path);

  if( split_file.getNameType()==SplitPathName::Name )
    {
     setAbs(file_name,split_dev,split_file);
    }
 }

FileName::FileName(StrLen path,StrLen file_name)
 {
  SplitPath split_dev(file_name);

  SplitPathName split_file(split_dev.path);

  if( split_file.getNameType()==SplitPathName::Name )
    {
     if( !split_dev && !PathIsRooted(file_name) )
       {
        setRel(path,split_file);
       }
     else
       {
        setAbs(file_name,split_dev,split_file);
       }
    }
 }

} // namespace CCore


