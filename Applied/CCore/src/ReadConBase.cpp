/* ReadConBase.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/ReadConBase.h>

namespace CCore {

/* class ReadConBase */

void ReadConBase::shift()
 {
  if( !off ) return;

  for(ulen i=0; i<len ;i++) buf[i]=buf[i+off];

  off=0;
 }

ReadConBase::ReadConBase()
 : off(0),
   len(0)
 {
 }

ReadConBase::~ReadConBase()
 {
 }

 // get

bool ReadConBase::try_get(Symbol &ret)
 {
  while( len )
    {
     const char *ptr=buf+off;

     DetectSymbol det(ptr,len);

     switch( det.status )
       {
        case DetectSymbol::None :
         {
          return false;
         }
        break;

        case DetectSymbol::Done :
         {
          ret=det.get(ptr);

          off+=det.delta;
          len-=det.delta;

          return true;
         }
        break;

        default: // case DetectSymbol::Broken
         {
          off+=det.delta;
          len-=det.delta;
         }
       }
    }

  return false;
 }

Symbol ReadConBase::get()
 {
  Symbol ret;

  while( !get(DefaultTimeout,ret) );

  return ret;
 }

bool ReadConBase::get(MSec timeout,Symbol &ret)
 {
  return get(TimeScope(timeout),ret);
 }

bool ReadConBase::get(TimeScope time_scope,Symbol &ret)
 {
  if( try_get(ret) ) return true;

  shift();

  while( +time_scope.get() )
    {
     ulen delta=read(buf+len,Len-len,time_scope); // off==0

     if( delta==0 ) continue;

     len+=delta;

     if( try_get(ret) ) return true;

     shift();
    }

  return false;
 }

} // namespace CCore

