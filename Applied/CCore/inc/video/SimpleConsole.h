/* SimpleConsole.h */
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

#ifndef CCore_inc_video_SimpleConsole_h
#define CCore_inc_video_SimpleConsole_h

#include <CCore/inc/video/FrameBuf.h>
#include <CCore/inc/Utf8.h>

namespace CCore {
namespace Video {

/* classes */

class DefaultFont;

template <RawColorType RawColor> class CharPanel;

template <RawColorType RawColor> class SimpleConsole;

/* class DefaultFont */

class DefaultFont
 {
   static const bool Table[];
   static const bool NoCharTable[];
   static const bool Marker[];

   const bool *pixel; // DX*DY

  public:

   static constexpr Coord DX = 10 ;
   static constexpr Coord DY = 18 ;
   static constexpr Coord BY = 15 ;

   DefaultFont()
    {
     pixel=Marker;
    }

   explicit DefaultFont(char ch)
    {
     unsigned index=(unsigned char)ch;

     if( index>=32 && index<128 )
       pixel=Table+(index-32)*(DX*DY);
     else
       pixel=NoCharTable;
    }

   // properties

   Coord dX() const { return DX; }

   Coord dY() const { return DY; }

   class Pattern
    {
      const bool *pixel;

     public:

      explicit Pattern(const bool *pixel_) : pixel(pixel_) {}

      // properties

      Coord dX() const { return DX; }

      bool operator [] (Coord x) const { return pixel[x]; }
    };

   Pattern operator [] (Coord y) const { return Pattern(pixel+(DX*y)); }
 };

/* class CharPanel<RawColor> */

template <RawColorType RawColor>
class CharPanel : NoCopy
 {
   FrameBuf<RawColor> out;

   VColor back   =  Black ;
   VColor fore   = Silver ;
   VColor line   =   Gray ;
   VColor marker =  Green ;

   Coord dx = 0 ;
   Coord dy = 0 ;

   RawColor backup[DefaultFont::DX*DefaultFont::DY];

  private:

   static Point Base(Point p) { return Point(p.x*DefaultFont::DX,p.y*DefaultFont::DY); }

   static Pane Cell(Point p) { return Pane(Base(p),DefaultFont::DX,DefaultFont::DY); }

   Pane row(Coord y) const { return Pane(0,y*DefaultFont::DY,out.dX(),DefaultFont::DY); }

  public:

   CharPanel() {}

   // properties

   bool operator + () const { return dy; }

   bool operator ! () const { return !dy; }

   Coord dX() const { return dx; }

   Coord dY() const { return dy; }

   // methods

   void init(FrameBuf<RawColor> out);

   void eraseLine(Coord y);

   void eraseLineLine(Coord y);

   void operator () (Point p,char ch);

   void setMarker(Point p);

   void clearMarker(Point p);
 };

template <RawColorType RawColor>
void CharPanel<RawColor>::init(FrameBuf<RawColor> out_)
 {
  out=out_;

  dx=out.dX()/DefaultFont::DX;
  dy=out.dY()/DefaultFont::DY;

  if( !dx ) dy=0;

  out.erase(back);
 }

template <RawColorType RawColor>
void CharPanel<RawColor>::eraseLine(Coord y)
 {
  out.block_unsafe(row(y),back);
 }

template <RawColorType RawColor>
void CharPanel<RawColor>::eraseLineLine(Coord y)
 {
  Pane pane=row(y);
  Pane top=SplitY(DefaultFont::DY-1,pane);

  out.block_unsafe(top,back);
  out.block_unsafe(pane,line);
 }

template <RawColorType RawColor>
void CharPanel<RawColor>::operator () (Point p,char ch)
 {
  out.glyph_unsafe(Base(p),DefaultFont(ch),back,fore);
 }

template <RawColorType RawColor>
void CharPanel<RawColor>::setMarker(Point p)
 {
  Pane cell=Cell(p);

  out.save_unsafe(cell,backup);

  out.glyph_unsafe(cell.getBase(),DefaultFont(),marker);
 }

template <RawColorType RawColor>
void CharPanel<RawColor>::clearMarker(Point p)
 {
  out.load_unsafe(Cell(p),backup);
 }

/* class SimpleConsole<RawColor> */

template <RawColorType RawColor>
class SimpleConsole : NoCopy
 {
   CharPanel<RawColor> panel;

   Point pos;

   bool marker = false ;

  private:

   void newLine();

   void printChar(char ch);

  public:

   SimpleConsole() {}

   // properties

   bool operator + () const { return +panel; }

   bool operator ! () const { return !panel; }

   // methods

   void init(FrameBuf<RawColor> out);

   void print(PtrLen<const char> str);

   void toggleMarker();
 };

template <RawColorType RawColor>
void SimpleConsole<RawColor>::newLine()
 {
  pos.x=0;

  if( ++pos.y>=panel.dY() )
    {
     pos.y=0;
    }

  if( pos.y+1<panel.dY() )
    {
     panel.eraseLine(pos.y);

     panel.eraseLineLine(pos.y+1);
    }
  else
    {
     panel.eraseLine(pos.y);

     panel.eraseLineLine(0);
    }
 }

template <RawColorType RawColor>
void SimpleConsole<RawColor>::printChar(char ch)
 {
  switch( ch )
    {
     case '\r' : pos.x=0; break;

     case '\n' : newLine(); break;

     case '\b' : if( pos.x>0 ) pos.x--; break;

     default:
      {
       if( Utf8Ext(ch) ) return;

       panel(pos,ch);

       if( ++pos.x>=panel.dX() ) newLine();
      }
    }
 }

template <RawColorType RawColor>
void SimpleConsole<RawColor>::init(FrameBuf<RawColor> out)
 {
  pos=Null;
  marker=false;

  panel.init(out);
 }

template <RawColorType RawColor>
void SimpleConsole<RawColor>::print(PtrLen<const char> str)
 {
  if( !panel ) return;

  if( marker ) panel.clearMarker(pos);

  for(char ch : str ) printChar(ch);

  if( marker ) panel.setMarker(pos);
 }

template <RawColorType RawColor>
void SimpleConsole<RawColor>::toggleMarker()
 {
  if( !panel ) return;

  if( marker )
    {
     marker=false;

     panel.clearMarker(pos);
    }
  else
    {
     marker=true;

     panel.setMarker(pos);
    }
 }

} // namespace Video
} // namespace CCore

#endif


