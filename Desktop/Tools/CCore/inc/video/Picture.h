/* Picture.h */
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

#ifndef CCore_inc_video_Picture_h
#define CCore_inc_video_Picture_h

#include <CCore/inc/video/MPoint.h>

#include <CCore/inc/video/DrawBuf.h>

#include <CCore/inc/RefObjectBase.h>

namespace CCore {
namespace Video {

/* classes */

struct AbstractPicture;

class DefaultAppIcon;

/* struct AbstractPicture */

struct AbstractPicture
 {
  // abstract

  virtual ulen getCount() const =0;

  virtual Point getSize() const =0;

  virtual VColor defaultGround() const =0;

  virtual void draw(ulen index,DrawBuf buf,Coord dx,Coord dy) const =0;

  // helpers

  void draw(ulen index,const DrawBuf &buf,Pane place) const
   {
    draw(index,buf.cutRebase(place),place.dx,place.dy);
   }

  void draw(const DrawBuf &buf,Coord dx,Coord dy) const
   {
    draw(0,buf,dx,dy);
   }

  void draw(const DrawBuf &buf,Pane place) const
   {
    draw(0,buf.cutRebase(place),place.dx,place.dy);
   }

  Point sizeX(Coord dx) const
   {
    return {dx,YdivX(getSize())*dx};
   }

  Point sizeY(Coord dy) const
   {
    return {XdivY(getSize())*dy,dy};
   }
 };

/* type PictureBase */

using PictureBase = RefObjectBase<AbstractPicture> ;

/* GetNullPicturePtr() */

PictureBase * GetNullPicturePtr();

/* type Picture */

using Picture = RefObjectHook<PictureBase,AbstractPicture,GetNullPicturePtr> ;

/* class DefaultAppIcon */

class DefaultAppIcon : public Picture
 {
  public:

   DefaultAppIcon() noexcept;
 };

/* SetAppIcon() */

void SetAppIcon(Picture pict); // implemented by the target

} // namespace Video
} // namespace CCore

#endif

