/* BookWindow.cpp */
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

#include <CCore/inc/video/book/BookWindow.h>
#include <CCore/inc/video/book/DrawBook.h>

#include <CCore/inc/Scope.h>

#include <CCore/inc/video/FigureLib.h>
#include <CCore/inc/video/LayoutCombo.h>
#include <CCore/inc/algon/BinarySearch.h>

#include <CCore/inc/video/DesktopKey.h>

namespace CCore {
namespace Video {
namespace Book {

/* class InnerBookWindow */

class InnerBookWindow::Shape : public DrawBook::Shape
 {
  public:

   Shape() noexcept {}
 };

bool InnerBookWindow::cache() const
 {
  if( block_cache ) return false;

  try
    {
     Scope scope("App::InnerBookWindow::cache()"_c);

     Coord wdx=getSize().x-2*cfg.width.get().roundUp();

     if( !ok || cache_wdx!=wdx )
       {
        DrawBook::SetExactArrayLen(shapes,frames.len);

        Point s;

        for(ulen i=0; i<frames.len ;i++)
          {
           auto &shape=shapes[i];

           Point ds=shape.set(cfg,map,scale,frames[i],i,wdx,s.y);

           updateReplace.assert();

           s=StackYSize_guarded(s,ds);
          }

        size=s;

        ok=true;
        cache_wdx=wdx;
       }

     return true;
    }
  catch(...)
    {
     block_cache=true;

     return false;
    }
 }

auto InnerBookWindow::getVisibleShapes(Coord off,Coord lim) const -> PtrLen<const Shape>
 {
  PtrLen<const Shape> r=Range(shapes);

  r=Algon::BinarySearch_if(r, [lim] (const Shape &shape) { return shape.getDown() >= lim ; } );

  auto s=Algon::BinarySearch_if(r, [off] (const Shape &shape) { return shape.getDown() > off ; } );

  if( +s ) --r;

  return r;
 }

auto InnerBookWindow::getVisibleShapes() const -> PtrLen<const Shape>
 {
  Coord wdy=(Coord)sy.page;
  Coord pos_y=(Coord)sy.getPos();

  return getVisibleShapes(pos_y,pos_y+wdy);
 }

DrawBook::RefList InnerBookWindow::getRef(Point point) const
 {
  if( !cache() ) return {};

  Pane pane=getPane();

  Pane inner=pane.shrink(cfg.width.get().roundUp());

  point-=inner.getBase();

  Coord wdy=(Coord)sy.page;

  Coord pos_x=(Coord)sx.getPos();
  Coord pos_y=(Coord)sy.getPos();

  for(auto &shape : getVisibleShapes(pos_y,pos_y+wdy) )
    {
     Coord down=shape.getDown();

     Point size=shape.getSize();

     if( pos_x<size.x )
       {
        down-=pos_y;

        if( shape.hit(point,-pos_x,down) ) return shape.getRef(point,-pos_x,down);
       }
    }

  return {};
 }

void InnerBookWindow::addXPos(ulen delta,bool mul_flag)
 {
  sx.add(Delta(delta,mul_flag));

  scroll_x.assert(sx.pos);

  redraw();
 }

void InnerBookWindow::subXPos(ulen delta,bool mul_flag)
 {
  sx.sub(Delta(delta,mul_flag));

  scroll_x.assert(sx.pos);

  redraw();
 }

void InnerBookWindow::addYPos(ulen delta,bool mul_flag)
 {
  sy.add(Delta(delta,mul_flag));

  scroll_y.assert(sy.pos);

  redraw();
 }

void InnerBookWindow::subYPos(ulen delta,bool mul_flag)
 {
  sy.sub(Delta(delta,mul_flag));

  scroll_y.assert(sy.pos);

  redraw();
 }

void InnerBookWindow::begXPos()
 {
  sx.beg();

  scroll_x.assert(sx.pos);

  redraw();
 }

void InnerBookWindow::endXPos()
 {
  sx.end();

  scroll_x.assert(sx.pos);

  redraw();
 }

void InnerBookWindow::begYPos()
 {
  sy.beg();

  scroll_y.assert(sy.pos);

  redraw();
 }

void InnerBookWindow::endYPos()
 {
  sy.end();

  scroll_y.assert(sy.pos);

  redraw();
 }

void InnerBookWindow::addYPosPage()
 {
  sy.addPage();

  scroll_y.assert(sy.pos);

  redraw();
 }

void InnerBookWindow::subYPosPage()
 {
  sy.subPage();

  scroll_y.assert(sy.pos);

  redraw();
 }

void InnerBookWindow::posX(ulen pos)
 {
  sx.setPos(pos);

  redraw();
 }

void InnerBookWindow::posY(ulen pos)
 {
  sy.setPos(pos);

  redraw();
 }

void InnerBookWindow::updated(unsigned flags)
 {
  if( flags&LayoutUpdate ) ok=false;
 }

InnerBookWindow::InnerBookWindow(SubWindowHost &host,const Config &cfg_,DrawBook::ExtMap &map_)
 : SubWindow(host),
   cfg(cfg_),

   map(map_),

   connector_posX(this,&InnerBookWindow::posX),
   connector_posY(this,&InnerBookWindow::posY),

   connector_updated(this,&InnerBookWindow::updated,host.getFrame()->updated)
 {
 }

InnerBookWindow::~InnerBookWindow()
 {
 }

 // methods

Point InnerBookWindow::getMinSize(Point cap) const
 {
  if( !cache() ) return Inf(Point(500,300),cap);

  if( frames.len )
    {
     Point delta=2*Point::Diag(cfg.width.get().roundUp());

     return Inf(size,cap-delta)+delta;
    }
  else
    {
     return Inf(Point(500,300),cap);
    }
 }

void InnerBookWindow::setPage(Book::TypeDef::Page *page,VColor book_back_,VColor book_fore_)
 {
  frames=Null;

  book_back=book_back_;
  book_fore=book_fore_;

  if( page )
    {
     frames=page->list;

     back=DrawBook::Combine(page->back,book_back_);
     fore=DrawBook::Combine(page->fore,book_fore_);
    }
  else
    {
     back=book_back_;
     fore=book_fore_;
    }

  sx.beg();
  sy.beg();

  block_cache=false;
  ok=false;
 }

void InnerBookWindow::setPage(Book::TypeDef::Page *page)
 {
  frames=Null;

  if( page )
    {
     frames=page->list;

     back=DrawBook::Combine(page->back,book_back);
     fore=DrawBook::Combine(page->fore,book_fore);
    }
  else
    {
     back=book_back;
     fore=book_fore;
    }

  sx.beg();
  sy.beg();

  block_cache=false;
  ok=false;
 }

void InnerBookWindow::posFrame(PtrLen<const UIntType auto> index_list)
 {
  if( !cache() ) return;

  if( !index_list )
    {
     sy.beg();
    }
  else
    {
     ulen frame_index=index_list[0];

     ++index_list;

     if( frame_index<shapes.getLen() )
       {
        if( shapes[frame_index].open(index_list) )
          {
           ok=false;

           changed.assert();

           if( !cache() ) return;
          }

        sy.setPos(ulen( shapes[frame_index].getDown(map,index_list) ));
       }
     else
       {
        sy.end();
       }
    }

  scroll_y.assert(sy.pos);
 }

void InnerBookWindow::setScale(Ratio scale_)
 {
  scale=scale_;

  ok=false;
 }

ulen InnerBookWindow::getFrameIndex() const
 {
  if( !cache() ) return 0;

  Coord pos_y=(Coord)sy.getPos();

  PtrLen<const Shape> r=Range(shapes);

  auto s=Algon::BinarySearch_if(r, [off=pos_y] (const Shape &shape) { return shape.getDown() > off ; } );

  if( +s ) --r;

  if( !r ) return 0;

  return Dist(shapes.getPtr(),r.ptr);
 }

 // special methods

void InnerBookWindow::adjustScrollPos()
 {
  sx.adjustPos();
  sy.adjustPos();
 }

 // drawing

void InnerBookWindow::layout()
 {
  if( !cache() )
    {
     sx.pos=0;
     sx.total=1;
     sx.page=1;

     sy.pos=0;
     sy.total=1;
     sy.page=1;

     return;
    }

  Point s=getSize()-2*Point::Diag(cfg.width.get().roundUp());

  if( s>Null )
    {
     sx.total=(ulen)size.x;
     sx.page=(ulen)s.x;

     sy.total=(ulen)size.y;
     sy.page=(ulen)s.y;
    }
  else
    {
     sx.total=(ulen)size.x;
     sx.page=1;

     sy.total=(ulen)size.y;
     sy.page=1;
    }
 }

void InnerBookWindow::draw(DrawBuf buf,DrawParam) const
 {
  if( !cache() )
    {
     buf.erase(Black);

     return;
    }

  Scope scope("App::InnerBookWindow::draw()"_c);

  Pane pane=getPane();

  SmoothDrawArt art(buf);

  // back , fore

  MCoord width=+cfg.width;

  VColor back=DrawBook::Combine(this->back,+cfg.back);
  VColor fore=DrawBook::Combine(this->fore,+cfg.fore);

  art.erase(back);

  // border

  {
   MPane p(pane);

   if( +p )
     {
      FigureBox fig(p);

      VColor vc = focus? +cfg.focus : +cfg.border ;

      fig.loop(art,width,vc);
     }
  }

  // frames

  Pane inner=pane.shrink(RoundUpLen(width));

  if( !inner ) return;

  buf=buf.cutRebase(inner);

  Coord wdy=(Coord)sy.page;

  Coord pos_x=(Coord)sx.getPos();
  Coord pos_y=(Coord)sy.getPos();

  for(auto &shape : getVisibleShapes(pos_y,pos_y+wdy) )
    {
     Coord down=shape.getDown();

     Point size=shape.getSize();

     if( pos_x<size.x )
       {
        shape.draw(cfg,map,fore,buf, -pos_x , down-pos_y );
       }
    }
 }

 // base

void InnerBookWindow::open()
 {
  focus=false;
 }

 // keyboard

void InnerBookWindow::gainFocus()
 {
  if( Change(focus,true) ) redraw();
 }

void InnerBookWindow::looseFocus()
 {
  if( Change(focus,false) ) redraw();
 }

 // mouse

MouseShape InnerBookWindow::getMouseShape(Point point,KeyMod) const
 {
  DrawBook::RefType ref=getRef(point).ref;

  return +ref?Mouse_Hand:Mouse_Arrow;
 }

 // user input

void InnerBookWindow::react(UserAction action)
 {
  action.dispatch(*this);
 }

void InnerBookWindow::react_Key(VKey vkey,KeyMod kmod,unsigned repeat)
 {
  switch( vkey )
    {
     case VKey_Left :
      {
       subXPos(repeat,!(kmod&KeyMod_Shift));
      }
     break;

     case VKey_Right :
      {
       addXPos(repeat,!(kmod&KeyMod_Shift));
      }
     break;

     case VKey_Up :
      {
       subYPos(repeat,!(kmod&KeyMod_Shift));
      }
     break;

     case VKey_Down :
      {
       addYPos(repeat,!(kmod&KeyMod_Shift));
      }
     break;

     case VKey_Home :
      {
       begXPos();
      }
     break;

     case VKey_End :
      {
       endXPos();
      }
     break;

     case VKey_PageUp :
      {
       if( kmod&KeyMod_Ctrl )
         begYPos();
       else
         subYPosPage();
      }
     break;

     case VKey_PageDown :
      {
       if( kmod&KeyMod_Ctrl )
         endYPos();
       else
         addYPosPage();
      }
     break;
    }
 }

void InnerBookWindow::react_LeftClick(Point point,MouseKey)
 {
  DrawBook::RefList ref=getRef(point);

  if( !ref.ref ) return;

  struct Func
   {
    InnerBookWindow *obj;
    RefArray<ulen> index_list;

    void operator () (Book::TypeDef::Link *ref)
     {
      obj->link.assert(*ref,index_list);
     }

    void operator () (Book::TypeDef::Page *ref)
     {
      obj->hint.assert(ref);
     }

    void operator () (Book::TypeDef::Collapse *ref)
     {
      ref->open=!ref->open;

      obj->ok=false;

      obj->changed.assert();
     }
   };

  Func func{this,ref.index_list};

  ElaborateAnyPtr(func,ref.ref);
 }

void InnerBookWindow::react_Wheel(Point,MouseKey mkey,Coord delta)
 {
  if( delta<0 )
    {
     if( mkey&MouseKey_Shift )
       addXPos(IntAbs(delta),true);
     else
       addYPos(IntAbs(delta),true);
    }
  else
    {
     if( mkey&MouseKey_Shift )
       subXPos(IntAbs(delta),true);
     else
       subYPos(IntAbs(delta),true);
    }
 }

/* class DisplayBookWindow */

void DisplayBookWindow::changed()
 {
  layout();

  redraw();
 }

DisplayBookWindow::DisplayBookWindow(SubWindowHost &host,const ConfigType &cfg,DrawBook::ExtMap &map)
 : ScrollableWindow<InnerBookWindow>(host,cfg,map),

   connector_changed(this,&DisplayBookWindow::changed,window.changed),

   link(window.link),
   hint(window.hint),
   updateReplace(window.updateReplace)
 {
  scroll_x.setStep(10);
  scroll_y.setStep(10);
 }

DisplayBookWindow::~DisplayBookWindow()
 {
 }

 // methods

void DisplayBookWindow::setPage(Book::TypeDef::Page *page,VColor book_back,VColor book_fore)
 {
  window.setPage(page,book_back,book_fore);

  layout();
 }

void DisplayBookWindow::setPage(Book::TypeDef::Page *page)
 {
  window.setPage(page);

  layout();

  redraw();
 }

void DisplayBookWindow::setPage(Book::TypeDef::Page *page,PtrLen<const UIntType auto> index_list)
 {
  window.setPage(page);

  layout();

  window.posFrame(index_list);
 }

void DisplayBookWindow::posFrame(PtrLen<const UIntType auto> index_list)
 {
  window.posFrame(index_list);
 }

void DisplayBookWindow::setScale(Ratio scale)
 {
  window.setScale(scale);

  layout();

  redraw();
 }

ulen DisplayBookWindow::getFrameIndex() const
 {
  return window.getFrameIndex();
 }

 // drawing

void DisplayBookWindow::drawBack(DrawBuf,DrawParam &draw_param) const
 {
  draw_param.back_done=false;
 }

/* class DisplayBookFrame */

DisplayBookFrame::DisplayBookFrame(Desktop *desktop,const Config &cfg_,DrawBook::ExtMap &map,Signal<> &update)
 : DragFrame(desktop,cfg_.frame_cfg,update),
   cfg(cfg_),
   client(*this,cfg.book_cfg,map),

   updateReplace(client.updateReplace)
 {
  bindClient(client);
 }

DisplayBookFrame::~DisplayBookFrame()
 {
 }

 // methods

void DisplayBookFrame::setPage(VColor book_back,VColor book_fore)
 {
  client.setPage(0,book_back,book_fore);

  client.redraw();
 }

void DisplayBookFrame::setPage(Book::TypeDef::Page *page)
 {
  client.setPage(page);
 }

void DisplayBookFrame::setScale(Ratio scale)
 {
  client.setScale(scale);
 }

bool DisplayBookFrame::getPlace(Pane &ret) const
 {
  if( place.ok )
    {
     ret=place.get();

     return true;
    }

  return false;
 }

 // base

void DisplayBookFrame::dying()
 {
  DragFrame::dying();

  place.set(getFrameHost()->getPlace());
 }

/* class BookWindow */

void BookWindow::AppBag::bindItems(ConfigItemBind &binder)
 {
  binder.group("Book"_str);

   binder.item("back"_str,back);
   binder.item("fore"_str,fore);
   binder.item("font"_str,font);
   binder.item("codefont"_str,codefont);
   binder.item("decorfont"_str,decorfont);
   binder.item("default scale"_str,defscale);

   binder.space();

   binder.item("'Title'"_str,text_Title);
   binder.item("'Page'"_str,text_Page);
   binder.item("'Not ready'"_str,text_NotReady);
   binder.item("'Fonts'"_str,text_Font);
   binder.item("'Fav Books'"_str,text_FavBooks);

   binder.space();

   binder.item("'Open favorite'"_str,hint_FavBooks);
   binder.item("'Previuos page'"_str,hint_PrevPage);
   binder.item("'Parent page'"_str,hint_ParentPage);
   binder.item("'Next page'"_str,hint_NextPage);
   binder.item("'Scale'"_str,hint_Scale);
   binder.item("'Font'"_str,hint_Font);
   binder.item("'Reload'"_str,hint_Reload);
   binder.item("'Back'"_str,hint_GotoBack);
   binder.item("'Fore'"_str,hint_GotoFore);
 }

void BookWindow::AppBag::findFonts()
 {
  DialogFontLookup dev;

  font=dev.build("Times New Roman"_c,22);
  codefont=dev.build("Anonymous Pro"_c,22);
  decorfont=dev.build("Georgia"_c|Bold|Italic,20);
 }

StrLen BookWindow::FavFile() { return "/BookFav.ddl"_c; }

void BookWindow::error(String etext)
 {
  if( msg.isDead() )
    {
     msg.setInfo(etext);

     msg.create(getFrame(),+cfg.text_Error);

     disableFrameReact();
    }
 }

void BookWindow::setNav(Book::TypeDef::Page *page)
 {
  if( page )
    {
     cur=page;
     prev=page->prev;
     up=page->up;
     next=page->next;
    }
  else
    {
     cur=0;
     prev=0;
     up=0;
     next=0;
    }

  knob_prev.enable(prev);
  knob_up.enable(up);
  knob_next.enable(next);
 }

void BookWindow::enableFrame()
 {
  enableFrameReact();
 }

void BookWindow::font_completed(bool ok)
 {
  font_flag=false;

  if( ok )
    {
     wlist.del(progress);

     wlist.insTop(knob_fav,label_title,text_title,label_page,text_page,knob_prev,knob_up,knob_next,
                  line2,spinor,line3,btn_replace,knob_reload,line4,back_btn,fore_btn,book);

     book.setFocus();

     if( Change(opt.ok,false) )
       {
        load(opt.file_name);
       }

     redraw();
    }
 }

void BookWindow::push(Book::TypeDef::Page *page,RefArray<ulen> index_list)
 {
  if( page )
    {
     history.shrink(history.getLen()-history_index);

     if( history.getLen()>HistoryCap )
       {
        auto data=Range(history).suffix(HistoryCap/2);

        DynArray<History> temp(DoCopy(data.len),data.ptr);

        history=std::move(temp);

        history_index=history.getLen();
       }

     history.append_copy({page,index_list});

     history_index++;

     back_btn.enable();

     fore_btn.disable();
    }
 }

void BookWindow::pushCur()
 {
  RefArray<ulen> index_list(DoFill(1),book.getFrameIndex());

  push(cur,index_list);
 }

void BookWindow::goTo(Book::TypeDef::Page *page,PtrLen<const UIntType auto> index_list)
 {
  if( page )
    {
     if( page!=cur )
       {
        text_page.setText(String(page->name.getStr()));

        setNav(page);

        layout();

        book.setPage(page,index_list);

        redraw();
       }
     else
       {
        book.posFrame(index_list);

        redraw();
       }
    }
 }

void BookWindow::link(Book::TypeDef::Link dst,RefArray<ulen> index_list)
 {
  if( auto *page=dst.page.getPtr() )
    {
     push(cur,index_list);

     goTo(page,Range_const(dst.index_list.getRange()));
    }
 }

void BookWindow::gotoNav(Book::TypeDef::Page *page)
 {
  if( page )
    {
     pushCur();

     goTo<ulen>(page,Empty);
    }
 }

void BookWindow::gotoHistory(History obj)
 {
  goTo(obj.page,Range_const(obj.index_list));
 }

void BookWindow::back()
 {
  if( history_index>0 )
    {
     if( history_index==history.getLen() )
       {
        pushCur();

        history_index--;
       }

     gotoHistory(history[--history_index]);

     back_btn.enable( history_index>0 );
     fore_btn.enable( history.getLen()-history_index > 1u );
    }
 }

void BookWindow::fore()
 {
  if( history.getLen()-history_index > 1u )
    {
     gotoHistory(history[++history_index]);

     back_btn.enable();
     fore_btn.enable( history.getLen()-history_index > 1u );
    }
 }

void BookWindow::hint(Book::TypeDef::Page *page)
 {
  popup.setPage(page);

  if( page )
    {
     String title(page->name.getStr());

     if( popup.isDead() )
       {
        Pane place;

        if( !popup.getPlace(place) )
          {
           Pane pane=toScreen(getPane());

           place=SplitX(pane,Div(1,3)*pane.dx);
          }

        popup.create(getFrame(),place,title);
       }
     else
       {
        popup.setTitle(title);
       }
    }
 }

void BookWindow::gotoPrev()
 {
  gotoNav(prev);
 }

void BookWindow::gotoUp()
 {
  gotoNav(up);
 }

void BookWindow::gotoNext()
 {
  gotoNav(next);
 }

void BookWindow::setScale(int scale_)
 {
  Ratio scale=Div(scale_,100);

  ext_map.setScale(scale);

  book.setScale(scale);
  popup.setScale(scale);
 }

void BookWindow::updateReplace()
 {
  replace_frame.update();
 }

void BookWindow::openReplace()
 {
  if( replace_frame.isDead() ) replace_frame.create(getFrame());
 }

void BookWindow::replaceApply()
 {
  reload();
 }

void BookWindow::openFav()
 {
  if( fav_frame.isDead() )
    {
     if( source_ok ) fav_frame.setInsData(text_title.getText(),source_file);

     fav_frame.create(getFrame(),+cfg.text_FavBooks);
    }
 }

void BookWindow::favDestroyed()
 {
  String path=fav_frame.getSelectedPath();

  if( path.notEmpty() )
    {
     load(path);
    }
 }

BookWindow::BookWindow(SubWindowHost &host,const Config &cfg_,OptFileName opt_,Signal<> &update)
 : ComboWindow(host),
   cfg(cfg_),
   opt(opt_),

   ext_map(replace_map),

   knob_fav(wlist,cfg.knob_cfg,KnobShape::FaceLove),

   label_title(wlist,cfg.label_cfg,cfg.text_Title),
   text_title(wlist,cfg.text_cfg),

   label_page(wlist,cfg.label_cfg,cfg.text_Page),
   text_page(wlist,cfg.text_cfg),

   line1(wlist,cfg.line_cfg),

   knob_prev(wlist,cfg.knob_cfg,KnobShape::FaceLeft),
   knob_up(wlist,cfg.knob_cfg,KnobShape::FaceUp),
   knob_next(wlist,cfg.knob_cfg,KnobShape::FaceRight),

   line2(wlist,cfg.line_cfg),

   spinor(wlist,cfg.spinor_cfg),

   line3(wlist,cfg.line_cfg),

   btn_replace(wlist,cfg.btn_cfg,cfg.text_Font),
   knob_reload(wlist,cfg.knob_cfg,KnobShape::FaceReload),

   line4(wlist,cfg.line_cfg),

   back_btn(wlist,cfg.movebtn_cfg,MoveButtonShape::BackDir),
   fore_btn(wlist,cfg.movebtn_cfg,MoveButtonShape::ForeDir),

   book(wlist,cfg.book_cfg,ext_map),
   progress(wlist,cfg.progress_cfg),

   msg(host.getFrameDesktop(),cfg.msg_cfg,update),

   popup(host.getFrameDesktop(),cfg.popup_cfg,ext_map,update),

   replace_frame(host.getFrameDesktop(),cfg.replace_cfg,replace_map,update),

   fav_frame(host.getFrameDesktop(),cfg.fav_cfg,update,HomeKey(),FavFile()),

   progress_control(progress),
   font_inc(progress_control),

   connector_msg_destroyed(this,&BookWindow::enableFrame,msg.destroyed),
   connector_font_completed(this,&BookWindow::font_completed,font_inc.completed),

   connector_link(this,&BookWindow::link,book.link),
   connector_back_pressed(this,&BookWindow::back,back_btn.pressed),
   connector_fore_pressed(this,&BookWindow::fore,fore_btn.pressed),
   connector_hint(this,&BookWindow::hint,book.hint),

   connector_prev_pressed(this,&BookWindow::gotoPrev,knob_prev.pressed),
   connector_up_pressed(this,&BookWindow::gotoUp,knob_up.pressed),
   connector_next_pressed(this,&BookWindow::gotoNext,knob_next.pressed),

   connector_scale_changed(this,&BookWindow::setScale,spinor.changed),
   connector_book_updateReplace(this,&BookWindow::updateReplace,book.updateReplace),
   connector_popup_updateReplace(this,&BookWindow::updateReplace,popup.updateReplace),
   connector_knob_replace_pressed(this,&BookWindow::openReplace,btn_replace.pressed),
   connector_replace_apply(this,&BookWindow::replaceApply,replace_frame.apply),
   connector_knob_reload_pressed(this,&BookWindow::reload,knob_reload.pressed),
   connector_knob_fav_pressed(this,&BookWindow::openFav,knob_fav.pressed),
   connector_fav_destroyed(this,&BookWindow::favDestroyed,fav_frame.destroyed)
 {
  wlist.insTop(progress);

  knob_prev.disable();
  knob_up.disable();
  knob_next.disable();

  back_btn.disable();
  fore_btn.disable();

  spinor.setRange(25,400);
  spinor.setValue(100);
  spinor.setOptions(".f2"_c);

  knob_fav.bindHint(cfg.hint_FavBooks);

  knob_prev.bindHint(cfg.hint_PrevPage);
  knob_up.bindHint(cfg.hint_ParentPage);
  knob_next.bindHint(cfg.hint_NextPage);

  spinor.bindHint(cfg.hint_Scale);
  btn_replace.bindHint(cfg.hint_Font);
  knob_reload.bindHint(cfg.hint_Reload);
  back_btn.bindHint(cfg.hint_GotoBack);
  fore_btn.bindHint(cfg.hint_GotoFore);
 }

BookWindow::~BookWindow()
 {
 }

 // methods

Point BookWindow::getMinSize() const
 {
  Coord space=+cfg.space_dxy;

  LayToRight lay1{Lay(knob_fav),Lay(label_title),LayCenterY(text_title),Lay(label_page),LayCenterY(text_page)};

  LayToRight lay2{LayCenterY(knob_prev),LayCenterY(knob_up),LayCenterY(knob_next),Lay(line2),
                  LayCenterY(spinor),Lay(line3),LayCenterY(btn_replace),LayCenterY(knob_reload),Lay(line4),
                  LayCenterY(back_btn),LayAlignLeft(LayCenterY(fore_btn))};

  LayToBottom lay3{ExtLayNoSpace(lay1),ExtLayNoSpace(lay2),Lay(book)};

  LaySame lay(lay3,ExtLay{LayTop(progress)});

  return lay.getMinSize(space);
 }

void BookWindow::blank()
 {
  history.erase();
  history_index=0;

  source_file=Null;
  source_ok=false;

  back_btn.disable();
  fore_btn.disable();

  if( popup.isAlive() ) popup.destroy();

  popup.setPage(Book::NoColor,Book::NoColor);

  book.setPage(0,Book::NoColor,Book::NoColor);

  setNav(0);

  book_map.blank();
  ext_map.blank();

  text_title.setText(""_str);
  text_page.setText(""_str);

  spinor.setValue( (+cfg.defscale)*Coord(100) );

  setScale(spinor.getValue());

  book.setFocus();

  layout();

  redraw();
 }

void BookWindow::load(String file_name)
 {
  if( font_flag )
    {
     error(+cfg.text_NotReady);

     return;
    }

  blank();

  source_file=file_name;

  SimpleArray<char> temp(64_KByte);

  auto result=book_map.load(Range(file_name),Range(temp));

  if( result.ok )
    {
     auto *ptr=book_map.get();

     text_title.setText(String(ptr->title.getStr()));

     VColor book_back=DrawBook::CastColor(ptr->back);
     VColor book_fore=DrawBook::CastColor(ptr->fore);

     if( auto *page=ptr->start.getPtr() )
       {
        text_page.setText(String(page->name.getStr()));

        setNav(page);

        layout();

        ext_map.setSource(book_map.getFile(),book_map.getFileName());

        popup.setPage(book_back,book_fore);

        book.setPage(page,book_back,book_fore);
       }
     else
       {
        text_page.setText(""_str);

        setNav(0);

        layout();

        popup.setPage(book_back,book_fore);

        book.setPage(0,book_back,book_fore);
       }

     source_ok=true;

     redraw();
    }
  else
    {
     error(String(result.etext));
    }
 }

void BookWindow::reload()
 {
  load(source_file);
 }

 // drawing

void BookWindow::layout()
 {
  Coord space=+cfg.space_dxy;

  LayToRight lay1{Lay(knob_fav),Lay(label_title),LayCenterY(text_title),Lay(label_page),LayCenterY(text_page)};

  LayToRight lay2{LayCenterY(knob_prev),LayCenterY(knob_up),LayCenterY(knob_next),Lay(line2),
                  LayCenterY(spinor),Lay(line3),LayCenterY(btn_replace),LayCenterY(knob_reload),Lay(line4),
                  LayCenterY(back_btn),LayAlignLeft(LayCenterY(fore_btn))};

  LayToBottom lay3{ExtLayNoSpace(lay1),ExtLayNoSpace(lay2),Lay(book)};

  LaySame lay(lay3,ExtLay{LayTop(progress)});

  lay.setPlace(getPane(),space);
 }

void BookWindow::drawBack(DrawBuf buf,DrawParam &draw_param) const
 {
  VColor back=+cfg.back;

  if( book.isListed() )
    {
     PaneSub sub(getPane(),book.getPlace());

     draw_param.erase(buf,sub,back);
    }
  else
    {
     draw_param.erase(buf,back);
    }
 }

 // base

void BookWindow::open()
 {
  ComboWindow::open();

  replace_map.load();

  replace_frame.update();

  if( font_flag )
    {
     ext_map.cache(font_inc);
    }
 }

 // user input

void BookWindow::react(UserAction action)
 {
  action.dispatch(*this, [this] (UserAction action) { wlist.react(action); } );
 }

void BookWindow::react_Key(VKey vkey,KeyMod kmod)
 {
  switch( vkey )
    {
     case VKey_F5 :
      {
       gotoPrev();
      }
     break;

     case VKey_F6 :
      {
       gotoUp();
      }
     break;

     case VKey_F7 :
      {
       gotoNext();
      }
     break;

     case VKey_BackSpace :
      {
       back();
      }
     break;

     default:
      {
       wlist.put_Key(vkey,kmod);
      }
    }
 }

} // namespace Book
} // namespace Video
} // namespace CCore

