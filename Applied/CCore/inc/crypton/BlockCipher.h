/* BlockCipher.h */
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

#ifndef CCore_inc_crypton_BlockCipher_h
#define CCore_inc_crypton_BlockCipher_h

#include <CCore/inc/Gadget.h>

namespace CCore {
namespace Crypton {

/* functions */

void GuardNoCipherKey();

/* concept CipherFuncType<T> */

template <class T>
concept CipherFuncType = NothrowDtorType<T> && requires(T &func,const uint8 *src,uint8 *dst)
 {
  Ground<ulen>( T::BlockLen );
  Ground<ulen>( T::KeyLen );

  Ground<const char *>( T::GetName() );

  T();

  func.key(src);

  { func.unkey() } noexcept;

  func.apply(src,dst);

  func.apply(dst);
 } ;

/* classes */

template <CipherFuncType T> class BlockCipher;

/* class BlockCipher<T> */

template <CipherFuncType T>
class BlockCipher : NoCopy
 {
  public:

   // length in octets

   static constexpr ulen BlockLen = T::BlockLen ;
   static constexpr ulen KeyLen = T::KeyLen ;

   static const char * GetName() { return T::GetName(); }

  private:

   T func;
   bool ok;

  private:

   void guardKey() const
    {
     if( !ok ) GuardNoCipherKey();
    }

  public:

   // constructors

   BlockCipher() { unkey(); }

   ~BlockCipher() { func.unkey(); }

   explicit BlockCipher(const uint8 src[KeyLen]) : BlockCipher() { key(src); }

   // methods

   void key(const uint8 src[KeyLen]) { func.key(src); ok=true; }

   void unkey() { func.unkey(); ok=false; }

   void apply(const uint8 src[BlockLen],uint8 *restrict dst/* [BlockLen] */) const
    {
     guardKey();

     func.apply(src,dst);
    }

   void apply(uint8 src_dst[BlockLen]) const
    {
     guardKey();

     func.apply(src_dst);
    }
 };

/* concept CipherType<T> */

template <class T>
concept CipherType = NothrowDtorType<T> && requires(T &obj,const T &cobj,const uint8 *src,uint8 *dst)
 {
  Ground<ulen>( T::BlockLen );
  Ground<ulen>( T::KeyLen );

  Ground<const char *>( T::GetName() );

  T();

  T(src);

  obj.key(src);

  obj.unkey();

  cobj.apply(src,dst);

  cobj.apply(dst);
 } ;

} // namespace Crypton
} // namespace CCore

#endif


