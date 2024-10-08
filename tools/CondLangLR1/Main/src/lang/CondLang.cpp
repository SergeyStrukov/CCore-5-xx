/* CondLang.cpp */
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

#include "inc/lang/CondLang.h"
#include "inc/lang/LangParser.h"

#include <CCore/inc/FileToMem.h>
#include <CCore/inc/Exception.h>
#include <CCore/inc/List.h>
#include <CCore/inc/algon/SortUnique.h>
#include <CCore/inc/String.h>

namespace App {

/* class CondLang::Builder */

class CondLang::Builder : NoCopy
 {
   template <class T>
   class ObjList : NoCopy
    {
      using Algo = typename SLink<T>::template LinearAlgo<&T::link> ;

      typename Algo::FirstLast list;
      ulen count = 0 ;

     public:

      ObjList() {}

      ulen operator + () const { return count; }

      bool operator ! () const { return !count; }

      ulen getCount() const { return count; }

      void add(T *obj) { list.ins_last(obj); count++; }

      T * del()
       {
        if( T *ret=list.del_first() )
          {
           count--;

           return ret;
          }

        return 0;
       }

      template <FuncInitArgType<T &> FuncInit>
      void apply(FuncInit func_init) const
       {
        FunctorTypeOf<FuncInit> func(func_init);

        auto cur=list.start();

        for(ulen cnt=count; cnt ;cnt--)
          {
           func(*cur);

           ++cur;
          }
       }

      template <FuncInitArgType<T &> FuncInit,FuncInitArgType<T &> EndFuncInit>
      void apply(FuncInit func_init,EndFuncInit end_func_init) const
       {
        FunctorTypeOf<FuncInit> func(func_init);
        FunctorTypeOf<EndFuncInit> end_func(end_func_init);

        if( !count ) return;

        auto cur=list.start();

        for(ulen cnt=count-1; cnt ;cnt--)
          {
           func(*cur);

           ++cur;
          }

        end_func(*cur);
       }
    };

   // Build...

   struct BuildKind;
   struct BuildElement;

   struct BuildCondArg : NoCopy , PosStr
    {
     SLink<BuildCondArg> link;

     BuildElement *element = 0 ;
     BuildKind *kind = 0 ;
     BuildCondArg *peer = 0 ;

     explicit BuildCondArg(PosStr postr_) : PosStr(postr_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Putobj(out,str);
      }
    };

   struct BuildCondAND;
   struct BuildCondOR;
   struct BuildCondNOT;
   struct BuildCondEQ;
   struct BuildCondNE;
   struct BuildCondGT;
   struct BuildCondGE;
   struct BuildCondLT;
   struct BuildCondLE;

  public:

   using BuildCond = AnyPtr<BuildCondAND,
                            BuildCondOR,
                            BuildCondNOT,
                            BuildCondEQ,
                            BuildCondNE,
                            BuildCondGT,
                            BuildCondGE,
                            BuildCondLT,
                            BuildCondLE> ;

  private:

   struct BuildCondAND : NoCopy
    {
     BuildCond a;
     BuildCond b;

     BuildCondAND(BuildCond a_,BuildCond b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"( #; & #; )",a,b);
      }
    };

   struct BuildCondOR : NoCopy
    {
     BuildCond a;
     BuildCond b;

     BuildCondOR(BuildCond a_,BuildCond b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"( #; | #; )",a,b);
      }
    };

   struct BuildCondNOT : NoCopy
    {
     BuildCond a;

     explicit BuildCondNOT(BuildCond a_) : a(a_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"! #;",a);
      }
    };

   struct BuildCondEQ : NoCopy
    {
     BuildCondArg a;
     BuildCondArg b;

     BuildCondEQ(PosStr a_,PosStr b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"#; == #;",a,b);
      }
    };

   struct BuildCondNE : NoCopy
    {
     BuildCondArg a;
     BuildCondArg b;

     BuildCondNE(PosStr a_,PosStr b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"#; != #;",a,b);
      }
    };

   struct BuildCondGT : NoCopy
    {
     BuildCondArg a;
     BuildCondArg b;

     BuildCondGT(PosStr a_,PosStr b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"#; > #;",a,b);
      }
    };

   struct BuildCondGE : NoCopy
    {
     BuildCondArg a;
     BuildCondArg b;

     BuildCondGE(PosStr a_,PosStr b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"#; >= #;",a,b);
      }
    };

   struct BuildCondLT : NoCopy
    {
     BuildCondArg a;
     BuildCondArg b;

     BuildCondLT(PosStr a_,PosStr b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"#; < #;",a,b);
      }
    };

   struct BuildCondLE : NoCopy
    {
     BuildCondArg a;
     BuildCondArg b;

     BuildCondLE(PosStr a_,PosStr b_) : a(a_),b(b_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"#; <= #;",a,b);
      }
    };

   struct BuildAtom : NoCopy
    {
     SLink<BuildAtom> link;

     StrLen str;

     AtomDesc *desc = 0 ;

     explicit BuildAtom(StrLen str_) : str(str_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Putobj(out,str);
      }
    };

   struct BuildSynt;

   struct BuildKind : NoCopy , PosStr
    {
     SLink<BuildKind> link;

     BuildSynt *synt;

     KindDesc *desc = 0 ;

     BuildKind(PosStr postr,BuildSynt *synt_) : PosStr(postr),synt(synt_) {}

     // print object

     void print(PrinterType auto &out) const
      {
       Putobj(out,str);
      }
    };

   struct BuildElement : NoCopy , PosStr
    {
     SLink<BuildElement> link;

     BuildSynt *synt = 0 ;
     StrLen arg;
     BuildAtom *atom = 0 ;

     ulen index = MaxULen ;

     explicit BuildElement(PosStr postr) : PosStr(postr) {}

     bool cutArg(ulen name_len)
      {
       ulen len=str.len;

       if( len>name_len )
         {
          arg=str.part(name_len+1);

          return true;
         }

       return false;
      }

     // print object

     void print(PrinterType auto &out) const
      {
       Putobj(out,str);

       if( synt )
         {
          Putch(out,'$');

          if( +arg ) Putch(out,'$');
         }

       if( atom )
         {
          Putch(out,'@');
         }
      }
    };

   struct BuildRule : NoCopyBase<PosStr>
    {
     SLink<BuildRule> link;

     BuildCond cond;
     PosStr result;
     ObjList<BuildElement> element_list;

     BuildKind *result_kind = 0 ;

     BuildRule() {}

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"rule #; ",str);

       if( +cond ) Printf(out,"if( #; ) ",cond);

       Putobj(out," (");

       element_list.apply( [&] (const BuildElement &element) { Printf(out," #;",element); } );

       Putobj(out," )");

       if( result_kind ) Printf(out," -> #;",*result_kind);
      }
    };

   struct BuildSynt : NoCopy , PosStr
    {
     SLink<BuildSynt> link;

     bool is_lang;
     ObjList<BuildKind> kind_list;
     ObjList<BuildRule> rule_list;

     SyntDesc *desc = 0 ;
     ulen rule_off = MaxULen ;

     ObjList<BuildCondArg> arg_list;

     BuildSynt(PosStr postr,bool is_lang_) : PosStr(postr),is_lang(is_lang_) {}

     void add(BuildCondArg *arg) { arg_list.add(arg); }

     // print object

     void print(PrinterType auto &out) const
      {
       Printf(out,"synt #;",str);

       if( is_lang ) Putobj(out," lang");

       if( +kind_list )
         {
          Putobj(out," { ");

          kind_list.apply( [&] (const BuildKind &kind) { Printf(out,"#; , ",kind); } ,
                           [&] (const BuildKind &kind) { Printf(out,"#;",kind); }
                         );

          Putobj(out," }");
         }

       Putobj(out,"\n\n");

       rule_list.apply( [&] (const BuildRule &rule) { Printf(out,"  #;\n",rule); } );

       Putch(out,'\n');
      }
    };

   struct BuildNoneofAtom : NoCopy
    {
     SLink<BuildNoneofAtom> link;

     PosStr name;

     explicit BuildNoneofAtom(PosStr postr) : name(postr) {}
    };

   struct BuildNoneofSynt : NoCopy
    {
     SLink<BuildNoneofSynt> link;

     PosStr name;
     PosStr rule_name;

     ObjList<BuildNoneofAtom> atom_list;

     BuildSynt *synt = 0 ;

     explicit BuildNoneofSynt(PosStr postr) : name(postr) {}

     void addAtom(BuildNoneofAtom *atom) { atom_list.add(atom); }

     void setRuleName(PosStr postr) { rule_name=postr; }
    };

  public:

   BuildCond condOR(BuildCond a,BuildCond b);
   BuildCond condAND(BuildCond a,BuildCond b);
   BuildCond condNOT(BuildCond a);
   BuildCond condEQ(PosStr a,PosStr b);
   BuildCond condNE(PosStr a,PosStr b);
   BuildCond condGT(PosStr a,PosStr b);
   BuildCond condGE(PosStr a,PosStr b);
   BuildCond condLT(PosStr a,PosStr b);
   BuildCond condLE(PosStr a,PosStr b);

  public:

   void startSynt(PosStr postr,bool is_lang);

   void addKind(PosStr postr);
   void endKinds();

   void startRule();

   void addElement(PosStr postr);
   void endElements();

   void rule(BuildCond cond);
   void rule(PosStr postr);
   void result(PosStr postr);

   void endRule();

   void endSynt();

   void startNoneofSynt(PosStr postr);

   void addAtom(PosStr postr);

   void endAtoms();

   void ruleNoneofSynt(PosStr postr);

   void endLang();

  public:

   template <class ... TT>
   static void error(const char *format,const TT & ... tt)
    {
     Printf(NoException,format,tt...);
    }

   template <class ... TT>
   static void exception(const char *format,const TT & ... tt)
    {
     Printf(Exception,format,tt...);
    }

  private:

   CondLang &lang;
   ElementPool pool;

   ObjList<BuildSynt> synt_list;
   ObjList<BuildAtom> atom_list;
   ObjList<BuildNoneofSynt> noneof_list;

   BuildSynt *current_synt = 0 ;
   BuildRule *current_rule = 0 ;
   BuildNoneofSynt *current_noneof = 0 ;

   ObjList<BuildCondArg> arg_list;

  private:

   static bool CorrectElement(StrLen name);

   static StrLen SyntElementName(StrLen name);

   PosStr buildRuleName(PosStr name,ulen index);

   void buildNoneofSynts();

   void updateNoneofSynts(BuildSynt *synt,PosStr rule_name,ObjList<BuildNoneofAtom> &atom_list);

   void updateNoneofSynts();

   struct BindName
    {
     StrLen name;

     explicit BindName(StrLen name_) : name(name_) {}

     bool operator < (BindName obj) const { return StrLess(name,obj.name); }

     bool operator != (BindName obj) const { return StrCmp(name,obj.name); }
    };

   struct BindElement : BindName
    {
     AnyPtr<BuildSynt,BuildElement> ptr;

     explicit BindElement(BuildSynt &synt) : BindName(synt.str),ptr(&synt) {}

     BindElement(StrLen name,BuildElement &element) : BindName(name),ptr(&element) {}
    };

   void bindElements(PtrLen<BindElement> range);

   void bindElements();

   struct BindAtom : BindName
    {
     BuildElement *element;

     explicit BindAtom(BuildElement &element_) : BindName(element_.str),element(&element_) {}
    };

   static bool IsExpChar(char ch) { return ch=='\\' || ch=='"' ; }

   static ulen ExpCharCount(CursorOverType<char> auto r)
    {
     ulen ret=0;

     for(; +r ;++r) if( IsExpChar(*r) ) ret++;

     return ret;
    }

   class PutChar : NoCopy
    {
      PtrLen<char> out;

     public:

      explicit PutChar(PtrLen<char> out_) : out(out_) {}

      void put() {}

      void put(char ch) { *out=ch; ++out; }

      template <class ... CC>
      void put(char ch,CC ... cc)
       {
        put(ch);
        put(cc...);
       }

      void transform(char ch)
       {
        if( IsExpChar(ch) )
          put('\\',ch);
        else
          put(ch);
       }

      void transform(CursorOverType<char> auto r)
       {
        for(; +r ;++r) transform(*r);
       }
    };

   StrLen buildAtomName(StrLen name);

   void bindAtom(PtrLen<BindAtom> range);

   void buildAtoms();

   struct CheckRuleName : BindName
    {
     BuildRule *rule;

     explicit CheckRuleName(BuildRule &rule_) : BindName(rule_.str),rule(&rule_) {}
    };

   void checkRuleNames(BuildSynt &synt);

   void checkRuleNames();

   struct BindResult : BindName
    {
     AnyPtr<BuildRule,BuildKind> ptr;

     explicit BindResult(BuildRule &rule) : BindName(rule.result.str),ptr(&rule) {}

     explicit BindResult(BuildKind &kind) : BindName(kind.str),ptr(&kind) {}
    };

   void bindResults(PtrLen<BindResult> range);

   void bindResults();

   struct BindArgs : BindName
    {
     AnyPtr<BuildElement,BuildCondArg> ptr;

     explicit BindArgs(BuildElement &element) : BindName(element.arg),ptr(&element) {}

     explicit BindArgs(BuildCondArg &arg) : BindName(arg.str),ptr(&arg) {}
    };

   struct BindCondArgs
    {
     Collector<BindArgs> &collector;

     explicit BindCondArgs(Collector<BindArgs> &collector_) : collector(collector_) {}

     void operator () (BuildCondAND *cond)
      {
       cond->a.apply(*this);
       cond->b.apply(*this);
      }

     void operator () (BuildCondOR *cond)
      {
       cond->a.apply(*this);
       cond->b.apply(*this);
      }

     void operator () (BuildCondNOT *cond)
      {
       cond->a.apply(*this);
      }

     template <class T>
     void operator () (T *cond)
      {
       cond->a.peer=&cond->b;
       cond->b.peer=&cond->a;

       collector.append_fill(cond->a);
       collector.append_fill(cond->b);
      }
    };

   struct BindKindArg : BindName
    {
     AnyPtr<BuildKind,BuildCondArg> ptr;

     explicit BindKindArg(BuildKind &kind) : BindName(kind.str),ptr(&kind) {}

     explicit BindKindArg(BuildCondArg &arg) : BindName(arg.str),ptr(&arg) {}
    };

   void bindKindArgs();

   void bindArgs(PtrLen<BindArgs> range);

   void bindArgs(BuildRule &rule);

   void bindArgs(PtrLen<BindKindArg> range);

   void bindArgs(BuildSynt &synt);

   void bindArgs();

   struct BuildLangCond : NoCopy
    {
     Builder *builder;
     Cond result;

     CmpArg buildArg(BuildCondArg &arg)
      {
       if( BuildKind *kind=arg.kind )
         {
          auto ptr=builder->lang.pool.create<CmpArgKind>();

          ptr->kind=kind->desc;

          return ptr;
         }
       else
         {
          auto ptr=builder->lang.pool.create<CmpArgElement>();

          ptr->index=arg.element->index;

          return ptr;
         }
      }

     explicit BuildLangCond(Builder *builder_) : builder(builder_) {}

     void operator () (BuildCondAND *cond)
      {
       auto a=builder->buildCond(cond->a);
       auto b=builder->buildCond(cond->b);

       auto result_=builder->lang.pool.create<CondAND>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondOR *cond)
      {
       auto a=builder->buildCond(cond->a);
       auto b=builder->buildCond(cond->b);

       auto result_=builder->lang.pool.create<CondOR>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondNOT *cond)
      {
       auto a=builder->buildCond(cond->a);

       auto result_=builder->lang.pool.create<CondNOT>();

       result_->a=a;

       result=result_;
      }

     void operator () (BuildCondEQ *cond)
      {
       auto a=buildArg(cond->a);
       auto b=buildArg(cond->b);

       auto result_=builder->lang.pool.create<CondEQ>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondNE *cond)
      {
       auto a=buildArg(cond->a);
       auto b=buildArg(cond->b);

       auto result_=builder->lang.pool.create<CondNE>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondGT *cond)
      {
       auto a=buildArg(cond->a);
       auto b=buildArg(cond->b);

       auto result_=builder->lang.pool.create<CondGT>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondGE *cond)
      {
       auto a=buildArg(cond->a);
       auto b=buildArg(cond->b);

       auto result_=builder->lang.pool.create<CondGE>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondLT *cond)
      {
       auto a=buildArg(cond->a);
       auto b=buildArg(cond->b);

       auto result_=builder->lang.pool.create<CondLT>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }

     void operator () (BuildCondLE *cond)
      {
       auto a=buildArg(cond->a);
       auto b=buildArg(cond->b);

       auto result_=builder->lang.pool.create<CondLE>();

       result_->a=a;
       result_->b=b;

       result=result_;
      }
    };

   Cond buildCond(BuildCond cond);

   void complete();

  public:

   explicit Builder(CondLang &lang);

   ~Builder();

   void print(PrinterType auto &out) const
    {
     atom_list.apply( [&] (const BuildAtom &atom) { Printf(out,"#;\n",atom); } );

     Putch(out,'\n');

     synt_list.apply( [&] (const BuildSynt &synt) { Printf(out,"#;\n",synt); } );
    }
 };

auto CondLang::Builder::condOR(BuildCond a,BuildCond b) -> BuildCond
 {
  return pool.create<BuildCondOR>(a,b);
 }

auto CondLang::Builder::condAND(BuildCond a,BuildCond b) -> BuildCond
 {
  return pool.create<BuildCondAND>(a,b);
 }

auto CondLang::Builder::condNOT(BuildCond a) -> BuildCond
 {
  return pool.create<BuildCondNOT>(a);
 }

auto CondLang::Builder::condEQ(PosStr a,PosStr b) -> BuildCond
 {
  return pool.create<BuildCondEQ>(a,b);
 }

auto CondLang::Builder::condNE(PosStr a,PosStr b) -> BuildCond
 {
  return pool.create<BuildCondNE>(a,b);
 }

auto CondLang::Builder::condGT(PosStr a,PosStr b) -> BuildCond
 {
  return pool.create<BuildCondGT>(a,b);
 }

auto CondLang::Builder::condGE(PosStr a,PosStr b) -> BuildCond
 {
  return pool.create<BuildCondGE>(a,b);
 }

auto CondLang::Builder::condLT(PosStr a,PosStr b) -> BuildCond
 {
  return pool.create<BuildCondLT>(a,b);
 }

auto CondLang::Builder::condLE(PosStr a,PosStr b) -> BuildCond
 {
  return pool.create<BuildCondLE>(a,b);
 }

void CondLang::Builder::startSynt(PosStr postr,bool is_lang)
 {
  current_synt=pool.create<BuildSynt>(postr,is_lang);
 }

void CondLang::Builder::addKind(PosStr postr)
 {
  current_synt->kind_list.add(pool.create<BuildKind>(postr,current_synt));
 }

void CondLang::Builder::endKinds()
 {
  // do nothing
 }

void CondLang::Builder::startRule()
 {
  current_rule=pool.create<BuildRule>();
 }

void CondLang::Builder::addElement(PosStr postr)
 {
  if( CorrectElement(postr.str) ) ++postr.str;

  current_rule->element_list.add(pool.create<BuildElement>(postr));
 }

void CondLang::Builder::endElements()
 {
  // do nothing
 }

void CondLang::Builder::rule(BuildCond cond)
 {
  current_rule->cond=cond;
 }

void CondLang::Builder::rule(PosStr postr)
 {
  *(PosStr *)current_rule=postr;
 }

void CondLang::Builder::result(PosStr postr)
 {
  current_rule->result=postr;
 }

void CondLang::Builder::endRule()
 {
  current_synt->rule_list.add(Replace_null(current_rule));
 }

void CondLang::Builder::endSynt()
 {
  synt_list.add(Replace_null(current_synt));
 }

void CondLang::Builder::startNoneofSynt(PosStr postr)
 {
  current_noneof=pool.create<BuildNoneofSynt>(postr);
 }

void CondLang::Builder::addAtom(PosStr postr)
 {
  if( CorrectElement(postr.str) ) ++postr.str;

  current_noneof->addAtom(pool.create<BuildNoneofAtom>(postr));
 }

void CondLang::Builder::endAtoms()
 {
  // do nothing
 }

void CondLang::Builder::ruleNoneofSynt(PosStr postr)
 {
  current_noneof->setRuleName(postr);

  noneof_list.add(Replace_null(current_noneof));
 }

void CondLang::Builder::endLang()
 {
  {
   ReportException report;

   buildNoneofSynts();
   bindElements();
   buildAtoms();
   checkRuleNames();
   updateNoneofSynts();
   bindResults();
   bindArgs();

   report.guard();
  }

  //Putobj(Con,*this);

  complete();
 }

bool CondLang::Builder::CorrectElement(StrLen name)
 {
  if( name.len>=2 && name[0]=='`' )
    {
     char ch=name.back(1);

     if( ch=='}' || ch==':' )
       {
        for(auto r=name.inner(1,1); +r ;++r)
          if( *r!='`' )
            return false;

        return true;
       }
    }

  return false;
 }

StrLen CondLang::Builder::SyntElementName(StrLen name)
 {
  ulen len=LangParser::ScanName(name);

  if( !len ) return {};

  if( len==name.len ) return name;

  if( name[len]=='.' && LangParser::IsName(name.part(len+1)) ) return name.prefix(len);

  return {};
 }

void CondLang::Builder::buildNoneofSynts()
 {
  noneof_list.apply( [this] (BuildNoneofSynt &noneof)
                            {
                             BuildSynt *synt=pool.create<BuildSynt>(noneof.name,false);

                             synt_list.add(synt);

                             noneof.synt=synt;
                            }
                   );
 }

PosStr CondLang::Builder::buildRuleName(PosStr name,ulen index)
 {
  String name_ind=Stringf("#;@A#;",name.str,index);

  return PosStr(name.pos,pool.dup(Range(name_ind)));
 }

void CondLang::Builder::updateNoneofSynts(BuildSynt *synt,PosStr rule_name,ObjList<BuildNoneofAtom> &none_list)
 {
  DynArray<StrLen> temp(DoReserve,none_list.getCount());

  none_list.apply( [&] (BuildNoneofAtom &atom) { temp.append_copy(buildAtomName(atom.name.str)); } );

  ulen index=0;

  atom_list.apply( [&] (BuildAtom &atom)
                       {
                        for(StrLen str : temp )
                          if( str.equal(atom.str) )
                            {
                             index++;

                             return;
                            }

                        BuildRule *rule=pool.create<BuildRule>();

                        *(PosStr *)rule=buildRuleName(rule_name,index++);

                        BuildElement *element=pool.create<BuildElement>(PosStr(atom.str));

                        element->atom=&atom;

                        rule->element_list.add(element);

                        synt->rule_list.add(rule);
                       }
                 );
 }

void CondLang::Builder::updateNoneofSynts()
 {
  noneof_list.apply( [this] (BuildNoneofSynt &noneof)
                            {
                             updateNoneofSynts(noneof.synt,noneof.rule_name,noneof.atom_list);
                            }
                   );
 }

void CondLang::Builder::bindElements(PtrLen<BindElement> range)
 {
  BuildSynt *synt=0;
  ulen count=0;

  for(auto &bind : range ) bind.ptr.applyFor<BuildSynt>( [&] (BuildSynt *synt_) { synt=synt_; count++; } );

  switch( count )
    {
     case 0 :
      {
       // do nothing
      }
     break;

     case 1 :
      {
       ulen name_len=synt->str.len;

       for(auto &bind : range )
         bind.ptr.applyFor<BuildElement>( [=] (BuildElement *element)
                                              {
                                               element->synt=synt;

                                               if( element->cutArg(name_len) && !synt->kind_list )
                                                 {
                                                  error("Builder #; : no syntax class kinds, argument is not allowed",element->pos);
                                                 }
                                              }
                                        );
      }
     break;

     default:
      {
       for(auto &bind : range )
         bind.ptr.applyFor<BuildSynt>( [this] (BuildSynt *synt)
                                              {
                                               error("Builder #; : multiple declaration of syntax class #;",synt->pos,synt->str);
                                              }
                                     );
      }
    }
 }

void CondLang::Builder::bindElements()
 {
  ulen lang_count=0;
  Collector<BindElement> collector;

  synt_list.apply( [&] (BuildSynt &synt)
                       {
                        lang_count+=synt.is_lang;

                        collector.append_fill(synt);

                        synt.rule_list.apply( [&] (BuildRule &rule)
                                                  {
                                                   rule.element_list.apply( [&] (BuildElement &element)
                                                                                {
                                                                                 StrLen name=SyntElementName(element.str);

                                                                                 if( +name ) collector.append_fill(name,element);
                                                                                }
                                                                          );
                                                  }
                                            );
                       }
                 );

  Algon::SortThenApplyUniqueRange(collector.flat(), [this] (PtrLen<BindElement> range) { bindElements(range); } );

  if( !lang_count ) error("Builder : no lang syntax class");
 }

StrLen CondLang::Builder::buildAtomName(StrLen name)
 {
  ulen len=LenAdd(5,name.len,ExpCharCount(name));

  auto ret=pool.createArray_raw<char>(len);

  PutChar out(ret);

  out.put('A','(','"');

  out.transform(name);

  out.put('"',')');

  return Range_const(ret);
 }

void CondLang::Builder::bindAtom(PtrLen<BindAtom> range)
 {
  StrLen name=range->element->str;
  BuildAtom *atom=pool.create<BuildAtom>(buildAtomName(name));

  atom_list.add(atom);

  for(auto &bind: range ) bind.element->atom=atom;
 }

void CondLang::Builder::buildAtoms()
 {
  Collector<BindAtom> collector;

  synt_list.apply( [&] (BuildSynt &synt)
                       {
                        synt.rule_list.apply( [&] (BuildRule &rule)
                                                  {
                                                   rule.element_list.apply( [&] (BuildElement &element)
                                                                                {
                                                                                 if( !element.synt ) collector.append_fill(element);
                                                                                }
                                                                          );
                                                  }
                                            );
                       }
                 );

  Algon::SortThenApplyUniqueRange(collector.flat(), [this] (PtrLen<BindAtom> range) { bindAtom(range); } );
 }

void CondLang::Builder::checkRuleNames(BuildSynt &synt)
 {
  Collector<CheckRuleName> collector;

  synt.rule_list.apply( [&] (BuildRule &rule) { collector.append_fill(rule); } );

  Algon::SortThenApplyUniqueRange(collector.flat(), [this] (PtrLen<CheckRuleName> range)
                                                           {
                                                            if( range.len>1 )
                                                              {
                                                               for(; +range ;++range)
                                                                 error("Builder #; : multiple declaration of rule #;",range->rule->pos,range->rule->str);
                                                              }
                                                           }
                                 );
 }

void CondLang::Builder::checkRuleNames()
 {
  synt_list.apply( [this] (BuildSynt &synt) { checkRuleNames(synt); } );
 }

void CondLang::Builder::bindResults(PtrLen<BindResult> range)
 {
  BuildKind *kind=0;
  ulen count=0;

  for(auto &bind : range ) bind.ptr.applyFor<BuildKind>( [&] (BuildKind *kind_) { kind=kind_; count++; } );

  switch( count )
    {
     case 0 :
      {
       for(auto &bind : range )
         bind.ptr.applyFor<BuildRule>( [this] (BuildRule *rule)
                                              {
                                               error("Builder #; : bad kind name #;",rule->result.pos,rule->result.str);
                                              }
                                     );
      }
     break;

     case 1 :
      {
       for(auto &bind : range ) bind.ptr.applyFor<BuildRule>( [=] (BuildRule *rule) { rule->result_kind=kind; } );
      }
     break;

     default:
      {
       for(auto &bind : range )
         bind.ptr.applyFor<BuildKind>( [this] (BuildKind *kind)
                                              {
                                               error("Builder #; : multiple declaration of kind #;",kind->pos,kind->str);
                                              }
                                     );
      }
    }
 }

void CondLang::Builder::bindResults()
 {
  synt_list.apply( [this] (BuildSynt &synt)
                          {
                           if( +synt.kind_list )
                             {
                              Collector<BindResult> collector;

                              synt.rule_list.apply( [&] (BuildRule &rule)
                                                        {
                                                         collector.append_fill(rule);
                                                        }
                                                  );

                              synt.kind_list.apply( [&] (BuildKind &kind)
                                                        {
                                                         collector.append_fill(kind);
                                                        }
                                                  );

                              Algon::SortThenApplyUniqueRange(collector.flat(), [this] (PtrLen<BindResult> range)
                                                                                       {
                                                                                        bindResults(range);
                                                                                       }
                                                             );
                             }
                          }
                 );
 }

void CondLang::Builder::bindKindArgs()
 {
  while( BuildCondArg *arg=arg_list.del() )
    {
     if( BuildElement *element=arg->peer->element )
       {
        element->synt->add(arg);
       }
     else
       {
        error("Builder #; : comparision of constants",arg->pos);
       }
    }
 }

void CondLang::Builder::bindArgs(PtrLen<BindArgs> range)
 {
  BuildElement *element=0;
  ulen count=0;

  for(auto &bind : range ) bind.ptr.applyFor<BuildElement>( [&] (BuildElement *element_) { element=element_; count++; } );

  switch( count )
    {
     case 0 :
      {
       for(auto &bind : range ) bind.ptr.applyFor<BuildCondArg>( [this] (BuildCondArg *arg) { arg_list.add(arg); } );
      }
     break;

     case 1 :
      {
       for(auto &bind : range ) bind.ptr.applyFor<BuildCondArg>( [=] (BuildCondArg *arg) { arg->element=element; } );
      }
     break;

     default:
      {
       for(auto &bind : range )
         bind.ptr.applyFor<BuildElement>( [this] (BuildElement *element)
                                                 {
                                                  error("Builder #; : multiple declaration of element #; argument",element->pos,element->str);
                                                 }
                                        );
      }
    }
 }

void CondLang::Builder::bindArgs(BuildRule &rule)
 {
  Collector<BindArgs> collector;

  ulen index=0;

  rule.element_list.apply( [&] (BuildElement &element)
                               {
                                if( +element.arg )
                                  {
                                   element.index=index;

                                   collector.append_fill(element);
                                  }

                                index++;
                               }
                         );

  rule.cond.apply( BindCondArgs(collector) );

  Algon::SortThenApplyUniqueRange(collector.flat(), [this] (PtrLen<BindArgs> range) { bindArgs(range); } );
 }

void CondLang::Builder::bindArgs(PtrLen<BindKindArg> range)
 {
  BuildKind *kind=0;

  for(auto &bind : range ) bind.ptr.applyFor<BuildKind>( [&] (BuildKind *kind_) { kind=kind_; } );

  if( kind )
    {
     for(auto &bind : range ) bind.ptr.applyFor<BuildCondArg>( [=] (BuildCondArg *arg) { arg->kind=kind; } );
    }
  else
    {
     for(auto &bind : range )
       bind.ptr.applyFor<BuildCondArg>( [this] (BuildCondArg *arg)
                                               {
                                                error("Builder #; : bad kind name #;",arg->pos,arg->str);
                                               }
                                      );
    }
 }

void CondLang::Builder::bindArgs(BuildSynt &synt)
 {
  Collector<BindKindArg> collector;

  synt.kind_list.apply( [&] (BuildKind &kind) { collector.append_fill(kind); } );

  synt.arg_list.apply( [&] (BuildCondArg &arg) { collector.append_fill(arg); } );

  Algon::SortThenApplyUniqueRange(collector.flat(), [this] (PtrLen<BindKindArg> range) { bindArgs(range); } );
 }

void CondLang::Builder::bindArgs()
 {
  synt_list.apply( [this] (BuildSynt &synt)
                          {
                           synt.rule_list.apply( [this] (BuildRule &rule)
                                                        {
                                                         if( +rule.cond ) bindArgs(rule);
                                                        }
                                               );
                          }
                 );

  bindKindArgs();

  synt_list.apply( [this] (BuildSynt &synt) { bindArgs(synt); } );
 }

auto CondLang::Builder::buildCond(BuildCond cond) -> Cond
 {
  BuildLangCond func(this);

  cond.apply( FunctorRef(func) );

  return func.result;
 }

void CondLang::Builder::complete()
 {
  // atoms
  {
   ulen len=atom_list.getCount();

   auto atoms=lang.createAtoms(len);

   ulen index=0;

   atom_list.apply( [&] (BuildAtom &atom)
                        {
                         auto &desc=atoms[index];

                         desc.index=index++;
                         desc.name=lang.pool.dup(atom.str);

                         atom.desc=&desc;
                        }
                  );
  }

  // synt
  {
   ulen len=synt_list.getCount();

   auto synts=lang.createSynts(len);

   ulen index=0;

   synt_list.apply( [&] (BuildSynt &synt)
                        {
                         auto &desc=synts[index];

                         desc.index=index++;
                         desc.name=lang.pool.dup(synt.str);

                         desc.is_lang=synt.is_lang;

                         if( ulen klen=synt.kind_list.getCount() )
                           {
                            auto kinds=lang.createKinds(desc,klen);

                            ulen kindex=0;

                            synt.kind_list.apply( [&] (BuildKind &kind)
                                                      {
                                                       KindDesc &kdesc=kinds[kindex];

                                                       kdesc.index=kindex++;
                                                       kdesc.name=lang.pool.dup(kind.str);

                                                       kind.desc=&kdesc;
                                                      }
                                                );
                           }

                         synt.desc=&desc;
                        }
                  );
  }

  // rules
  {
   ulen len=0;

   synt_list.apply( [&] (BuildSynt &synt)
                        {
                         synt.rule_off=len;

                         len=LenAdd(len,synt.rule_list.getCount());
                        }
                  );

   auto rules=lang.createRules(len);

   synt_list.apply( [&] (BuildSynt &synt)
                        {
                         ulen index=synt.rule_off;

                         synt.desc->rules=Range_const(rules.part(index,synt.rule_list.getCount()));

                         synt.rule_list.apply( [&] (BuildRule &rule)
                                                   {
                                                    RuleDesc &desc=rules[index];

                                                    desc.index=index++;
                                                    desc.name=lang.pool.dup(rule.str);

                                                    desc.ret=synt.desc;

                                                    if( rule.result_kind )
                                                      desc.kind=rule.result_kind->desc;

                                                    if( +rule.cond )
                                                      desc.cond=buildCond(rule.cond);

                                                    ulen alen=rule.element_list.getCount();

                                                    auto args=lang.createElements(desc,alen);

                                                    rule.element_list.apply( [&] (BuildElement &element)
                                                                                 {
                                                                                  if( BuildSynt *synt=element.synt )
                                                                                    {
                                                                                     args->ptr=synt->desc;
                                                                                    }
                                                                                  else
                                                                                    {
                                                                                     args->ptr=element.atom->desc;
                                                                                    }

                                                                                  ++args;
                                                                                 }
                                                                           );
                                                   }
                                             );
                        }
                  );
  }
 }

CondLang::Builder::Builder(CondLang &lang_)
 : lang(lang_)
 {
 }

CondLang::Builder::~Builder()
 {
 }

/* class CondLang */

CondLang::CondLang(StrLen file_name)
 {
  FileToMem file(file_name);

  LangParser::Parser<Builder> parser(*this);

  parser.run(Mutate<const char>(Range(file)));

  pool.shrink_extra();
 }

CondLang::~CondLang()
 {
 }

} // namespace App

