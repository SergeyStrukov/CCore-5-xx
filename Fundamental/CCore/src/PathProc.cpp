/* PathProc.cpp */
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

#include <CCore/inc/PathProc.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class PathProc */

void PathProc::push(ulen len)
 {
  if( stack_len<StackLen )
    {
     stack[stack_len++]=len;
    }
  else
    {
     auto r=Range(stack);

     for(; r.len>=2 ;++r) r[0]=r[1];

     r[0]=len;
    }
 }

void PathProc::pop()
 {
  if( stack_len>1 )
    {
     stack_len--;
    }
  else
    {
     ulen len=stack[0];

     for(len-=2; buf[len]!='/' ;len--);

     stack[0]=len+1;
    }
 }

PathProc::PathProc(BeginType bt)
 {
  if( bt==BeginEmpty )
    {
     buf_len=0;
    }
  else
    {
     buf[0]='/';
     buf_len=1;
    }

  init(buf_len);
 }

PathProc::PathProc(StrLen dev_name)
 {
  if( dev_name.len>=MaxPathLen-2 )
    {
     Printf(Exception,"CCore::PathProc::PathProc(...) : too long dev_name");
    }

  dev_name.copyTo(buf);

  buf[dev_name.len]=':';
  buf[dev_name.len+1]='/';

  buf_len=dev_name.len+2;

  init(buf_len);
 }

void PathProc::root()
 {
  buf_len=root_len;

  init();
 }

void PathProc::rootDir()
 {
  buf_len=root_len+1;

  buf[root_len]='.';

  init();
 }

void PathProc::add(StrLen name)
 {
  if( !name )
    {
     Printf(Exception,"CCore::PathProc::add(...) : empty name");
    }

  if( IsDotDot(name) )
    {
     back();
    }
  else
    {
     ulen len=top();

     if( buf_len-len==1 && buf[len]=='.' )
       {
        if( name.len>MaxPathLen-len )
          {
           Printf(Exception,"CCore::PathProc::add(...) : too long path");
          }

        name.copyTo(buf+len);

        buf_len=len+name.len;
       }
     else if( buf_len>root_len )
       {
        if( name.len>=MaxPathLen-buf_len )
          {
           Printf(Exception,"CCore::PathProc::add(...) : too long path");
          }

        buf[buf_len]='/';

        push(buf_len+1);

        name.copyTo(buf+buf_len+1);

        buf_len+=name.len+1;
       }
     else
       {
        if( name.len>MaxPathLen-buf_len )
          {
           Printf(Exception,"CCore::PathProc::add(...) : too long path");
          }

        name.copyTo(buf+buf_len);

        buf_len+=name.len;
       }
    }
 }

void PathProc::back()
 {
  ulen len=top();

  if( len>root_len )
    {
     buf_len=len-1;

     pop();
    }
  else
    {
     if( isRoot() || isRootDir() )
       {
        Printf(Exception,"CCore::PathProc::back() : at root");
       }

     rootDir();
    }
 }

void PathProc::addPath(StrLen path)
 {
  if( !path ) return;

  if( IsSlash(*path) )
    {
     rootDir();

     ++path;
    }

  while( +path )
    {
     SplitDirName split(path);

     if( !split )
       {
        add(path);

        break;
       }
     else
       {
        add(split.dir_name);

        path=split.path;
       }
    }
 }

void PathProc::copyPath(const PathProc &path)
 {
  if( &path==this ) return;

  buf_len=path.buf_len;

  Range(buf,buf_len).copy(path.buf);

  root_len=path.root_len;

  stack_len=path.stack_len;

  Range(stack,stack_len).copy(path.stack);
 }

} // namespace CCore

