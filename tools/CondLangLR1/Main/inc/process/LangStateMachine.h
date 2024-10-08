/* LangStateMachine.h */
//----------------------------------------------------------------------------------------
//
//  Project: CondLangLR1 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CondLangLR1_process_LangStateMachine_h
#define CondLangLR1_process_LangStateMachine_h

#include "inc/lang/Lang.h"
#include "LangEstimate.h"
#include "LangDiagram.h"
#include "inc/SparseMatrix.h"

#include <CCore/inc/List.h>
#include <CCore/inc/Tree.h>
#include <CCore/inc/NodeAllocator.h>

namespace App {

/* consts */

inline constexpr ulen StateCap = 1'000'000 ;

/* functions */

void GuardStateCapReached();

inline void GuardStateCap(ulen count)
 {
  if( count>=StateCap ) GuardStateCapReached();
 }

/* classes */

struct LangOpt;

template <class Estimate> struct StateMachineDesc;

template <class Estimate,class Context=EmptyContext> class LangStateMachine;

/* struct LangOpt */

struct LangOpt
 {
  const Lang &lang;

  LangOpt(const Lang &lang_) : lang(lang_) {}
 };

/* struct StateMachineDesc<Estimate> */

template <class Estimate>
struct StateMachineDesc : NoCopy
 {
  ulen index;
  Estimate est;
  PtrLen<const StateMachineDesc *> transitions;

  // print object

  void print(PrinterType auto &out) const
   {
    Printf(out,"#;) #;\n",index,est);

    for(auto *dst : transitions ) Printf(out,"  #3;",dst->index);

    Putch(out,'\n');
   }
 };

/* class LangStateMachine<Estimate,Context> */

template <class Estimate,class Context>
class LangStateMachine : NoCopy
 {
   using Matrix = SparseMatrix<Vertex,Estimate> ;

   using Vector = SparseVector<Vertex,Estimate> ;

   using Position = MatrixPosition<Vertex,Estimate> ;

   struct ArrowRec : NoCopy
    {
     ArrowRec *next = 0 ;

     Position pos;

     template <class E>
     ArrowRec(const Arrow &arrow,const E &estimate) : pos(arrow.dst,arrow.src,estimate(arrow.beta)) {}
    };

   struct Head : NoCopy
    {
     ArrowRec *top = 0 ;
     ulen count = 0 ;

     Head() noexcept {}

     void ins(ArrowRec *ptr)
      {
       ptr->next=top;
       top=ptr;
       count++;
      }

     Matrix build()
      {
       typename Matrix::PosFill fill(count);

       for(ArrowRec *ptr=top; ptr ;ptr=ptr->next) fill.add(ptr->pos);

       Matrix ret(fill);

       return ret;
      }
    };

   struct State : NoCopy
    {
     // links

     SLink<State> list_link;
     RBTreeLink<State,Vector> tree_link;

     // data

     ulen index;
     Estimate est;
     DynArray<State *> transitions;

     // methods

     State(ulen index_,ulen transition_count) : index(index_),transitions(transition_count) {}

     void complete(Vertex stop)
      {
       est=tree_link.key[stop];
      }

     const Vector & getVector() const { return tree_link.key; }
    };

   using ListAlgo = typename SLink<State>::template LinearAlgo<&State::list_link> ;
   using TreeAlgo = typename RBTreeLink<State,Vector>::template Algo<&State::tree_link,const Vector &> ;

   using StateCur = typename ListAlgo::Cur ;

   class StateStorage : NoCopy
    {
      typename ListAlgo::FirstLast list;
      typename TreeAlgo::Root root;

      NodePoolAllocator<State> allocator;

      ulen transition_count;
      Vertex stop;

     public:

      StateStorage(ulen transition_count_,Vertex stop_)
       : allocator(1000),
         transition_count(transition_count_),
         stop(stop_)
       {
       }

      ~StateStorage()
       {
        while( State *state=list.del_first() ) allocator.free_nonnull(state);
       }

      State * find_or_add(const Vector &vstate)
       {
        typename TreeAlgo::PrepareIns prepare(root,vstate);

        if( prepare.found ) return prepare.found;

        ulen index=allocator.getCount();

        GuardStateCap(index);

        State *ret=allocator.alloc(index,transition_count);

        prepare.complete(ret);

        list.ins_last(ret);

        ret->complete(stop);

        return ret;
       }

      ulen getCount() const { return allocator.getCount(); }

      StateCur getStart() const { return list.start(); }
    };

  public:

   using StateDesc = StateMachineDesc<Estimate> ;

  private:

   SimpleArray<StateDesc> state_table;
   DynArray<const StateDesc *> transition_buf;

   ulen atom_count;
   ulen element_count;

  private:

   template <class ... SS>
   void build(const Lang &lang,const LangDiagram &diagram,ulen atom_count,SS && ... ss);

  public:

   // constructors

   template <class ... SS>
   LangStateMachine(const Lang &lang,const LangDiagram &diagram,SS && ... ss)
    {
     build(lang,diagram,lang.getAtomCount(), std::forward<SS>(ss)... );
    }

   template <class ... SS>
   LangStateMachine(const ExtLang &lang,const LangDiagram &diagram,SS && ... ss)
    {
     build(lang,diagram,lang.getOriginalAtomCount(), std::forward<SS>(ss)... );
    }

   ~LangStateMachine() {}

   // description

   PtrLen<const StateDesc> getStateTable() const { return Range_const(state_table); }

   ulen getStateCount() const { return state_table.getLen(); }

   ulen getAtomCount() const { return atom_count; }

   ulen getElementCount() const { return element_count; }

   // print object

   using PrintOptType = LangOpt ;

   void print(PrinterType auto &out,LangOpt opt) const
    {
     Putobj(out,"-----\n");

     Printf(out,"atoms = #; elements = #;\n",getAtomCount(),getElementCount());

     opt.lang.applyForAtoms(getAtomCount(), [&] (Atom atom) { Printf(out,"#; ",atom.getName()); } );

     opt.lang.applyForSynts( [&] (Synt synt) { Printf(out,"#; ",synt.getName()); } );

     Putobj(out,"\n-----\n");

     for(auto &state : getStateTable() )
       {
        Putobj(out,state,"-----\n");
       }
    }
 };

template <class Estimate,class Context>
template <class ... SS>
void LangStateMachine<Estimate,Context>::build(const Lang &lang,const LangDiagram &diagram,ulen atom_count,SS && ... ss)
 {
  //--------------------------------------------------------------------------

  TrackStage("Estimate lang");

  LangEstimate<Estimate,Context> estimate(lang, std::forward<SS>(ss)... );

  //--------------------------------------------------------------------------

  TrackStage("Build matrix");

  ulen count=LenAdd(atom_count,lang.getSyntCount());

  this->atom_count=atom_count;
  this->element_count=count;

  Matrix N;

  DynArray<Matrix> T(count);

  {
   Head headN;

   SimpleArray<Head> headT(count);

   auto arrows=diagram.getArrows();

   DynArray<ArrowRec,ArrayAlgo_mini<ArrowRec> > buf(DoReserve,arrows.len);

   for(auto &arrow : arrows )
     {
      ArrowRec *ptr=buf.append_fill(arrow,estimate);

      if( !arrow.alpha )
        {
         headN.ins(ptr);
        }
      else
        {
         arrow.alpha.apply( [&] (Atom atom) {
                                             ulen index=atom.getIndex();

                                             if( index<atom_count ) headT[index].ins(ptr);
                                            } ,

                            [&] (Synt synt) {
                                             headT[synt.getIndex()+atom_count].ins(ptr);
                                            }
                          );
        }
     }

   N=headN.build();

   for(ulen i=0; i<count ;i++) T[i]=headT[i].build();
  }

  //--------------------------------------------------------------------------

  TrackStage("Calculate matrix");

  Matrix F=N;

  {
   TrackStep track;

   while( SetCmp(F,F+F*F) ) track.step();

   track.finish();
  }

  {
   for(Matrix &X : T ) X=X+F*X;
  }

  Vector S;

  {
   auto range=diagram.getStart();

   DynArray<typename Vector::Cell> buf(DoReserve,range.len);

   for(; +range ;++range) buf.append_fill(*range,Estimate(ElementOne));

   S=Vector(Range_const(buf));
  }

  {
   S=S+F*S;
  }

  //--------------------------------------------------------------------------

  TrackStage("Calculate states");

  StateStorage storage(count,diagram.getStop());

  {
   TrackStep track;

   for(StateCur cur(storage.find_or_add(S)); +cur ;++cur)
     {
      const Vector &V=cur->getVector();

      for(ulen i=0; i<count ;i++)
        {
         State *dst=storage.find_or_add(T[i]*V);

         cur->transitions[i]=dst;
        }

      track.step();
     }

   track.finish();
  }

  //--------------------------------------------------------------------------

  {
   ulen state_count=storage.getCount();

   SimpleArray<StateDesc> state_table(state_count);

   auto tbuf=transition_buf.extend_raw(LenOf(count,state_count));

   for(StateCur cur(storage.getStart()); +cur ;++cur)
     {
      ulen index=cur->index;

      auto &desc=state_table[index];

      desc.index=index;
      desc.est=cur->est;
      desc.transitions=tbuf.part(index*count,count);

      for(ulen i=0; i<count ;i++)
        {
         ulen dst=cur->transitions[i]->index;

         desc.transitions[i]=state_table.getPtr()+dst;
        }
     }

   Swap(state_table,this->state_table);
  }
 }

} // namespace App

#endif

