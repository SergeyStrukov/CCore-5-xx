/* Process.cpp */
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

#include "inc/process/Process.h"

#include "inc/process/NonEmptyEstimate.h"
#include "inc/process/GoodEstimate.h"
#include "inc/process/LR1Estimate.h"

#include "inc/process/LR1Prop.h"

#include "inc/process/StateMap.h"

#include "inc/PrintTools.h"

#include <CCore/inc/Path.h>
#include <CCore/inc/String.h>
#include <CCore/inc/Exception.h>

namespace App {

/* RunGoodTest() */

static bool RunGoodTest(const Lang &lang)
 {
  LangEstimate<GoodEstimate> estimate(lang);

  bool ret=true;

  lang.applyForSynts( [&] (Synt synt)
                          {
                           auto est=estimate[synt];

                           if( est.notGood() )
                             {
                              if( !est )
                                {
                                 Printf(NoException,"#; is #;\n",synt.getName(),est);

                                 ret=false;
                                }
                              else
                                {
                                 Printf(Con,"Warning: #; is #;\n",synt.getName(),est);
                                }

                             }
                          }
                    );

  return ret;
 }

/* PrintFibres() */

template <class TopCompress>
void PrintFibre(PrinterType auto &out,const TopCompress &compress,PtrLen<const State> range)
 {
  ulen prop=range->getPropIndex();
  bool flag=false;

  for(State top : range )
    {
     ulen index=top.getPropIndex();

     Printf(out," #;:#;)",top.getIndex(),index);

     if( index!=prop ) flag=true;
    }

  if( flag )
    {
     for(State top : range )
       {
        Printf(out,"  #.b;\n  -----\n",compress.getProps()[top.getPropIndex()]);
       }
    }

  Putch(out,'\n');
 }

[[maybe_unused]] static bool HasMultipleProps(PtrLen<const State> range)
 {
  ulen prop=range->getPropIndex();

  for(State top : range )
    {
     if( top.getPropIndex()!=prop ) return true;
    }

  return false;
 }

template <class TopCompress,class BottomCompress>
void PrintFibres(PrinterType auto &out,const TopCompress &compress,const BottomCompress &bottom,const StateMap &map)
 {
  Printf(out,"#;\n",Title("Fibres"));

  bottom.applyForStates( [&] (State state)
                             {
                              auto range=map.getFibre(state);

                              Printf(out,"\n#;:#;) <-",state.getIndex(),state.getPropIndex());

                              if( !range )
                                {
                                 Putobj(out," empty\n");
                                }
                              else
                                {
                                 PrintFibre(out,compress,range);
                                }
                             }
                       );

  {
   auto range=map.getNullFibre();

   if( +range )
     {
      Printf(out,"\nNULL <-");

      PrintFibre(out,compress,range);
     }
  }

  Printf(out,"\n#;\n",TextDivider());

  ulen count=0;

  bottom.applyForStates( [&] (State state)
                             {
                              auto range=map.getFibre(state);

                              if( range.len>1 && HasMultipleProps(range) )
                                {
                                 Printf(out,"\n#;) MP fibre size #;\n",state.getIndex(),range.len);

                                 count++;
                                }
                             }
                       );

  {
   auto range=map.getNullFibre();

   if( range.len>1 && HasMultipleProps(range) )
     {
      Printf(out,"\nNULL MP fibre size #;\n",range.len);

      count++;
     }
  }

  Printf(out,"\nMP fibres = #;\n",count);

  Printf(out,"\n#;\n",TextDivider());
 }

/* PrintBad() */

void PrintBad(PrinterType auto &out,const Lang &lang,const StateCompress<LR1Estimate> &compress)
 {
  ExtLangOpt extopt(lang,compress.getAtomCount());

  Printf(out,"#;\n",Title("Bad States"));

  ulen state_count=compress.getStateCount();

  DynArray<Trace> trace_table(state_count);

  for(ulen i=0; i<state_count ;i++) trace_table[i]=compress.buildTrace(i);

  for(auto p=compress.getStateTable(); +p ;++p)
    {
     ulen prop_index=p->prop_index;

     auto &prop=compress.getProps()[prop_index];

     if( prop.hasConflict() )
       {
        Printf(out,"\n#; = #;\n\n",BindOpt(extopt,PrintState(p->index,trace_table)),prop_index);

        Printf(out,"#;) #.b;",prop_index,prop);
       }
    }

  Printf(out,"\n#;\n",TextDivider());
 }

/* PrintRules() */

void PrintRules(PrinterType auto &out,const LangBase::SyntDesc &desc)
 {
  Putobj(out,"{ ");

  auto rules=desc.rules;

  for(ulen i=0; i<rules.len ;i++)
    {
     Printf(out,"lang.rules+#;",rules[i].index);

     if( i+1<rules.len ) Putobj(out," , ");
    }

  Putobj(out," }");
 }

/* PrintArgs() */

void PrintArgs(PrinterType auto &out,const LangBase::RuleDesc &desc)
 {
  Putobj(out,"{ ");

  auto args=desc.args;

  for(ulen i=0; i<args.len ;i++)
    {
     args[i].apply( [&] (Atom atom) { Printf(out,"lang.atoms+#;",atom.getIndex()); } ,
                    [&] (Synt synt) { Printf(out,"lang.synts+#;",synt.getIndex()); } );

     if( i+1<args.len ) Putobj(out," , ");
    }

  Putobj(out," }");
 }

/* Process() */

void Process(StrLen file_name)
 {
  SplitPath dev_name(file_name);
  SplitName path_name(dev_name.path);
  SplitExt name_ext(path_name.name);

  String in_name;

  if( !name_ext )
    {
     in_name=StringCat(dev_name.dev,path_name.path,name_ext.name,".lang");
    }
  else
    {
     in_name=file_name;
    }

  TrackStage("Load file #.q;",StrLen(Range(in_name)));

  CondLang clang(Range(in_name));

  TrackStage("Build top lang");

  TopLang top(clang);

  TrackStage("Run good test on top lang");

  if( !RunGoodTest(top) ) return;

  TrackStage("Build bottom lang");

  BottomLang bottom(clang);

  TrackStage("Extend bottom lang");

  ExtLang ext_bottom(bottom);

  TrackStage("Process top lang");

  ExtLang ext_top(top);
  LangDiagram diagram(ext_top);
  LangStateMachine<LR1Estimate,LR1MapContext> machine(ext_top,diagram,ext_bottom);

  StateCompress<LR1Estimate> compress(machine);

  TrackStage("LR1) #;",PrintCompressCounts(compress));

  {
   ulen conflicts=0;

   for(auto &est : compress.getProps() ) conflicts+=est.hasConflict();

   if( conflicts )
     {
      {
       String out_name=StringCat(dev_name.dev,path_name.path,name_ext.name,".bad.txt");
       PrintFile out(Range(out_name),Open_ToWrite|Open_AutoDelete);

       Putobj(out,clang);

       PrintBad(out,ext_top,compress);

       out.preserveFile();
      }

      Printf(Exception,"#; CONFLICTs detected. Not LR1 language.",conflicts);
     }
   else
     {
      TrackStage("No conflicts. LR1 language.");
     }
  }

  StateCompress<LR1Estimate,LR1PropNonEmpty> compress_ne(machine);

  TrackStage("NonEmpty) #;",PrintCompressCounts(compress_ne));

#if 0

  StateCompress<LR1Estimate,LR1PropShiftSet> compress_shift(machine);

  TrackStage("Shift) #;",PrintCompressCounts(compress_shift));

  StateCompress<LR1Estimate,LR1PropValidSet> compress_valid(machine);

  TrackStage("Valid) #;",PrintCompressCounts(compress_valid));

  StateCompress<LR1Estimate,LR1PropRuleSet> compress_rules(machine);

  TrackStage("Rules) #;",PrintCompressCounts(compress_rules));

  StateMap map(compress,compress_ne);

  {
   String out_name=StringCat(dev_name.dev,path_name.path,"Result.txt");
   PrintFile out(Range(out_name),Open_ToWrite|Open_AutoDelete);

   PrintFibres(out,compress,compress_ne,map);

   Putobj(out,BindOpt(ext_top,compress_ne));
  }

#endif

  {
   String out_name=StringCat(dev_name.dev,path_name.path,name_ext.name,".txt");
   PrintFile out(Range(out_name),Open_ToWrite|Open_AutoDelete);

   Putobj(out,clang);

   Putobj(out,BindOpt(ext_top,compress));

   out.preserveFile();
  }

  {
   String out_name=StringCat(dev_name.dev,path_name.path,name_ext.name,".ddl");

   PosPrint<PrintFile> out(Range(out_name),Open_ToWrite|Open_AutoDelete);

   Printf(out,"/* #;.ddl */\n\n",name_ext.name);

   Putobj(out,"//include <LangTypes.ddl>\n\n");

   Putobj(out,"Lang lang=\n");

   // lang
   {
    ListPrint lang_out(out);

    // atoms
    {
     ListPrint atom_out(&lang_out);

     for(auto &atom : clang.getAtoms() )
       Printf(atom_out,"{ #; , #; , lang.elements+#; }#;",atom.index,StrLen(atom.name.inner(2,1)),atom.index,EndItem());

     Putobj(atom_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // synts
    {
     ListPrint synt_out(&lang_out);

     ulen element=clang.getAtomCount();
     ulen top_index=0;

     for(auto &synt : clang.getSynts() )
       {
        Printf(synt_out,"{ #; , #.q; ,",synt.index,synt.name);

        auto kinds=synt.kinds;

        if( !kinds )
          {
           Indent indent(synt_out.getCol());

           Printf(synt_out," { { #; , 0 , \"\" , lang.synts+#; , lang.elements+#; ,#;",top_index,synt.index,element++,AutoIndent());

           auto &top_synt=top.getSynts()[top_index++];

           ListPrint rule_out(&synt_out);

           for(auto &top_rule : top_synt.rules )
             Printf(rule_out,"lang.top_rules+#;#;",top_rule.index,EndItem());

           Putobj(rule_out,EndList());

           Printf(synt_out,"#;   }#; } ,#;",indent,indent,indent);
          }
        else
          {
           Indent indent(synt_out.getCol());

           Putobj(synt_out,indent);

           ListPrint kind_out(&synt_out);

           for(auto &kind : kinds )
             {
              Printf(kind_out,"{ #; , #; , #.q; , lang.synts+#; , lang.elements+#; ,#;",top_index,kind.index,kind.name,synt.index,element++,AutoIndent());

              auto &top_synt=top.getSynts()[top_index++];

              ListPrint rule_out(&kind_out);

              for(auto &top_rule : top_synt.rules )
                Printf(rule_out,"lang.top_rules+#;#;",top_rule.index,EndItem());

              Putobj(rule_out,EndList());

              Printf(kind_out,"\n}#;",EndItem());
             }

           Putobj(kind_out,EndList());

           Putobj(synt_out," ,",indent);
          }

        ListPrint rule_out(&synt_out);

        for(auto &rule : synt.rules )
          Printf(rule_out,"lang.rules+#;#;",rule.index,EndItem());

        Putobj(rule_out,EndList());

        Putobj(synt_out,"\n}",EndItem());
       }

     Putobj(synt_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // lang
    {
     ListPrint synt_out(&lang_out);

     for(auto &synt : clang.getSynts() )
       if( synt.is_lang )
         {
          Printf(synt_out,"lang.synts+#;",synt.index);
         }

     Putobj(synt_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // elements
    {
     ListPrint elem_out(&lang_out);

     ulen element=0;

     for(auto &atom : clang.getAtoms() )
       Printf(elem_out,"{ #; , lang.atoms+#; }#;",element++,atom.index,EndItem());

     for(auto &synt : clang.getSynts() )
       {
        auto len=synt.kinds.len;

        if( !len ) len=1;

        for(ulen i=0; i<len ;i++)
          {
           Printf(elem_out,"{ #; , lang.synts[#;].kinds+#; }#;",element++,synt.index,i,EndItem());
          }
       }

     Putobj(elem_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // rules
    {
     ListPrint rule_out(&lang_out);

     for(auto &rule : clang.getRules() )
       {
        Printf(rule_out,"{ #; , #.q; , lang.synts[#;].kinds+#; ,#;",rule.index,rule.name,rule.ret->index,rule.getKindIndex(),AutoIndent());

        ListPrint arg_out(&rule_out);

        for(auto &element : rule.args )
          element.apply( [&] (const CondLangBase::AtomDesc *desc)
                             {
                              Printf(arg_out,"lang.atoms+#;#;",desc->index,EndItem());
                             } ,
                         [&] (const CondLangBase::SyntDesc *desc)
                             {
                              Printf(arg_out,"lang.synts+#;#;",desc->index,EndItem());
                             }
                       );

        Putobj(arg_out,EndList());

        Putobj(rule_out,"\n}",EndItem());
       }

     Putobj(rule_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // top rules
    {
     ListPrint rule_out(&lang_out);

     for(auto &rule : top.getRules() )
       {
        Printf(rule_out,"{ #; , #.q; , lang.rules+#; , lang.synts[#;].kinds+#; ,#;",
          rule.index,rule.name,rule.map_index,rule.ret->map_index,rule.ret->kind_index,AutoIndent());

        ListPrint arg_out(&rule_out);

        for(auto &element : rule.args )
          element.apply( [&] (Atom atom)
                             {
                              Printf(arg_out,"lang.atoms+#;#;",atom.getIndex(),EndItem());
                             } ,
                         [&] (Synt synt)
                             {
                              Printf(arg_out,"lang.synts[#;].kinds+#;#;",synt.getMapIndex(),synt.getKindIndex(),EndItem());
                             }
                       );

        Putobj(arg_out,EndList());

        Putobj(rule_out,"\n}",EndItem());
       }

     Putobj(rule_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // states
    {
     ListPrint state_out(&lang_out);

     for(auto &state : compress.getStateTable() )
       {
        Printf(state_out,"{ #; , lang.finals+#; ,#;",state.index,state.prop_index,AutoIndent());

        ListPrint trans_out(&state_out);

        for(auto &trans : state.transitions )
          Printf(trans_out,"{ lang.elements+#; , lang.states+#; }#;",trans.element,trans.dst->index,EndItem());

        Putobj(trans_out,EndList());

        Putobj(state_out,"\n}",EndItem());
       }

     Putobj(state_out,EndList());
    }

    Putobj(lang_out,EndItem());

    // finals
    {
     ListPrint final_out(&lang_out);

     ulen atom_count=clang.getAtomCount();

     ulen index=0;

     for(auto &final : compress.getProps() )
       {
        Printf(final_out,"{ #; ,#;",index++,AutoIndent());

        ListPrint action_out(&final_out);

        if( final.hasNull() )
          {
           Printf(action_out,"{ null , null }#;",EndItem());
          }
        else
          {
           auto &alpha=final.getAlpha();

           if( alpha.nonEmpty() )
             {
              Printf(action_out,"{ null , lang.rules+#; }#;",alpha.getPtr()->getIndex()-atom_count,EndItem());
             }
          }

        for(auto &rec : Range(final.getBeta()) )
          {
           if( rec.object.shift )
             {
              Printf(action_out,"{ lang.atoms+#; , null }#;",rec.index.getIndex(),EndItem());
             }
           else
             {
              auto &rules=rec.object.rules;

              if( rules.nonEmpty() )
                {
                 Printf(action_out,"{ lang.atoms+#; , lang.rules+#; }#;",rec.index.getIndex(),rules.getPtr()->getIndex()-atom_count,EndItem());
                }
             }
          }

        Putobj(action_out,EndList());

        Putobj(final_out,"\n}",EndItem());
       }

     Putobj(final_out,EndList());
    }

    Putobj(lang_out,EndItem(),EndList());
   }

   Putobj(out,";\n\n");

   out.preserveFile();
  }

  TrackStage("Finish");
 }

} // namespace App

