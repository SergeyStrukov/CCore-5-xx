/* SaveLoad.h */
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

#ifndef CCore_inc_SaveLoad_h
#define CCore_inc_SaveLoad_h

#include <CCore/inc/MemberFold.h>

namespace CCore {

/* concept CoreSaveDevType<Dev> */

template <class Dev>
concept CoreSaveDevType = requires(Dev &dev,uint8 val,const uint8 *ptr,ulen len)
 {
  dev.do_put(val);

  dev.do_put(ptr,len);
 } ;

/* concept CoreRangeSaveDevType<Dev> */

template <class Dev>
concept CoreRangeSaveDevType = CoreSaveDevType<Dev> && requires(Dev &dev,ulen len)
 {
  Ground< PtrLen<uint8> >( dev.do_putRange(len) );
 } ;

/* concept SaveDevType<Dev> */

template <class Dev>
concept SaveDevType = requires(Dev &dev,uint8 val,const uint8 *ptr,ulen len,PtrLen<const uint8> buf)
 {
  dev.put(val);

  dev.put(ptr,len);

  dev.put(buf);
 } ;

/* concept RangeSaveDevType<Dev> */

template <class Dev>
concept RangeSaveDevType = SaveDevType<Dev> && requires(Dev &dev,ulen len)
 {
  Ground< PtrLen<uint8> >( dev.putRange(len) );
 } ;

/* concept CoreLoadDevType<Dev> */

template <class Dev>
concept CoreLoadDevType = requires(Dev &dev,uint8 *ptr,ulen len)
 {
  Ground<uint8>( dev.do_get() );

  dev.do_get(ptr,len);
 } ;

/* concept CoreRangeLoadDevType<Dev> */

template <class Dev>
concept CoreRangeLoadDevType = CoreLoadDevType<Dev> && requires(Dev &dev,ulen len)
 {
  Ground< PtrLen<const uint8> >( dev.do_getRange(len) );
 } ;

/* concept LoadDevType<Dev> */

template <class Dev>
concept LoadDevType = requires(Dev &dev,uint8 *ptr,ulen len,PtrLen<uint8> buf)
 {
  Ground<uint8>( dev.get() );

  dev.get(ptr,len);

  dev.get(buf);
 } ;

/* concept RangeLoadDevType<Dev> */

template <class Dev>
concept RangeLoadDevType = LoadDevType<Dev> && requires(Dev &dev,ulen len)
 {
  Ground< PtrLen<const uint8> >( dev.getRange(len) );
 } ;

/* concept ExtraLoadDevType<Dev> */

template <class Dev>
concept ExtraLoadDevType = RangeLoadDevType<Dev> && requires(Dev &dev,ulen len)
 {
  Ground< PtrLen<const uint8> >( dev.getFinalRange() );

  Ground< PtrLen<const uint8> >( dev.getFinalRange(len) );

  dev.fail();
 };

/* code template */

 //
 //   // save/load object
 //
 //   static constexpr ulen SaveLoadLen = SaveLenCounter<>::SaveLoadLen ;
 //
 //   void save((Range)SaveDevType auto &dev) const
 //    {
 //     dev.template use<BeOrder>();
 //    }
 //
 //   void load((Range)LoadDevType auto &dev)
 //    {
 //     dev.template use<BeOrder>();
 //    }
 //

 //
 //   // save/load object
 //
 //   using SaveLoadOrder = BeOrder ;
 //
 //   template <template <class ...> class Ret,class T>
 //   static constexpr auto Fold()
 //    {
 //     return Ret(&T::a,&T::b,&T::c);
 //    }
 //

/* functions */

template <class Proxy,class T,class Dev>
void ProxyLoad(T &obj,Dev &dev)
 {
  Proxy proxy;

  dev(proxy);

  obj=proxy.get();
 }

template <CursorType R,class Dev>
void SaveRange(R r,Dev &dev)
 {
  for(; +r ;++r) dev(*r);
 }

template <class T,class Dev>
void SaveRange(const T *ptr,ulen len,Dev &dev)
 {
  SaveRange(Range(ptr,len),dev);
 }

template <CursorType R,class Dev>
void LoadRange(R r,Dev &dev)
 {
  for(; +r ;++r) dev(*r);
 }

template <class T,class Dev>
void LoadRange(T *ptr,ulen len,Dev &dev)
 {
  LoadRange(Range(ptr,len),dev);
 }

template <class Custom,CursorType R,class Dev>
void SaveRange_use(R r,Dev &dev)
 {
  for(; +r ;++r) dev.template use<Custom>(*r);
 }

template <class Custom,class T,class Dev>
void SaveRange_use(const T *ptr,ulen len,Dev &dev)
 {
  SaveRange_use<Custom>(Range(ptr,len),dev);
 }

template <class Custom,CursorType R,class Dev>
void LoadRange_use(R r,Dev &dev)
 {
  for(; +r ;++r) dev.template use<Custom>(*r);
 }

template <class Custom,class T,class Dev>
void LoadRange_use(T *ptr,ulen len,Dev &dev)
 {
  LoadRange_use<Custom>(Range(ptr,len),dev);
 }

/* classes */

struct SaveLoadBe16;

struct SaveLoadBe32;

struct SaveLoadBe64;

struct SaveLoadLe16;

struct SaveLoadLe32;

struct SaveLoadLe64;

struct NeOrder;

struct BeOrder;

struct LeOrder;

template <class T,class Custom> struct SaveAdapter;

template <class T,class Custom> struct LoadAdapter;

template <class Dev> class PutDevBase;

template <class Dev> class GetDevBase;

class BufPutDev;

class BufGetDev;

class CountPutDev;

struct SaveLoadLenHelper;

struct Has_SaveLoadLenHelper;

template <class T> struct Get_SaveLoadLenCtor;

template <class ... TT> struct SaveLenCounter;

template <class ... TT> struct SplitLoad;

class RangeGetDev;

/* struct SaveLoadBe16 */

struct SaveLoadBe16
 {
  uint8 buf[2];

  // constructors

  SaveLoadBe16() : buf() {}

  SaveLoadBe16(uint8 b0,uint8 b1)
   {
    buf[0]=b0;
    buf[1]=b1;
   }

  explicit SaveLoadBe16(uint16 value)
   {
    buf[0]=uint8(value>>8);
    buf[1]=uint8(value   );
   }

  // methods

  uint16 get() const
   {
    uint16 b0=buf[0];
    uint16 b1=buf[1];

    return uint16( (b0<<8)|b1 );
   }

  // save/load object

  static constexpr ulen SaveLoadLen = 2 ;

  void save(SaveDevType auto &dev) const
   {
    dev.put(buf,2);
   }

  void load(LoadDevType auto &dev)
   {
    dev.get(buf,2);
   }
 };

/* struct SaveLoadBe32 */

struct SaveLoadBe32
 {
  uint8 buf[4];

  // constructors

  SaveLoadBe32() : buf() {}

  SaveLoadBe32(uint8 b0,uint8 b1,uint8 b2,uint8 b3)
   {
    buf[0]=b0;
    buf[1]=b1;
    buf[2]=b2;
    buf[3]=b3;
   }

  explicit SaveLoadBe32(uint32 value)
   {
    buf[0]=uint8(value>>24);
    buf[1]=uint8(value>>16);
    buf[2]=uint8(value>> 8);
    buf[3]=uint8(value    );
   }

  // methods

  uint32 get() const
   {
    uint32 b0=buf[0];
    uint32 b1=buf[1];
    uint32 b2=buf[2];
    uint32 b3=buf[3];

    return uint32( (b0<<24)|(b1<<16)|(b2<<8)|b3 );
   }

  // save/load object

  static constexpr ulen SaveLoadLen = 4 ;

  void save(SaveDevType auto &dev) const
   {
    dev.put(buf,4);
   }

  void load(LoadDevType auto &dev)
   {
    dev.get(buf,4);
   }
 };

/* struct SaveLoadBe64 */

struct SaveLoadBe64
 {
  uint8 buf[8];

  // constructors

  SaveLoadBe64() : buf() {}

  SaveLoadBe64(uint8 b0,uint8 b1,uint8 b2,uint8 b3,uint8 b4,uint8 b5,uint8 b6,uint8 b7)
   {
    buf[0]=b0;
    buf[1]=b1;
    buf[2]=b2;
    buf[3]=b3;
    buf[4]=b4;
    buf[5]=b5;
    buf[6]=b6;
    buf[7]=b7;
   }

  explicit SaveLoadBe64(uint64 value)
   {
    buf[0]=uint8(value>>56);
    buf[1]=uint8(value>>48);
    buf[2]=uint8(value>>40);
    buf[3]=uint8(value>>32);
    buf[4]=uint8(value>>24);
    buf[5]=uint8(value>>16);
    buf[6]=uint8(value>> 8);
    buf[7]=uint8(value    );
   }

  // methods

  uint64 get() const
   {
    uint64 b0=buf[0];
    uint64 b1=buf[1];
    uint64 b2=buf[2];
    uint64 b3=buf[3];
    uint64 b4=buf[4];
    uint64 b5=buf[5];
    uint64 b6=buf[6];
    uint64 b7=buf[7];

    return uint64( (b0<<56)|(b1<<48)|(b2<<40)|(b3<<32)|(b4<<24)|(b5<<16)|(b6<<8)|b7 );
   }

  // save/load object

  static constexpr ulen SaveLoadLen = 8 ;

  void save(SaveDevType auto &dev) const
   {
    dev.put(buf,8);
   }

  void load(LoadDevType auto &dev)
   {
    dev.get(buf,8);
   }
 };

/* struct SaveLoadLe16 */

struct SaveLoadLe16
 {
  uint8 buf[2];

  // constructors

  SaveLoadLe16() : buf() {}

  SaveLoadLe16(uint8 b0,uint8 b1)
   {
    buf[0]=b0;
    buf[1]=b1;
   }

  explicit SaveLoadLe16(uint16 value)
   {
    buf[0]=uint8(value   );
    buf[1]=uint8(value>>8);
   }

  // methods

  uint16 get() const
   {
    uint16 b0=buf[0];
    uint16 b1=buf[1];

    return uint16( (b1<<8)|b0 );
   }

  // save/load object

  static constexpr ulen SaveLoadLen = 2 ;

  void save(SaveDevType auto &dev) const
   {
    dev.put(buf,2);
   }

  void load(LoadDevType auto &dev)
   {
    dev.get(buf,2);
   }
 };

/* struct SaveLoadLe32 */

struct SaveLoadLe32
 {
  uint8 buf[4];

  // constructors

  SaveLoadLe32() : buf() {}

  SaveLoadLe32(uint8 b0,uint8 b1,uint8 b2,uint8 b3)
   {
    buf[0]=b0;
    buf[1]=b1;
    buf[2]=b2;
    buf[3]=b3;
   }

  explicit SaveLoadLe32(uint32 value)
   {
    buf[0]=uint8(value    );
    buf[1]=uint8(value>> 8);
    buf[2]=uint8(value>>16);
    buf[3]=uint8(value>>24);
   }

  // methods

  uint32 get() const
   {
    uint32 b0=buf[0];
    uint32 b1=buf[1];
    uint32 b2=buf[2];
    uint32 b3=buf[3];

    return uint32( (b3<<24)|(b2<<16)|(b1<<8)|b0 );
   }

  // save/load object

  static constexpr ulen SaveLoadLen = 4 ;

  void save(SaveDevType auto &dev) const
   {
    dev.put(buf,4);
   }

  void load(LoadDevType auto &dev)
   {
    dev.get(buf,4);
   }
 };

/* struct SaveLoadLe64 */

struct SaveLoadLe64
 {
  uint8 buf[8];

  // constructors

  SaveLoadLe64() : buf() {}

  SaveLoadLe64(uint8 b0,uint8 b1,uint8 b2,uint8 b3,uint8 b4,uint8 b5,uint8 b6,uint8 b7)
   {
    buf[0]=b0;
    buf[1]=b1;
    buf[2]=b2;
    buf[3]=b3;
    buf[4]=b4;
    buf[5]=b5;
    buf[6]=b6;
    buf[7]=b7;
   }

  explicit SaveLoadLe64(uint64 value)
   {
    buf[0]=uint8(value    );
    buf[1]=uint8(value>> 8);
    buf[2]=uint8(value>>16);
    buf[3]=uint8(value>>24);
    buf[4]=uint8(value>>32);
    buf[5]=uint8(value>>40);
    buf[6]=uint8(value>>48);
    buf[7]=uint8(value>>56);
   }

  // methods

  uint64 get() const
   {
    uint64 b0=buf[0];
    uint64 b1=buf[1];
    uint64 b2=buf[2];
    uint64 b3=buf[3];
    uint64 b4=buf[4];
    uint64 b5=buf[5];
    uint64 b6=buf[6];
    uint64 b7=buf[7];

    return uint64( (b7<<56)|(b6<<48)|(b5<<40)|(b4<<32)|(b3<<24)|(b2<<16)|(b1<<8)|b0 );
   }

  // save/load object

  static constexpr ulen SaveLoadLen = 8 ;

  void save(SaveDevType auto &dev) const
   {
    dev.put(buf,8);
   }

  void load(LoadDevType auto &dev)
   {
    dev.get(buf,8);
   }
 };

/* struct NeOrder */

struct NeOrder
 {
  // empty
 };

/* struct BeOrder */

struct BeOrder
 {
  template <class Dev>
  static void Save16(uint16 obj,Dev &dev)
   {
    dev(SaveLoadBe16(obj));
   }

  template <class Dev>
  static void Save32(uint32 obj,Dev &dev)
   {
    dev(SaveLoadBe32(obj));
   }

  template <class Dev>
  static void Save64(uint64 obj,Dev &dev)
   {
    dev(SaveLoadBe64(obj));
   }

  template <class Dev>
  static void Load16(uint16 &obj,Dev &dev)
   {
    ProxyLoad<SaveLoadBe16>(obj,dev);
   }

  template <class Dev>
  static void Load32(uint32 &obj,Dev &dev)
   {
    ProxyLoad<SaveLoadBe32>(obj,dev);
   }

  template <class Dev>
  static void Load64(uint64 &obj,Dev &dev)
   {
    ProxyLoad<SaveLoadBe64>(obj,dev);
   }
 };

/* struct LeOrder */

struct LeOrder
 {
  template <class Dev>
  static void Save16(uint16 obj,Dev &dev)
   {
    dev(SaveLoadLe16(obj));
   }

  template <class Dev>
  static void Save32(uint32 obj,Dev &dev)
   {
    dev(SaveLoadLe32(obj));
   }

  template <class Dev>
  static void Save64(uint64 obj,Dev &dev)
   {
    dev(SaveLoadLe64(obj));
   }

  template <class Dev>
  static void Load16(uint16 &obj,Dev &dev)
   {
    ProxyLoad<SaveLoadLe16>(obj,dev);
   }

  template <class Dev>
  static void Load32(uint32 &obj,Dev &dev)
   {
    ProxyLoad<SaveLoadLe32>(obj,dev);
   }

  template <class Dev>
  static void Load64(uint64 &obj,Dev &dev)
   {
    ProxyLoad<SaveLoadLe64>(obj,dev);
   }
 };

/* concept Has_SaveLoadOrder<T> */

template <class T>
concept Has_SaveLoadOrder = requires()
 {
  typename T::SaveLoadOrder;
 } ;

/* struct SaveAdapter<T,Custom> */

template <class T,class Custom>
struct SaveAdapter
 {
  template <class Dev>
  static void Save(const T &obj,Dev &dev) { obj.save(dev); }
 };

template <class T,class Custom> requires ( Has_SaveLoadOrder<T> )
struct SaveAdapter<T,Custom>
 {
  template <class Dev>
  static void Save(const T &obj,Dev &dev)
   {
    Members<T>(&obj, [&] (auto & ... tt) { dev.template use<typename T::SaveLoadOrder>(tt...); } );
   }
 };

template <class Custom>
struct SaveAdapter<uint8,Custom>
 {
  template <class Dev>
  static void Save(uint8 obj,Dev &dev) { dev.put(obj); }
 };

template <class Custom>
struct SaveAdapter<uint16,Custom>
 {
  template <class Dev>
  static void Save(uint16 obj,Dev &dev) { Custom::Save16(obj,dev); }
 };

template <class Custom>
struct SaveAdapter<uint32,Custom>
 {
  template <class Dev>
  static void Save(uint32 obj,Dev &dev) { Custom::Save32(obj,dev); }
 };

template <class Custom>
struct SaveAdapter<uint64,Custom>
 {
  template <class Dev>
  static void Save(uint64 obj,Dev &dev) { Custom::Save64(obj,dev); }
 };

/* struct LoadAdapter<T,Custom> */

template <class T,class Custom>
struct LoadAdapter
 {
  template <class Dev>
  static void Load(T &obj,Dev &dev) { obj.load(dev); }
 };

template <class T,class Custom> requires ( Has_SaveLoadOrder<T> )
struct LoadAdapter<T,Custom>
 {
  template <class Dev>
  static void Load(T &obj,Dev &dev)
   {
    Members<T>(&obj, [&] (auto & ... tt) { dev.template use<typename T::SaveLoadOrder>(tt...); } );
   }
 };

template <class Custom>
struct LoadAdapter<uint8,Custom>
 {
  template <class Dev>
  static void Load(uint8 &obj,Dev &dev) { obj=dev.get(); }
 };

template <class Custom>
struct LoadAdapter<uint16,Custom>
 {
  template <class Dev>
  static void Load(uint16 &obj,Dev &dev) { Custom::Load16(obj,dev); }
 };

template <class Custom>
struct LoadAdapter<uint32,Custom>
 {
  template <class Dev>
  static void Load(uint32 &obj,Dev &dev) { Custom::Load32(obj,dev); }
 };

template <class Custom>
struct LoadAdapter<uint64,Custom>
 {
  template <class Dev>
  static void Load(uint64 &obj,Dev &dev) { Custom::Load64(obj,dev); }
 };

/* class PutDevBase<Dev> */

template <class Dev>
class PutDevBase
 {
  private:

   Dev * getDev() { return static_cast<Dev *>(this); }

  public:

   // put

   void put(uint8 value) requires ( CoreSaveDevType<Dev> ) { getDev()->do_put(value); }

   void put(const uint8 *ptr,ulen len) requires ( CoreSaveDevType<Dev> ) { getDev()->do_put(ptr,len); }

   void put(PtrLen<const uint8> data) requires ( CoreSaveDevType<Dev> ) { getDev()->do_put(data.ptr,data.len); }

   PtrLen<uint8> putRange(ulen len) requires ( CoreRangeSaveDevType<Dev> ) { return getDev()->do_putRange(len); } // may return Empty

   // save

   template <class Custom,class ... TT>
   void use(const TT & ... tt)
    {
     ( SaveAdapter<TT,Custom>::Save(tt,*this) , ... );
    }

   template <class ... TT>
   void operator () (const TT & ... tt)
    {
     use<NeOrder>(tt...);
    }
 };

/* class GetDevBase<Dev> */

template <class Dev>
class GetDevBase
 {
  private:

   Dev * getDev() { return static_cast<Dev *>(this); }

  public:

   // get

   uint8 get() requires ( CoreLoadDevType<Dev> ) { return getDev()->do_get(); }

   void get(uint8 *ptr,ulen len) requires ( CoreLoadDevType<Dev> ) { getDev()->do_get(ptr,len); }

   void get(PtrLen<uint8> buf) requires ( CoreLoadDevType<Dev> ) { getDev()->do_get(buf.ptr,buf.len); }

   PtrLen<const uint8> getRange(ulen len) requires ( CoreRangeLoadDevType<Dev> ) { return getDev()->do_getRange(len); } // may return Empty

   // load

   template <class Custom,class ... TT>
   void use(TT & ... tt)
    {
     ( LoadAdapter<TT,Custom>::Load(tt,*this) , ... );
    }

   template <class ... TT>
   void operator () (TT & ... tt)
    {
     use<NeOrder>(tt...);
    }
 };

/* class BufPutDev */

class BufPutDev : public PutDevBase<BufPutDev>
 {
   uint8 *buf;

  public:

   explicit BufPutDev(uint8 *buf_) : buf(buf_) {}

   // methods

   void do_put(uint8 value) { *(buf++)=value; }

   void do_put(const uint8 *ptr,ulen len) { Range(ptr,len).copyTo(buf); buf+=len; }

   PtrLen<uint8> do_putRange(ulen len) { PtrLen<uint8> ret(buf,len); buf+=len; return ret; }

   // getRest()

   uint8 * getRest() const { return buf; }
 };

/* class BufGetDev */

class BufGetDev : public GetDevBase<BufGetDev>
 {
   const uint8 *buf;

  public:

   explicit BufGetDev(const uint8 *buf_) : buf(buf_) {}

   // methods

   uint8 do_get() { return *(buf++); }

   void do_get(uint8 *ptr,ulen len) { Range(ptr,len).copy(buf); buf+=len; }

   PtrLen<const uint8> do_getRange(ulen len) { PtrLen<const uint8> ret(buf,len); buf+=len; return ret; }

   // getRest()

   const uint8 * getRest() const { return buf; }
 };

/* class CountPutDev */

class CountPutDev : public PutDevBase<CountPutDev>
 {
   ULenSat count;

  public:

   CountPutDev() {}

   operator ULenSat() const { return count; }

   // methods

   void do_put(uint8) { count+=1u; }

   void do_put(const uint8 *,ulen len) { count+=len; }

   PtrLen<uint8> do_putRange(ulen len) { count+=len; return Empty; }

   // Count()

   template <class T>
   static ULenSat Count(const T &t)
    {
     CountPutDev dev;

     dev(t);

     return dev;
    }
 };

/* struct SaveLoadLenHelper */

struct SaveLoadLenHelper
 {
  constexpr SaveLoadLenHelper() {}

  template <class ... TT>
  constexpr ulen operator () () { return SaveLenCounter<TT...>::SaveLoadLen; }
 };

/* struct Has_SaveLoadLenHelper */

struct Has_SaveLoadLenHelper
 {
  constexpr Has_SaveLoadLenHelper() {}

  template <class ... TT>
  constexpr bool operator () () { return SaveLenCounter<TT...>::Has_SaveLoadLen; }
 };

/* concept Has_SaveLoadLen<T> */

template <class T>
concept Has_SaveLoadLen = Meta::OneOf<T,uint8,uint16,uint32,uint64>
 || ( Has_SaveLoadOrder<T> && Members<T>(Has_SaveLoadLenHelper()) )
 || requires()
 {
  Ground<ulen>( T::SaveLoadLen );
 } ;

/* concept No_SaveLoadLen<T> */

template <class T>
concept No_SaveLoadLen = !Has_SaveLoadLen<T> ;

/* struct Get_SaveLoadLenCtor<T> */

template <class T>
struct Get_SaveLoadLenCtor : Meta::DefConst<ulen,T::SaveLoadLen> {};

template <class T> requires ( Has_SaveLoadOrder<T> )
struct Get_SaveLoadLenCtor<T> : Meta::DefConst<ulen, Members<T>(SaveLoadLenHelper()) > {};

template <>
struct Get_SaveLoadLenCtor<uint8> : Meta::DefConst<ulen,1> {};

template <>
struct Get_SaveLoadLenCtor<uint16> : Meta::DefConst<ulen,2> {};

template <>
struct Get_SaveLoadLenCtor<uint32> : Meta::DefConst<ulen,4> {};

template <>
struct Get_SaveLoadLenCtor<uint64> : Meta::DefConst<ulen,8> {};

/* const Get_SaveLoadLen<T> */

template <Has_SaveLoadLen T>
inline constexpr ulen Get_SaveLoadLen = Get_SaveLoadLenCtor<T>::Ret ;

/* struct SaveLenCounter<TT> */

template <class ... TT>
struct SaveLenCounter
 {
  static constexpr bool Has_SaveLoadLen = false ;

  static ULenSat Count(const TT & ... tt)
   {
    return ( ULenSat() + ... + CountPutDev::Count(tt) );
   }
 };

template <class ... TT> requires ( ... && Has_SaveLoadLen<TT> )
struct SaveLenCounter<TT...>
 {
  static constexpr bool Has_SaveLoadLen = true ;

  static constexpr ulen SaveLoadLen = UIntConstAdd<ulen,Get_SaveLoadLen<TT>...> ;

  static ULenSat Count(const TT & ...) { return SaveLoadLen; }
 };

/* concept BufLoadableType<T> */

template <class T>
concept BufLoadableType = Has_SaveLoadLen<T> && ( OneOfTypes<T,uint8,uint16,uint32,uint64> ||
requires(T &obj,BufGetDev &dev)
 {
  obj.load(dev);

 } ) ;

/* struct SplitLoad<TT> */

template <>
struct SplitLoad<>
 {
  SplitLoad() {}

  template <class Custom,class Dev>
  void use(Dev &)
   {
   }

  template <class Custom,class Dev,class ... SS>
  void use_buf(Dev &dev,SS & ... ss)
   {
    dev.template use_buf<Custom>(ss...);
   }

  template <class Custom,class Dev,class ... SS>
  void use_rest(Dev &dev,SS & ... ss)
   {
    dev.template use_rest<Custom>(ss...);
   }
 };

template <BufLoadableType T,class ... TT>
struct SplitLoad<T,TT...>
 {
  T &first;
  SplitLoad<TT...> rest;

  SplitLoad(T &t,TT & ... tt) : first(t),rest(tt...) {}

  template <class Custom,class Dev>
  void use(Dev &dev)
   {
    rest.template use_buf<Custom>(dev,first);
   }

  template <class Custom,class Dev,class ... SS>
  void use_buf(Dev &dev,SS & ... ss)
   {
    rest.template use_buf<Custom>(dev,ss...,first);
   }

  template <class Custom,class Dev,class ... SS>
  void use_rest(Dev &dev,SS & ... ss)
   {
    rest.template use_rest<Custom>(dev,ss...,first);
   }
 };

template <class T,class ... TT>
struct SplitLoad<T,TT...>
 {
  T &first;
  SplitLoad<TT...> rest;

  SplitLoad(T &t,TT & ... tt) : first(t),rest(tt...) {}

  template <class Custom,class Dev>
  void use(Dev &dev)
   {
    rest.template use_rest<Custom>(dev,first);
   }

  template <class Custom,class Dev,class ... SS>
  void use_buf(Dev &dev,SS & ... ss)
   {
    dev.template use_buf<Custom>(ss...);

    rest.template use_rest<Custom>(dev,first);
   }

  template <class Custom,class Dev,class ... SS>
  void use_rest(Dev &dev,SS & ... ss)
   {
    rest.template use_rest<Custom>(dev,ss...,first);
   }
 };

/* class RangeGetDev */

class RangeGetDev
 {
   PtrLen<const uint8> range;
   bool nok;

  public:

   explicit RangeGetDev(PtrLen<const uint8> range_) : range(range_),nok(false) {}

   bool operator ! () const { return nok; }

   // get

   uint8 get()
    {
     if( !range )
       {
        nok=true;

        return 0;
       }
     else
       {
        uint8 ret=*range;

        ++range;

        return ret;
       }
    }

   void get(uint8 *ptr,ulen len)
    {
     if( range.len<len )
       {
        Range(ptr,len).set_null();

        nok=true;
       }
     else
       {
        (range+=len).copyTo(ptr);
       }
    }

   void get(PtrLen<uint8> buf) { get(buf.ptr,buf.len); }

   PtrLen<const uint8> getRange(ulen len) // may return Empty
    {
     if( range.len<len )
       {
        nok=true;

        return Empty;
       }
     else
       {
        return range+=len;
       }
    }

   // getRest()

   PtrLen<const uint8> getRest() const { return range; }

   // extra

   PtrLen<const uint8> getFinalRange()
    {
     ulen len=range.len;

     return range+=len;
    }

   PtrLen<const uint8> getFinalRange(ulen len)
    {
     if( range.len!=len )
       {
        nok=true;

        return Empty;
       }
     else
       {
        return range+=len;
       }
    }

   void fail() { nok=true; }

   NegBool finish() { if( +range ) nok=true; return nok; }

   // split load

   template <class Custom>
   void use_buf() {}

   template <class Custom,class ... TT>
   void use_buf(TT & ... tt)
    {
     const uint8 *buf=getRange(SaveLenCounter<TT...>::SaveLoadLen).ptr;

     if( nok ) return;

     BufGetDev dev(buf);

     dev.use<Custom>(tt...);
    }

   template <class Custom>
   void use_rest() {}

   template <class Custom,class T,class ... TT>
   void use_rest(T &t,TT & ... tt)
    {
     LoadAdapter<T,Custom>::Load(t,*this);

     use<Custom>(tt...);
    }

   template <class Custom>
   void use() {}

   template <class Custom,class ... TT>
   void use(TT & ... tt)
    {
     SplitLoad<TT...> split(tt...);

     split.template use<Custom>(*this);
    }

   template <class ... TT>
   void operator () (TT & ... tt)
    {
     use<NeOrder>(tt...);
    }
 };

/* SaveLen() */

template <class ... TT>
ULenSat SaveLen(const TT & ... tt)
 {
  return SaveLenCounter<TT...>::Count(tt...);
 }

} // namespace CCore

#endif


