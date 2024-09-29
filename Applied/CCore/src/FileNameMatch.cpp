/* FileNameMatch.cpp */
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

#include <CCore/inc/FileNameMatch.h>

#include <CCore/inc/Cmp.h>
#include <CCore/inc/TreeMap.h>
#include <CCore/inc/algon/SortUnique.h>
#include <CCore/inc/algon/UniqueToFront.h>
#include <CCore/inc/algon/BinarySearch.h>
#include <CCore/inc/StrToChar.h>

#include <CCore/inc/Exception.h>

namespace CCore {

using namespace FileNameMatch;

/* class SlowFileNameFilter */

void SlowFileNameFilter::prepare(StrLen text)
 {
  filter.reserve(text.len);

  while( +text )
    {
     Char ch=CutChar_guarded(text);

     filter.append_copy(ch);
    }

  filter.shrink_extra();

  ulen len=LenAdd(filter.getLen(),1);

  flags.extend_default(len);
  list.extend_default(len);

  ok=true;
 }

void SlowFileNameFilter::suffixes(ulen count,FuncInitArgType<Filter> auto func_init) const
 {
  Suffixes(Range(list.getPtr(),count),Range(filter),func_init);
 }

void SlowFileNameFilter::add(Filter s,Char ch) const
 {
  Add(s,ch, [this] (ulen len) { flags[len]=true; } );
 }

ulen SlowFileNameFilter::complete() const
 {
  auto base=list.getPtr();

  ulen count=0;

  for(ulen i=0,lim=list.getLen(); i<lim ;i++) if( flags[i] ) base[count++]=i;

  return count;
 }

SlowFileNameFilter::SlowFileNameFilter() noexcept
 {
 }

SlowFileNameFilter::SlowFileNameFilter(StrLen filter)
 {
  prepare(filter);
 }

SlowFileNameFilter::~SlowFileNameFilter() {}

 // methods

void SlowFileNameFilter::reset()
 {
  filter.erase();
  flags.erase();
  list.erase();

  ok=false;
 }

void SlowFileNameFilter::reset(StrLen filter)
 {
  reset();
  prepare(filter);
 }

bool SlowFileNameFilter::operator () (StrLen file) const
 {
  if( !ok ) return false;

  ulen len=filter.getLen();

  // initial state

  list[0]=len;

  ulen count=1;

  // state machine

  while( +file )
    {
     Char ch=CutChar_guarded(file);

     for(bool &flag : flags ) flag=false;

     suffixes(count, [ch,this] (Filter s) { add(s,ch); } );

     count=complete();

     if( !count ) return false;
    }

  // final

  TestFinal test;

  suffixes(count,FunctorRef(test));

  return test;
 }

/* struct FileNameFilter::StatePtr */

struct FileNameFilter::StatePtr
 {
  State *state;
 };

/* class FileNameFilter::State */

class FileNameFilter::State
 {
   Filter filter;
   DynArray<ulen> list;

  private:

   // all suffixes

   void suffixes(FuncInitArgType<Filter> auto func_init) const
    {
     Suffixes(Range(list),filter,func_init);
    }

   // follow states

   static ulen Normalize(PtrLen<ulen> list)
    {
     Sort(list);

     auto tail=Algon::CopyUniqueToFront(list);

     return tail.len;
    }

   void complete()
    {
     ulen delta=Normalize(Range(list));

     list.shrink(delta);

     list.shrink_extra();
    }

   void add(Filter s)
    {
     Add(s, [this] (ulen len) { list.append_copy(len); } );
    }

   explicit State(const State *obj)
    : filter(obj->filter)
    {
     if( obj->list.notEmpty() )
       {
        list.reserve(2*filter.len);

        obj->suffixes( [this] (Filter s) { add(s); } );

        complete();
       }
    }

   void add(Filter s,Char ch)
    {
     Add(s,ch, [this] (ulen len) { list.append_copy(len); } );
    }

   State(const State *obj,Char ch)
    : filter(obj->filter)
    {
     list.reserve(2*filter.len);

     obj->suffixes( [this,ch] (Filter s) { add(s,ch); } );

     complete();
    }

  public:

   explicit State(Filter filter_)
    : filter(filter_),
      list(DoReserve,1)
    {
     list.append_copy(filter_.len);
    }

   explicit State(StatePtr sptr) : State(std::move(*sptr.state)) {}

   ~State()
    {
    }

   // std move

   State(State &&obj) = default ;

   State & operator = (State &&obj) = default ;

   // methods

   bool isFinal() const
    {
     TestFinal test;

     suffixes(FunctorRef(test));

     return test;
    }

   void follow(Char buf[],FuncArgType<Char,State &&> auto char_func,FuncArgType<State &&> auto other_func) const // buf.len == filter.len
    {
     bool other=false;

     Char *temp=buf;

     suffixes( [&other,&temp] (Filter s)
                              {
                               if( +s )
                                 {
                                  Char ch=*s;

                                  if( ch=='*' || ch=='?' )
                                    {
                                     if( ch=='*' )
                                       {
                                        for(++s; +s && *s=='*' ;++s);

                                        if( +s )
                                          {
                                           Char ch=*s;

                                           if( ch!='?' ) *(temp++)=ch;
                                          }
                                       }

                                     other=true;
                                    }
                                  else
                                    {
                                     *(temp++)=ch;
                                    }
                                 }

                              } );

     Algon::SortThenApplyUnique(Range(buf,temp), [this,char_func] (Char ch)
                                                                  {
                                                                   char_func(ch,State(this,ch));

                                                                  } );

     if( other )
       {
        other_func(State(this));
       }
    }

   // Cmp()

   friend CmpResult Cmp(StatePtr a,const State &b)
    {
     return *a.state <=> b ;
    }

   // cmp objects

   CmpResult operator <=> (const State &obj) const
    {
     return RangeCmpOf(list,obj.list);
    }
 };

/* struct FileNameFilter::StateArrow */

struct FileNameFilter::StateArrow
 {
  Char ch;
  FullState *state;

  StateArrow(Char ch_,FullState *state_) : ch(ch_),state(state_) {}
 };

/* class FileNameFilter::FullState */

class FileNameFilter::FullState : NoCopy
 {
  private:

   ulen index = 0 ;
   DynArray<StateArrow> arrows;
   FullState *other = 0 ;

  public:

   FullState() {}

   ~FullState() {}

   // get

   ulen getIndex() const { return index; }

   const FullState * getOther() const { return other; }

   PtrLen<const StateArrow> getArrows() const { return Range(arrows); }

   // set

   void setIndex(ulen index_) { index=index_; }

   void setOther(FullState *other_) { other=other_; }

   void addArrow(Char ch,FullState *state) { arrows.append_fill(ch,state); }
 };

/* struct StateCouple */

struct FileNameFilter::StateCouple
 {
  const State *state;
  FullState *full;
 };

/* class FileNameFilter::StateMap */

class FileNameFilter::StateMap : NoCopy
 {
   RBTreeMap<State,FullState,StatePtr,NodePoolAllocator> map;

   ulen max_states;

  public:

   explicit StateMap(ulen max_states_)
    : max_states(max_states_)
    {
    }

   ~StateMap()
    {
    }

   struct Result
    {
     StateCouple couple;
     bool new_flag;

     operator StateCouple() const { return couple; }
    };

   Result find_or_add(State &&state)
    {
     if( map.getCount()>=max_states )
       {
        Printf(Exception,"CCore::FileNameFilter::StateMap::find_or_add(...) : too many states");
       }

     auto result=map.find_or_add(StatePtr{&state});

     return {{result.key,result.obj},result.new_flag};
    }
 };

/* class FileNameFilter::IndState::Arrow */

FileNameFilter::IndState::Arrow::Arrow(const StateArrow &obj)
 {
  ch=obj.ch;
  index=obj.state->getIndex();
 }

/* class FileNameFilter::IndState */

FileNameFilter::IndState::IndState(StateCouple couple)
 {
  if( const FullState *other=couple.full->getOther() ) other_index=other->getIndex();

  arrows.extend_cast(couple.full->getArrows());

  is_final=couple.state->isFinal();

  Sort(Range(arrows));
 }

 // methods

ulen FileNameFilter::IndState::next(Char ch) const
 {
  auto r=Range(arrows);

  Algon::BinarySearch_greater_or_equal(r,ch);

  if( +r && (*r).ch==ch ) return (*r).index;

  return other_index;
 }

/* class FileNameFilter */

void FileNameFilter::build(StrLen filter_,ulen max_states)
 {
  StateMap map(max_states);
  DynArray<StateCouple> list;

  StrToChar filter(filter_);

  {
   State first(filter);

   list.append_copy(map.find_or_add(std::move(first)));
  }

  TempArray<Char,1024> temp((+filter).len);

  for(ulen index=0; index<list.getLen() ;index++)
    {
     StateCouple couple=list[index];

     couple.full->setIndex(index);

     auto char_func = [&map,&list,couple] (Char ch,State &&next_state)
                                          {
                                           auto result=map.find_or_add(std::move(next_state));

                                           if( result.new_flag ) list.append_copy(result);

                                           couple.full->addArrow(ch,result.couple.full);
                                          } ;

     auto other_func = [&map,&list,couple] (State &&next_state)
                                           {
                                            auto result=map.find_or_add(std::move(next_state));

                                            if( result.new_flag ) list.append_copy(result);

                                            couple.full->setOther(result.couple.full);
                                           } ;

     couple.state->follow(temp.getPtr(),char_func,other_func);
    }

  states.extend_cast(Range_const(list));
 }

FileNameFilter::FileNameFilter() noexcept
 {
 }

FileNameFilter::FileNameFilter(StrLen filter,ulen max_states)
 {
  build(filter,max_states);
 }

FileNameFilter::~FileNameFilter()
 {
 }

void FileNameFilter::reset()
 {
  states.erase();
 }

void FileNameFilter::reset(StrLen filter,ulen max_states)
 {
  states.erase();

  build(filter,max_states);
 }

bool FileNameFilter::operator () (StrLen file) const
 {
  if( !states.getLen() ) return false;

  auto *ptr=states.getPtr();

  ulen index=0;

  while( +file )
    {
     Char ch=CutChar(file);

     ulen next=ptr[index].next(ch);

     if( next==MaxULen ) return false;

     index=next;
    }

  return ptr[index].isFinal();
 }

/* class ComboFileNameFilter */

void ComboFileNameFilter::prepare(StrLen filter,ulen max_states)
 {
  try
    {
     SilentReportException report;

     fast.create(filter,max_states);
    }
  catch(CatchType)
    {
     slow.create(filter);
    }

  ok=true;
 }

ComboFileNameFilter::ComboFileNameFilter() noexcept
 {
 }

ComboFileNameFilter::ComboFileNameFilter(StrLen filter,ulen max_states)
 {
  prepare(filter,max_states);
 }

ComboFileNameFilter::~ComboFileNameFilter()
 {
 }

 // methods

void ComboFileNameFilter::reset()
 {
  if( ok )
    {
     fast.destroy();
     slow.destroy();

     ok=false;
    }
 }

void ComboFileNameFilter::reset(StrLen filter,ulen max_states)
 {
  reset();

  prepare(filter,max_states);
 }

bool ComboFileNameFilter::operator () (StrLen file) const
 {
  if( ok )
    {
     if( +fast ) return (*fast)(file);

     return (*slow)(file);
    }

  return false;
 }

} // namespace CCore

