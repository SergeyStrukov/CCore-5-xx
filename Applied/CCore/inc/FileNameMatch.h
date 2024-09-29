/* FileNameMatch.h */
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

#ifndef CCore_inc_FileNameMatch_h
#define CCore_inc_FileNameMatch_h

#include <CCore/inc/Array.h>
#include <CCore/inc/Symbol.h>
#include <CCore/inc/FunctorType.h>
#include <CCore/inc/algon/ApplyToRange.h>
#include <CCore/inc/OptMember.h>

namespace CCore {

/* namespace FileNameMatch */

namespace FileNameMatch {

/* type Filter */

using Filter = PtrLen<const Char> ;

/* functions */

inline bool IsFinal(Filter s)
 {
  for(Char ch : s ) if( ch!='*' ) return false;

  return true;
 }

void Add(Filter s,FuncArgType<ulen> auto func)
 {
  for(; +s ;++s)
    {
     switch( *s )
       {
        case '*' :
         {
          func(s.len);
         }
        break;

        case '?' :
         {
          func(s.len-1);
         }
        return;

        default: return;
       }
    }
 }

void Add(Filter s,Char ch_,FuncArgType<ulen> auto func)
 {
  for(; +s ;++s)
    {
     switch( Char ch=*s )
       {
        case '*' :
         {
          func(s.len);
         }
        break;

        case '?' :
         {
          func(s.len-1);
         }
        return;

        default:
         {
          if( ch==ch_ ) func(s.len-1);
         }
        return;
       }
    }
 }

template <FuncInitArgType<Filter> FuncInit>
void Suffixes(PtrLen<const ulen> len_list,Filter filter,FuncInit func_init)
 {
  FunctorTypeOf<FuncInit> func(func_init);

  Algon::ApplyToRange(len_list, [filter,&func] (ulen slen) { return func(filter.suffix(slen)); } );
 }

/* struct TestFinal */

struct TestFinal
 {
  bool ret=false;

  operator bool() const { return ret; }

  bool operator () (Filter filter)
   {
    if( IsFinal(filter) )
      {
       ret=true;

       return false;
      }

    return true;
   }
 };

} // namespace FileNameMatch

/* classes */

class SlowFileNameFilter;

class FileNameFilter;

class ComboFileNameFilter;

/* class SlowFileNameFilter */

class SlowFileNameFilter : NoCopy
 {
   DynArray<Char> filter;

   mutable DynArray<bool> flags;
   mutable DynArray<ulen> list;

   bool ok = false ;

  private:

   using Filter = PtrLen<const Char> ;

   void prepare(StrLen filter);

   void suffixes(ulen count,FuncInitArgType<Filter> auto func_init) const;

   void add(Filter s,Char ch) const;

   ulen complete() const;

  public:

   SlowFileNameFilter() noexcept;

   explicit SlowFileNameFilter(StrLen filter);

   ~SlowFileNameFilter();

   // props

   bool operator + () const { return ok; }

   bool operator ! () const { return !ok; }

   // methods

   void reset();

   void reset(StrLen filter);

   bool operator () (StrLen file) const;
 };

/* class FileNameFilter */

class FileNameFilter : NoCopy
 {
   struct StatePtr;

   class State;

   struct StateArrow;

   class FullState;

   struct StateCouple;

   class StateMap;

  private:

   class IndState
    {
      struct Arrow
       {
        Char ch;
        ulen index;

        explicit Arrow(const StateArrow &obj);

        operator Char() const { return ch; }
       };

      DynArray<Arrow> arrows;
      ulen other_index = MaxULen ;
      bool is_final;

     public:

      explicit IndState(StateCouple couple);

      // std move

      IndState(IndState &&) = default ;

      IndState & operator = (IndState &&) = default ;

      // methods

      bool isFinal() const { return is_final; }

      ulen next(Char ch) const;
    };

   DynArray<IndState> states;

  private:

   void build(StrLen filter,ulen max_states);

  public:

   FileNameFilter() noexcept;

   explicit FileNameFilter(StrLen filter,ulen max_states=MaxULen);

   ~FileNameFilter();

   // props

   bool operator + () const { return states.getLen()!=0; }

   bool operator ! () const { return states.getLen()==0; }

   // methods

   void reset();

   void reset(StrLen filter,ulen max_states=MaxULen);

   bool operator () (StrLen file) const;
 };

/* class ComboFileNameFilter */

class ComboFileNameFilter : NoCopy
 {
   OptMember<FileNameFilter> fast;
   OptMember<SlowFileNameFilter> slow;
   bool ok = false ;

  private:

   void prepare(StrLen filter,ulen max_states);

  public:

   static constexpr ulen DefaultMaxStates = 100'000 ;

   ComboFileNameFilter() noexcept;

   explicit ComboFileNameFilter(StrLen filter,ulen max_states=DefaultMaxStates);

   ~ComboFileNameFilter();

   // props

   bool operator + () const { return ok; }

   bool operator ! () const { return !ok; }

   // methods

   void reset();

   void reset(StrLen filter,ulen max_states=DefaultMaxStates);

   bool operator () (StrLen file) const;
 };

} // namespace CCore

#endif

