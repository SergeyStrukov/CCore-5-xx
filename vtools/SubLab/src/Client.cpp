/* Client.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: SubLab 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <inc/Client.h>
#include <inc/Subs.h>

#include <CCore/inc/video/FigureLib.h>

namespace App {

/* class SpaceWindow */

void SpaceWindow::startDrag(Point point)
 {
  if( Change(drag,true) )
    {
     drag_base=point;
     space_base=space;

     captureMouse();

     redraw();
    }
 }

void SpaceWindow::dragTo(Point point)
 {
  Point delta=point-drag_base;

  space=space_base+delta;

  redraw();

  changed.assert();
 }

void SpaceWindow::endDrag()
 {
  drag=false;

  releaseMouse();
 }

void SpaceWindow::endDrag(Point point)
 {
  endDrag();

  dragTo(point);
 }

SpaceWindow::SpaceWindow(SubWindowHost &host,const ConfigType &cfg_)
 : SubWindow(host),
   cfg(cfg_)
 {
  space=Point::Diag(+cfg.space_dxy);
 }

SpaceWindow::~SpaceWindow()
 {
 }

 // drawing

bool SpaceWindow::isGoodSize(Point size) const
 {
  return size/2 >= space ;
 }

void SpaceWindow::layout()
 {
  // do nothing
 }

void SpaceWindow::draw(DrawBuf buf,DrawParam) const
 {
  Pane pane(Null,getSize());

  Pane inner=pane.shrink(space);

  if( +inner )
    {
     if( drag )
       {
        MPane p(inner);

        SmoothDrawArt art(buf.cut(pane));

        FigureBox fig(p);

        fig.loop(art,HalfNeg,Fraction(+cfg.border_dxy),+cfg.border);
       }
    }
  else
    {
     SmoothDrawArt art(buf.cut(pane));

     art.erase(+cfg.border);
    }
 }

 // base

void SpaceWindow::open()
 {
  drag=false;
 }

 // keyboard

FocusType SpaceWindow::askFocus() const
 {
  return NoFocus;
 }

 // mouse

void SpaceWindow::looseCapture()
 {
  drag=false;
 }

MouseShape SpaceWindow::getMouseShape(Point,KeyMod) const
 {
  return drag?Mouse_SizeAll:Mouse_Arrow;
 }

 // user input

void SpaceWindow::react(UserAction action)
 {
  action.dispatch(*this);
 }

void SpaceWindow::react_LeftClick(Point point,MouseKey mkey)
 {
  if( mkey&MouseKey_Shift )
    {
     space=point;

     redraw();

     changed.assert();
    }
  else
    {
     startDrag(point);
    }
 }

void SpaceWindow::react_LeftUp(Point point,MouseKey)
 {
  if( drag ) endDrag(point);
 }

void SpaceWindow::react_Move(Point point,MouseKey mkey)
 {
  if( drag )
    {
     if( mkey&MouseKey_Left )
       {
        dragTo(point);
       }
     else
       {
        endDrag();
       }
    }
 }

/* class ClientWindow::TypeInfo::Base */

class ClientWindow::TypeInfo::Base : public ComboInfoBase
 {
   struct Item
    {
     ComboInfoType type;
     String name;
     SubFactory factory;

     explicit Item(const String &title)
      : type(ComboInfoTitle),
        name(title),
        factory(0)
      {
      }

     Item(const String &name_,SubFactory factory_)
      : type(ComboInfoText),
        name(name_),
        factory(factory_)
      {
      }

     operator ComboInfoItem() const
      {
       return ComboInfoItem(type,Range(name));
      }
    };

   DynArray<Item> list;

  private:

   void add(const String &title)
    {
     list.append_fill(title);
    }

   void add(const String &name,SubFactory factory)
    {
     list.append_fill(name,factory);
    }

  public:

   Base() // Update here
    {
     using namespace Subs;

     add("Decorative"_str);

       add("XSingleLine"_str,Create<XSingleLineWindow>);
       add("YSingleLine"_str,Create<YSingleLineWindow>);
       add("XDoubleLine"_str,Create<XDoubleLineWindow>);
       add("YDoubleLine"_str,Create<YDoubleLineWindow>);
       add("Contour"_str,Create<ContourWindow>);
       add("TextContour left"_str,Create<TextContourWindow_Left>);
       add("TextContour right"_str,Create<TextContourWindow_Right>);
       add("TextContour center"_str,Create<TextContourWindow_Center>);
       add("Draw"_str,Create_def<DrawWindow>);
       add("Layout"_str,CreateCombo<LayoutWindow>);

     add("Button"_str);

       add("Button"_str,CreateCombo<ButtonWindow_Button>);
       add("MoveButton Back"_str,Create_param<MoveButtonWindow,MoveButtonShape::BackDir>);
       add("MoveButton Fore"_str,Create_param<MoveButtonWindow,MoveButtonShape::ForeDir>);
       add("AllButton Plus"_str,Create_param<AllButtonWindow,AllButtonShape::AllPlus>);
       add("AllButton Minus"_str,Create_param<AllButtonWindow,AllButtonShape::AllMinus>);
       add("Knob"_str,Create<KnobWindow_Asterisk>);
       add("Knob auto"_str,Create<KnobWindow_auto>);
       add("FireButton"_str,CreateCombo<FireButtonWindow_Sample>);
       add("RunButton"_str,CreateCombo<RunButtonWindow_Sample>);

     add("Box"_str);

       add("Check"_str,Create<CheckWindow>);
       add("Switch"_str,Create<SwitchWindow>);
       add("XSwitch"_str,Create<XShiftSwitchWindow>);
       add("YSwitch"_str,Create<YShiftSwitchWindow>);
       add("RadioLight"_str,CreateCombo<RadioLightWindow>);
       add("Alt"_str,CreateCombo<AltWindow_Sample>);

     add("Line"_str);

       add("Label"_str,Create<LabelWindow_Label>);
       add("Text"_str,Create<TextWindow_SampleText>);
       add("Label align"_str,CreateCombo<AlignWindow<LabelWindow_Label> >);
       add("Text align"_str,CreateCombo<AlignWindow<TextWindow_SampleText> >);
       add("TextLine"_str,Create<TextLineWindow_SampleText>);
       add("Progress"_str,Create<ProgressWindow_Sample>);
       add("ArrowProgress"_str,Create<ArrowProgressWindow_Sample>);
       add("LineEdit"_str,Create<LineEditWindow>);
       add("XScroll"_str,Create<XScrollWindow_Sample>);
       add("YScroll"_str,Create<YScrollWindow_Sample>);
       add("Spinor"_str,Create<SpinorWindow_Sample>);
       add("XSlider"_str,CreateCombo<XSliderWindow_Sample>);
       add("YSlider"_str,CreateCombo<YSliderWindow_Sample>);

     add("Pane"_str);

       add("Info"_str,Create<InfoWindow_SampleInfo>);
       add("Message knob"_str,Create<MessageWindow_knob>);
       add("Message ok"_str,Create<MessageWindow_ok>);
       add("Message yes/no"_str,Create<MessageWindow_yes_no>);
       add("SimpleTextList"_str,Create<SimpleTextListWindow_SampleList>);

       //add(""_str,Create<>);
    }

   virtual ~Base()
    {
    }

   // methods

   SubFactory getFactory(ulen index) const
    {
     SubFactory ret=list.at(index).factory;

     if( !ret )
       {
        Printf(Exception,"App::ClientWindow::TypeInfo::Base::getFactory(#;) : title index",index);
       }

     return ret;
    }

   // AbstractComboInfo

   virtual ulen getLineCount() const
    {
     return list.getLen();
    }

   virtual ComboInfoItem getLine(ulen index) const
    {
     return list.at(index);
    }
 };

/* class ClientWindow::TypeInfo */

ClientWindow::TypeInfo::TypeInfo()
 : ComboInfo(new Base)
 {
 }

ClientWindow::TypeInfo::~TypeInfo()
 {
 }

auto ClientWindow::TypeInfo::getFactory(ulen index) const -> SubFactory
 {
  return castPtr<Base>()->getFactory(index);
 }

/* class ClientWindow */

void ClientWindow::wheat_changed(bool)
 {
  redraw();
 }

void ClientWindow::type_selected(ulen index)
 {
  if( Change(cur_index,index) )
    {
     // delete old

     cur.set(0);

     // create new

     cur.set( info.getFactory(index)(wlist,pref) );

     wlist.insTop(*cur);

     // update

     list_type.setFocus();

     layout();

     redraw();
    }
 }

void ClientWindow::space_changed()
 {
  if( +cur )
    {
     cur->setPlace(space.getInner());

     space.redraw();
    }
 }

ClientWindow::ClientWindow(SubWindowHost &host,const Config &cfg_,const UserPreference &pref_)
 : ComboWindow(host),
   cfg(cfg_),
   pref(pref_),

   check_wheat(wlist,cfg.check_cfg,false),
   label_wheat(wlist,cfg.label_cfg,"Wheat"_str),
   list_type(wlist,cfg.list_cfg,info),
   space(wlist,cfg.space_cfg),

   connector_wheat_changed(this,&ClientWindow::wheat_changed,check_wheat.changed),
   connector_type_selected(this,&ClientWindow::type_selected,list_type.selected),
   connector_space_changed(this,&ClientWindow::space_changed,space.changed)
 {
  wlist.insTop(check_wheat,label_wheat,list_type,space);

  ulen index=list_type.getSelect();

  if( Change(cur_index,index) )
    {
     // create new

     cur.set( info.getFactory(index)(wlist,pref) );

     wlist.insTop(*cur);
    }

  // bind hints

  bindHint(cfg.hint_Text);

  check_wheat.bindHint(cfg.hint_Wheat);

  list_type.bindHint(cfg.hint_List);

  space.bindHint(cfg.hint_Space);
 }

ClientWindow::~ClientWindow()
 {
 }

 // drawing

void ClientWindow::layout()
 {
  PaneCut pane(getSize(),+cfg.space_dxy);

  pane.shrink();

  // check_wheat , label_wheat , list_type

  {
   auto check__wheat=CutBox(check_wheat);
   auto label__wheat=CutPoint(label_wheat);
   auto list__type=CutPoint(list_type);

   Coord len=check__wheat.getExt()+label__wheat.getMinSize().x;

   Coord dx=Sup(list__type.getMinSize().x,len);

   PaneCut p=pane.cutLeft(dx);

   Coord dy=SupDY(check__wheat,label__wheat);

   PaneCut q=p.cutTop(dy);

   q.place_cutLeft(check__wheat).placeMin(label__wheat);

   p.place(list__type);
  }

  // space

  pane.place(space);

  // cur

  if( +cur )
    {
     cur->setPlace(space.getInner());
    }
 }

void ClientWindow::drawBack(DrawBuf buf,DrawParam &draw_param) const
 {
  draw_param.erase(buf,getBack());
 }

 // base

void ClientWindow::open()
 {
  wlist.open();
 }

} // namespace App

