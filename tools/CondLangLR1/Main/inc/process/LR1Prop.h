/* LR1Prop.h */
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

#ifndef CondLangLR1_process_LR1Prop_h
#define CondLangLR1_process_LR1Prop_h

#include "LR1Estimate.h"

namespace App {

/* classes */

class LR1PropNonEmpty;

class LR1PropShiftSet;

class LR1PropValidSet;

class LR1PropRuleSet;

/* class LR1PropNonEmpty */

class LR1PropNonEmpty
 {
   bool non_empty;

  public:

   // constructors

   explicit LR1PropNonEmpty(const LR1Estimate &est) : non_empty(+est) {}

   // properties

   bool operator + () const { return non_empty; }

   bool operator ! () const { return !non_empty; }

   // cmp objects

   bool operator == (LR1PropNonEmpty obj) const noexcept { return non_empty==obj.non_empty; }

   CmpResult operator <=> (LR1PropNonEmpty obj) const noexcept { return Cmp(non_empty,obj.non_empty); }

   // print object

   using PrintOptType = BlockPrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     if( non_empty )
       Putobj(out,"non-empty");
     else
       Putobj(out,"empty");

     if( opt.flag ) Putch(out,'\n');
    }
 };

/* class LR1PropShiftSet */

class LR1PropShiftSet
 {
  public:

   using AtomSet = Set<Atom,SimpleJoiner> ;

  private:

   bool end;
   AtomSet shift_set;

  public:

   // constructors

   explicit LR1PropShiftSet(const LR1Estimate &est)
    {
     if( !est )
       {
        end=false;
       }
     else
       {
        end=est.hasNull();

        Collector<Atom> collector;

        for(auto &rec : Range(est.getBeta()) )
          if( rec.object.shift )
            collector.append_copy(rec.index);

        shift_set=AtomSet(Range_const(collector.flat()));
       }
    }

   // properties

   bool operator + () const { return end || shift_set.nonEmpty(); }

   bool operator ! () const { return !end && shift_set.isEmpty(); }

   // cmp objects

   bool operator == (const LR1PropShiftSet &obj) const noexcept
    {
     return end==obj.end && shift_set==obj.shift_set ;
    }

   CmpResult operator <=> (const LR1PropShiftSet &obj) const noexcept
    {
     return AlphaCmp(end,obj.end,shift_set,obj.shift_set);
    }

   // print object

   using PrintOptType = BlockPrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     if( end )
       {
        Putobj(out,"(END)");

        if( shift_set.nonEmpty() ) Printf(out," #;",shift_set);
       }
     else
       {
        Putobj(out,shift_set);
       }

     if( opt.flag ) Putch(out,'\n');
    }
 };

/* class LR1PropValidSet */

class LR1PropValidSet
 {
  public:

   using AtomSet = Set<Atom,SimpleJoiner> ;

  private:

   bool end;
   AtomSet valid_set;

  public:

   // constructors

   explicit LR1PropValidSet(const LR1Estimate &est)
    {
     if( !est )
       {
        end=false;
       }
     else
       {
        end=est.getAlpha().nonEmpty();

        Collector<Atom> collector;

        for(auto &rec : Range(est.getBeta()) )
          if( rec.object.rules.nonEmpty() )
            collector.append_copy(rec.index);

        valid_set=AtomSet(Range_const(collector.flat()));
       }
    }

   // properties

   bool operator + () const { return end || valid_set.nonEmpty(); }

   bool operator ! () const { return !end && valid_set.isEmpty(); }

   // cmp objects

   bool operator == (const LR1PropValidSet &obj) const noexcept
    {
     return end==obj.end && valid_set==obj.valid_set ;
    }

   CmpResult operator <=> (const LR1PropValidSet &obj) const noexcept
    {
     return AlphaCmp(end,obj.end,valid_set,obj.valid_set);
    }

   // print object

   using PrintOptType = BlockPrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     if( end )
       {
        Putobj(out,"(END)");

        if( valid_set.nonEmpty() ) Printf(out," #;",valid_set);
       }
     else
       {
        Putobj(out,valid_set);
       }

     if( opt.flag ) Putch(out,'\n');
    }
 };

/* class LR1PropRuleSet */

class LR1PropRuleSet
 {
  public:

   using RuleSet = Set<Atom,SimpleJoiner> ;

  private:

   RuleSet rule_set;

  public:

   // constructors

   explicit LR1PropRuleSet(const LR1Estimate &est)
    {
     if( +est )
       {
        RuleSet::Accumulator acc;

        acc+=est.getAlpha();

        for(auto &rec : Range(est.getBeta()) ) acc+=rec.object.rules;

        rule_set=acc;
       }
    }

   // properties

   bool operator + () const { return rule_set.nonEmpty(); }

   bool operator ! () const { return rule_set.isEmpty(); }

   // cmp objects

   bool operator == (const LR1PropRuleSet &obj) const noexcept { return rule_set == obj.rule_set ; }

   CmpResult operator <=> (const LR1PropRuleSet &obj) const noexcept { return rule_set <=> obj.rule_set ; }

   // print object

   using PrintOptType = BlockPrintOpt ;

   void print(PrinterType auto &out,PrintOptType opt) const
    {
     Putobj(out,rule_set);

     if( opt.flag ) Putch(out,'\n');
    }
 };

} // namespace App

#endif
