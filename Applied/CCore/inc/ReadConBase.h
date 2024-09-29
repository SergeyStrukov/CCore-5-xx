/* ReadConBase.h */
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

#ifndef CCore_inc_ReadConBase_h
#define CCore_inc_ReadConBase_h

#include <CCore/inc/TimeScope.h>
#include <CCore/inc/Symbol.h>

namespace CCore {

/* concept ReadConType<ReadCon> */

template <class ReadCon>
concept ReadConType = requires(ReadCon &con,MSec timeout,TimeScope time_scope,Symbol &ret,char ch,const char *ptr,ulen len,StrLen str)
 {
  Ground<Symbol>( con.get() );

  Ground<bool>( con.try_get(ret) );

  Ground<bool>( con.get(timeout,ret) );

  Ground<bool>( con.get(time_scope,ret) );

  con.put(ch);

  con.put(ptr,len);

  con.put(str);

  con.put(ret);
 } ;

/* classes */

class ReadConBase;

/* class ReadConBase */

class ReadConBase : NoCopy
 {
   static constexpr ulen Len = 2*Max_cast(MaxSymbolLen,16) ;

   char buf[Len];
   ulen off;
   ulen len;

  private:

   void shift();

   virtual ulen read(char *ptr,ulen len,TimeScope time_scope)=0;

  public:

   ReadConBase();

   ~ReadConBase();

   // get

   bool try_get(Symbol &ret);

   Symbol get();

   bool get(MSec timeout,Symbol &ret);

   bool get(TimeScope time_scope,Symbol &ret);
 };

} // namespace CCore

#endif

