/* ConfigBinder.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_ConfigBinder_h
#define CCore_inc_video_ConfigBinder_h

#include <CCore/inc/video/BindBagProxy.h>
#include <CCore/inc/video/ConfigStore.h>

namespace CCore {
namespace Video {

/* classes */

struct ConfigItemBind;

struct ConfigItemHost;

struct HomeSyncBase;

template <class ... TT> struct CombineBags;

template <class Bag,class ... TT> class ConfigBinder;

/* struct ConfigItemBind */

struct ConfigItemBind
 {
  virtual void group(String name)=0;

  virtual void space()=0;

  virtual void item(String name,unsigned &var)=0;

  virtual void item(String name,Coord &var)=0;

  virtual void item(String name,Fraction &var)=0;

  virtual void item(String name,VColor &var)=0;

  virtual void item(String name,Clr &var)=0;

  virtual void item(String name,Point &var)=0;

  virtual void item(String name,String &var)=0;

  virtual void item(String name,FontCouple &var)=0;

  virtual void item(String name,bool &var)=0;

  virtual void item(String name,Ratio &var)=0;
 };

/* struct ConfigItemHost */

struct ConfigItemHost
 {
  virtual void bind(ConfigItemBind &binder)=0;
 };

/* struct HomeSyncBase */

struct HomeSyncBase
 {
  virtual void syncMap(ConfigMap &map)=0;

  virtual void updateMap(ConfigMap &map) const = 0;

  bool syncHome(StrLen home_dir,StrLen cfg_file) noexcept; // "/dir" "/file" , true iff load file is ok

  void updateHome(StrLen home_dir,StrLen cfg_file) const noexcept; // "/dir" "/file"
 };

/* struct CombineBags<TT...> */

template <class ... TT>
struct CombineBags : TT...
 {
  CombineBags() noexcept {}

  template <class Ptr,class Func>
  static void Members(Ptr ptr,Func func)
   {
    ( TT::Members(ptr,func) , ... );
   }

  void bindItems(ConfigItemBind &binder)
   {
    ( TT::bindItems(binder) , ... );
   }

  void findFonts()
   {
    ( TT::findFonts() , ... );
   }
 };

/* class ConfigBinder<Bag,TT> */

template <class Bag,class ... TT>
class ConfigBinder : NoCopyBase<Bag> , public HomeSyncBase , public ConfigItemHost
 {
   template <class T>
   struct Item
    {
     T obj;
    };

   struct Pack : Item<TT>...
    {
    };

   Pack pack;

  private:

   template <class T>
   void bind()
    {
     BindBagProxy( static_cast<Item<T> &>(pack).obj ,get(),getSmartConfig());
    }

  public:

   // constructors

   ConfigBinder() noexcept
    {
     ( bind<TT>() , ... );
    }

   ~ConfigBinder() {}

   // methods

   const Bag & get() const { return *this; }

   Bag & ref() { return *this; }

   template <OneOfTypes<TT...> T>
   const T & getConfig() const
    {
     return static_cast<const Item<T> &>(pack).obj;
    }

   virtual void syncMap(ConfigMap &map)
    {
     Bag::Members((Bag *)this, [&map] (StrLen name,auto &obj) { map.sync(name,obj); } );
    }

   virtual void updateMap(ConfigMap &map) const
    {
     Bag::Members((const Bag *)this, [&map] (StrLen name,auto &obj) { map.update(name,obj); } );
    }

   virtual void bind(ConfigItemBind &binder)
    {
     this->bindItems(binder);
    }

   // getSmartConfig()

   class Proxy
    {
      const ConfigBinder *obj;

     public:

      Proxy(const ConfigBinder *obj_) : obj(obj_) {}

      template <OneOfTypes<TT...> T>
      operator const T & () const { return obj->template getConfig<T>(); }
    };

   Proxy getSmartConfig() const { return this; }
 };

} // namespace Video
} // namespace CCore

#endif

