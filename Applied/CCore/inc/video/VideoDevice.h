/* VideoDevice.h */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_video_VideoDevice_h
#define CCore_inc_video_VideoDevice_h

#include <CCore/inc/video/FrameBuf.h>

#include <CCore/inc/FunctorType.h>
#include <CCore/inc/Unid.h>

namespace CCore {
namespace Video {

/* enum ColorMode */

enum ColorMode
 {
  ColorMode_none = 0,

  ColorMode16,
  ColorMode24,
  ColorMode24Inv,
  ColorMode32,
  ColorMode32Inv
 };

/* classes */

template <template <class Color> class T> class MultiMode;

struct VideoMode;

struct VideoDim;

struct VideoDevice;

/* class MultiMode<T> */

template <template <class Color> class T>
class MultiMode
 {
   ColorMode mode = ColorMode_none ;

   union Union
    {
     T<RawColor16>    obj16;
     T<RawColor24>    obj24;
     T<RawColor24Inv> obj24inv;
     T<RawColor32>    obj32;
     T<RawColor32Inv> obj32inv;

     Union() {}
    };

   Union u;

  public:

   MultiMode() {}

   ~MultiMode() {}

   ColorMode operator + () const { return mode; }

   bool operator ! () const { return !mode; }

   template <class FuncInit>
   void init(ColorMode mode_,FuncInit func_init)
    {
     mode=mode_;

     FunctorTypeOf<FuncInit> func(func_init);

     switch( mode_ )
       {
        case ColorMode16    : CreateAt(u.obj16);    func(u.obj16);    break;
        case ColorMode24    : CreateAt(u.obj24);    func(u.obj24);    break;
        case ColorMode24Inv : CreateAt(u.obj24inv); func(u.obj24inv); break;
        case ColorMode32    : CreateAt(u.obj32);    func(u.obj32);    break;
        case ColorMode32Inv : CreateAt(u.obj32inv); func(u.obj32inv); break;
       }
    }

   void clear()
    {
     mode=ColorMode_none;
    }

   template <class FuncInit>
   void apply(FuncInit func_init)
    {
     FunctorTypeOf<FuncInit> func(func_init);

     switch( mode )
       {
        case ColorMode16    : func(u.obj16);    break;
        case ColorMode24    : func(u.obj24);    break;
        case ColorMode24Inv : func(u.obj24inv); break;
        case ColorMode32    : func(u.obj32);    break;
        case ColorMode32Inv : func(u.obj32inv); break;
       }
    }
 };

/* struct VideoMode */

struct VideoMode
 {
  ColorMode mode;
  Coord dx;
  Coord dy;
  int freq; // Hz

  VideoMode() noexcept : mode(ColorMode_none),dx(0),dy(0),freq(0) {}

  ColorMode operator + () const { return mode; }

  bool operator ! () const { return !mode; }
 };

/* struct VideoDim */

struct VideoDim
 {
  int hlen; // mm
  int vlen; // mm

  VideoDim() noexcept : hlen(0),vlen(0) {}
 };

/* struct VideoDevice */

struct VideoDevice
 {
  static const Unid TypeUnid;

  // not synchronized

  virtual bool updateVideoModeList(MSec timeout=DefaultTimeout)=0;

  virtual PtrLen<const VideoMode> getVideoModeList() const =0;

  virtual bool setVideoMode(ulen index)=0;

  virtual void setTick(MSec period)=0;

  virtual VideoDim getVideoDim() const =0;

  virtual ColorMode getColorMode() const =0;

  virtual VideoMode getVideoMode() const =0;

  virtual ColorPlane getPlane() const =0;

  // synchronized

  struct Control
   {
    // serialized

    virtual void change(bool plug,bool power)=0;

    virtual void tick()=0;
   };

  virtual void attach(Control *ctrl)=0;

  virtual void detach()=0;
 };

} // namespace Video
} // namespace CCore

#endif


