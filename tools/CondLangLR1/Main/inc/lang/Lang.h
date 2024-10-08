/* Lang.h */
//----------------------------------------------------------------------------------------
//
//  Project: CondLangLR1 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2015 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CondLangLR1_lang_Lang_h
#define CondLangLR1_lang_Lang_h

#include "CondLang.h"

namespace App {

/* ApplyToDesc() */

template <CursorType R,FuncInitArgType<Meta::PtrObjType<R> *> FuncInit>
void ApplyToDesc(R range,FuncInit func_init)
 {
  FunctorTypeOf<FuncInit> func(func_init);

  for(; +range ;++range) func(range.ptr);
 }

/* classes */

struct LangBase;

struct Atom;

struct Synt;

struct Rule;

class Lang;

class BottomLang;

class TopLang;

class ExtLang;

class LangMap;

/* struct LangBase */

struct LangBase : NoCopy
 {
  // AtomDesc

  struct AtomDesc : DescBase
   {
    AtomDesc() noexcept {}

    ulen map_index = MaxULen ;

    // print object

    void print(PrinterType auto &out) const
     {
      Printf(out,"#;) #;",index,name);
     }
   };

  // SyntDesc

  struct RuleDesc;

  struct SyntDesc : DescBase
   {
    PtrLen<const RuleDesc> rules;
    bool is_lang = false ;

    ulen map_index = MaxULen ;
    ulen kind_index = 0 ;

    SyntDesc() noexcept {}

    // print object

    void print(PrinterType auto &out) const
     {
      Printf(out,"#;) #;",index,name);

      if( is_lang ) Putobj(out," !");
     }
   };

  // Element

  struct Element
   {
    AnyPtr_const<AtomDesc,SyntDesc> ptr;

    // constructors

    Element() noexcept {}

    Element(const AtomDesc *atom) : ptr(atom) {}

    Element(const SyntDesc *synt) : ptr(synt) {}

    template <IsType<Synt> T>
    Element(const T &obj) : ptr(obj.desc) {}

    // properties

    bool operator + () const { return +ptr; }

    bool operator ! () const { return !ptr; }

    // atoms and synts

    template <FuncInitArgType<Atom> AtomFuncInit,FuncInitArgType<Synt> SyntFuncInit>
    void apply(AtomFuncInit atom_func_init,SyntFuncInit synt_func_init) const
     {
      struct Func
       {
        AtomFuncInit atom_func_init;
        SyntFuncInit synt_func_init;

        Func(AtomFuncInit atom_func_init_,SyntFuncInit synt_func_init_) : atom_func_init(atom_func_init_),synt_func_init(synt_func_init_) {}

        void operator () (const AtomDesc *atom)
         {
          FunctorTypeOf<AtomFuncInit> func(atom_func_init);

          func(atom);
         }

        void operator () (const SyntDesc *synt)
         {
          FunctorTypeOf<SyntFuncInit> func(synt_func_init);

          func(synt);
         }
       };

      ptr.apply( Func(atom_func_init,synt_func_init) );
     }

    void applyForAtom(FuncInitArgType<Atom> auto func_init) const
     {
      ptr.applyFor<AtomDesc>(func_init);
     }

    void applyForSynt(FuncInitArgType<Synt> auto func_init) const
     {
      ptr.applyFor<SyntDesc>(func_init);
     }

    // print object

    void print(PrinterType auto &out) const
     {
      apply( [&] (const AtomDesc *atom) { Putobj(out,atom->name); } ,
             [&] (const SyntDesc *synt) { Putobj(out,synt->name); } );
     }
   };

  // RuleDesc

  struct RuleDesc : DescBase
   {
    const SyntDesc *ret = 0 ;
    PtrLen<const Element> args;

    ulen map_index = MaxULen ;

    RuleDesc() noexcept {}

    // print object

    void print(PrinterType auto &out) const
     {
      Printf(out,"#;) #; -> #;",index,name,ret->name);
     }
   };
 };

/* struct Atom */

struct Atom
 {
  const LangBase::AtomDesc *desc;

  // constructors

  Atom() noexcept : desc(0) {}

  Atom(const LangBase::AtomDesc *desc_) : desc(desc_) {}

  // properties

  bool operator + () const { return desc!=0; }

  bool operator ! () const { return desc==0; }

  StrLen getName() const { return desc->name; }

  ulen getIndex() const { return desc->index; }

  // cmp objects

  bool operator == (Atom obj) const noexcept { return desc==obj.desc; }

  CmpResult operator <=> (Atom obj) const noexcept { return Cmp(desc,obj.desc); }

  // print object

  void print(PrinterType auto &out) const
   {
    if( !desc )
      Putobj(out,"(Null)");
    else
      Putobj(out,desc->name);
   }
 };

/* struct Synt */

struct Synt
 {
  const LangBase::SyntDesc *desc;

  // constructors

  Synt() noexcept : desc(0) {}

  Synt(const LangBase::SyntDesc *desc_) : desc(desc_) {}

  // properties

  bool operator + () const { return desc!=0; }

  bool operator ! () const { return desc==0; }

  StrLen getName() const { return desc->name; }

  ulen getIndex() const { return desc->index; }

  ulen getMapIndex() const { return desc->map_index; }

  ulen getKindIndex() const { return desc->kind_index; }

  bool isLang() const { return desc->is_lang; }

  // rules

  template <FuncInitArgType<Rule> FuncInit>
  void apply(FuncInit func_init) const
   {
    FunctorTypeOf<FuncInit> func(func_init);

    for(auto p=desc->rules; +p ;++p) func(p.ptr);
   }

  // cmp objects

  bool operator == (Synt obj) const noexcept { return desc==obj.desc; }

  CmpResult operator <=> (Synt obj) const noexcept { return Cmp(desc,obj.desc); }
 };

/* type Element */

using Element = LangBase::Element ;

/* struct Rule */

struct Rule
 {
  const LangBase::RuleDesc *desc;

  // constructors

  Rule() noexcept : desc(0) {}

  Rule(const LangBase::RuleDesc *desc_) : desc(desc_) {}

  // properties

  bool operator + () const { return desc!=0; }

  bool operator ! () const { return desc==0; }

  StrLen getName() const { return desc->name; }

  ulen getIndex() const { return desc->index; }

  Synt getRet() const { return desc->ret; }

  // elements

  PtrLen<const Element> getArgs() const { return desc->args; }

  template <FuncInitArgType<Element> FuncInit>
  void apply(FuncInit func_init) const
   {
    FunctorTypeOf<FuncInit> func(func_init);

    for(auto p=desc->args; +p ;++p) func(*p);
   }

  template <FuncInitArgType<Atom> AtomFuncInit,FuncInitArgType<Synt> SyntFuncInit>
  void apply(AtomFuncInit atom_func_init,SyntFuncInit synt_func_init) const
   {
    for(auto p=desc->args; +p ; ++p) p->apply(atom_func_init,synt_func_init);
   }

  // cmp objects

  bool operator == (Rule obj) const noexcept { return desc==obj.desc; }

  CmpResult operator <=> (Rule obj) const noexcept { return Cmp(desc,obj.desc); }
 };

/* class Lang */

class Lang : public LangBase
 {
  protected:

   ElementPool pool;

   PtrLen<AtomDesc> atoms;
   PtrLen<SyntDesc> synts;
   PtrLen<RuleDesc> rules;

  protected:

   Lang() {}

   ~Lang() {}

   PtrLen<AtomDesc> createAtoms(ulen len)
    {
     PtrLen<AtomDesc> ret=pool.createArray<AtomDesc>(len);

     atoms=ret;

     return ret;
    }

   PtrLen<SyntDesc> createSynts(ulen len)
    {
     PtrLen<SyntDesc> ret=pool.createArray<SyntDesc>(len);

     synts=ret;

     return ret;
    }

   PtrLen<RuleDesc> createRules(ulen len)
    {
     PtrLen<RuleDesc> ret=pool.createArray<RuleDesc>(len);

     rules=ret;

     return ret;
    }

   PtrLen<Element> createElements(RuleDesc &rule,ulen len)
    {
     PtrLen<Element> ret=pool.createArray<Element>(len);

     rule.args=Range_const(ret);

     return ret;
    }

  public:

   // description

   PtrLen<const AtomDesc> getAtoms() const { return Range_const(atoms); }

   ulen getAtomCount() const { return atoms.len; }

   PtrLen<const SyntDesc> getSynts() const { return Range_const(synts); }

   ulen getSyntCount() const { return synts.len; }

   PtrLen<const RuleDesc> getRules() const { return Range_const(rules); }

   ulen getRuleCount() const { return rules.len; }

   // apply

   void applyForAtoms(FuncInitArgType<Atom> auto func_init) const
    {
     ApplyToDesc(getAtoms(),func_init);
    }

   void applyForAtoms(ulen atom_count,FuncInitArgType<Atom> auto func_init) const
    {
     ApplyToDesc(getAtoms().prefix(atom_count),func_init);
    }

   void applyForSynts(FuncInitArgType<Synt> auto func_init) const
    {
     ApplyToDesc(getSynts(),func_init);
    }

   void applyForRules(FuncInitArgType<Rule> auto func_init) const
    {
     ApplyToDesc(getRules(),func_init);
    }

   // print object

   struct PrintOptType;

   void print(PrinterType auto &out,PrintOptType opt) const;
 };

/* class BottomLang */

class BottomLang : public Lang
 {
  public:

   explicit BottomLang(const CondLang &clang);

   ~BottomLang();
 };

/* class TopLang */

class TopLang : public Lang
 {
  private:

   struct ElementRec
    {
     CondLangBase::Element element;
     ulen element_index;
     ulen kind_index;
    };

   struct ElementRecExt : ElementRec
    {
     ulen kind_lim;

     explicit ElementRecExt(CondLangBase::Element element_)
      {
       element=element_;
       element_index=0; // set later
       kind_index=0;

       element_.apply( [this] (const CondLangBase::AtomDesc *) { kind_lim=1; } ,
                       [this] (const CondLangBase::SyntDesc *synt) { kind_lim=Max<ulen>(synt->kinds.len,1); } );
      }

     bool inc()
      {
       if( ++kind_index>=kind_lim )
         {
          kind_index=0;

          return false;
         }

       return true;
      }

     StrLen getExtraName() const
      {
       if( kind_lim==1 ) return StrLen();

       StrLen ret;

       element.applyForSynt( [&] (const CondLangBase::SyntDesc *synt) { ret=synt->kinds[kind_index].name; } );

       return ret;
      }
    };

   struct RuleRec
    {
     StrLen name;
     ulen map_index;
     DynArray<ElementRec> args;

     RuleRec(StrLen name_,ulen map_index_,PtrLen<const ElementRecExt> args_)
      : name(name_),map_index(map_index_),args(DoCast(args_.len),args_.ptr) {}

     // swap/move objects

     void objSwap(RuleRec &obj)
      {
       Swap(name,obj.name);
       Swap(map_index,obj.map_index);
       Swap(args,obj.args);
      }

     explicit RuleRec(ToMoveCtor<RuleRec> obj)
      : name(obj->name),
        map_index(obj->map_index),
        args(ToMoveCtor(obj->args))
      {
      }
    };

   static bool Next(PtrLen<ElementRecExt> range);

   struct EvalCondArg;
   struct TestCondArg;

   static bool TestCond(PtrLen<const ElementRecExt> args,CondLangBase::Cond cond);

   ulen makeRules(Collector<RuleRec> &collector,const CondLangBase::RuleDesc &rule);

   ulen makeRules(Collector<RuleRec> &collector,const CondLangBase::SyntDesc &synt);

   ulen makeRules(Collector<RuleRec> &collector,const CondLangBase::SyntDesc &synt,ulen kind_index);

   StrLen makeRuleName(StrLen name,PtrLen<const ElementRecExt> args);

  public:

   explicit TopLang(const CondLang &clang);

   ~TopLang();
 };

/* class ExtLang */

class ExtLang : public Lang
 {
   ulen original_atom_count;

  private:

   void build(const Lang &lang,ulen map_atom_count);

  public:

   explicit ExtLang(const Lang &lang) { build(lang,0); }

   explicit ExtLang(const TopLang &lang) { build(lang,lang.getAtomCount()); }

   ~ExtLang();

   // description

   PtrLen<const AtomDesc> getOriginalAtoms() const { return getAtoms().prefix(original_atom_count); }

   ulen getOriginalAtomCount() const { return original_atom_count; }

   PtrLen<const AtomDesc> getRuleAtoms() const { return getAtoms().part(original_atom_count); }

   ulen getRuleAtomCount() const { return getAtomCount()-original_atom_count; }

   // apply

   void applyForOriginalAtoms(FuncInitArgType<Atom> auto func_init) const
    {
     ApplyToDesc(getOriginalAtoms(),func_init);
    }

   void applyForRuleAtoms(FuncInitArgType<Atom> auto func_init) const
    {
     ApplyToDesc(getRuleAtoms(),func_init);
    }
 };

/* class LangMap */

class LangMap
 {
   const LangBase::AtomDesc *atoms;
   const LangBase::SyntDesc *synts;
   const LangBase::RuleDesc *rules;

  public:

   LangMap() : atoms(0),synts(0),rules(0) {}

   explicit LangMap(const Lang &lang)
    {
     atoms=lang.getAtoms().ptr;
     synts=lang.getSynts().ptr;
     rules=lang.getRules().ptr;
    }

   bool operator + () const { return atoms; }

   bool operator ! () const { return !atoms; }

   Atom operator () (Atom atom) const { return atoms+atom.desc->map_index; }

   Synt operator () (Synt synt) const { return synts+synt.desc->map_index; }

   Rule operator () (Rule rule) const { return rules+rule.desc->map_index; }

   const LangBase::AtomDesc & operator () (const LangBase::AtomDesc &atom) const { return atoms[atom.map_index]; }

   const LangBase::SyntDesc & operator () (const LangBase::SyntDesc &synt) const { return synts[synt.map_index]; }

   const LangBase::RuleDesc & operator () (const LangBase::RuleDesc &rule) const { return rules[rule.map_index]; }
 };

/* Lang::print() */

struct Lang::PrintOptType
 {
  LangMap map;

  PrintOptType() {}

  PrintOptType(const char *,const char *) {}

  PrintOptType(const LangMap &map_) : map(map_) {}
 };

void Lang::print(PrinterType auto &out,PrintOptType opt) const
 {
  Printf(out,"#;\n\n",Title("Atoms"));

  if( +opt.map )
    {
     for(auto &atom : getAtoms() ) Printf(out,"#;  -->  #;\n",atom,opt.map(atom));

     Printf(out,"\n#;\n\n",Title("Syntax classes"));

     for(auto &synt : getSynts() )
       {
        Printf(out,"#;  -->  #;\n",synt,opt.map(synt));

        for(auto &rule : synt.rules ) Printf(out,"  Rule(#;)\n",rule.index);
       }

     Printf(out,"\n#;\n\n",Title("Rules"));

     for(auto &rule : getRules() )
       {
        Printf(out,"#;  -->  #;\n",rule,opt.map(rule));

        for(auto &element : rule.args ) Printf(out,"  #;",element);

        Putch(out,'\n');
       }
    }
  else
    {
     for(auto &atom : getAtoms() ) Printf(out,"#;\n",atom);

     Printf(out,"\n#;\n\n",Title("Syntax classes"));

     for(auto &synt : getSynts() )
       {
        Printf(out,"#;\n",synt);

        for(auto &rule : synt.rules ) Printf(out,"  Rule(#;)\n",rule.index);
       }

     Printf(out,"\n#;\n\n",Title("Rules"));

     for(auto &rule : getRules() )
       {
        Printf(out,"#;\n",rule);

        for(auto &element : rule.args ) Printf(out,"  #;",element);

        Putch(out,'\n');
       }
    }

  Printf(out,"\n#;\n",TextDivider());
 }

} // namespace App

#endif



