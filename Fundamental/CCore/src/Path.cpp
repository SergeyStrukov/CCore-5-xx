/* Path.cpp */
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

#include <CCore/inc/Path.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* functions */

StrLen PrefixPath(StrLen file_name)
 {
  SplitPath split1(file_name);
  SplitPathName split2(split1.path);

  ulen len=split2.path.len;

  if( !len && !!split2 ) len=1;

  return file_name.prefix(split1.dev.len+len);
 }

StrLen SuffixExt(StrLen file_name)
 {
  SplitPath split1(file_name);
  SplitName split2(split1.path);
  SplitExt split3(split2.name);

  return split3.ext;
 }

bool PathIsRooted(StrLen path)
 {
  return +path && PathBase::IsSlash(path[0]) ;
 }

bool PathIsRel(StrLen path)
 {
  SplitDev split_dev(path);

  return !split_dev && !PathIsRooted(path) ;
 }

bool PathIsDev(StrLen path)
 {
  return +path && PathBase::IsColon(path.back(1)) ;
 }

bool PathIsBase(StrLen path)
 {
  if( +path )
    {
     char ch=path.back(1);

     if( PathBase::IsSlash(ch) || PathBase::IsColon(ch) ) return true;
    }

  return false;
 }

/* struct PathBase */

void PathBase::TurnSlash(PtrLen<char> name)
 {
  for(char &ch : name ) if( IsSlash(ch) ) ch='/';
 }

/* struct SplitDev */

SplitDev::SplitDev(StrLen dev_path_)
 {
  ForwardScan<IsColon,ToPrefix> scan(dev_path_);

  dev=scan.prefix;
  dev_path=scan.suffix;
  no_dev=scan.no;
 }

/* struct SplitPath */

SplitPath::SplitPath(StrLen dev_path)
 {
  BackwardScan<IsColon,ToPrefix> scan(dev_path);

  dev=scan.prefix;
  path=scan.suffix;
  no_dev=scan.no;
 }

/* struct SplitDir */

SplitDir::SplitDir(StrLen path_)
 {
  ForwardScan<IsSlash,ToPrefix> scan(path_);

  dir=scan.prefix;
  path=scan.suffix;
  no_path=scan.no;
 }

/* struct SplitName */

SplitName::SplitName(StrLen path_)
 {
  BackwardScan<IsSlash,ToPrefix> scan(path_);

  path=scan.prefix;
  name=scan.suffix;
  no_path=scan.no;
 }

/* struct SplitFullExt */

SplitFullExt::SplitFullExt(StrLen name_)
 {
  ForwardScan<IsDot,ToSuffix> scan(name_);

  name=scan.prefix;
  ext=scan.suffix;
  no_ext=scan.no;
 }

/* struct SplitExt */

SplitExt::SplitExt(StrLen name_)
 {
  BackwardScan<IsDot,ToSuffix> scan(name_);

  name=scan.prefix;
  ext=scan.suffix;
  no_ext=scan.no;
 }

/* struct SplitDevName */

SplitDevName::SplitDevName(StrLen dev_path_)
 {
  ForwardScan<IsColon,ToPrefixDel> scan(dev_path_);

  dev_name=scan.prefix;
  dev_path=scan.suffix;
  no_dev=scan.no;
 }

/* struct SplitDirName */

SplitDirName::SplitDirName(StrLen path_)
 {
  ForwardScan<IsSlash,ToPrefixDel> scan(path_);

  dir_name=scan.prefix;
  path=scan.suffix;
  no_path=scan.no;
 }

/* struct SplitPathName */

SplitPathName::SplitPathName(StrLen path_)
 {
  BackwardScan<IsSlash,ToPrefixDel> scan(path_);

  path=scan.prefix;
  name=scan.suffix;
  no_path=scan.no;
 }

} // namespace CCore


