/* test2053.StepEval.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/Array.h>
#include <CCore/inc/AnyPtr.h>
#include <CCore/inc/StepEval.h>
#include <CCore/inc/ElementPool.h>
#include <CCore/inc/PlatformRandom.h>

namespace App {

namespace Private_2053 {

/* TOut */

//const auto Trace = NoPrint ;
const auto Trace = Con ;

/* classes */

class Context;

struct Expr;

/* class Context */

class Context : NoCopy
 {
   PlatformRandom random;
   ElementPool pool;

   class ContextPtr
    {
      Context *ctx;

     public:

      explicit ContextPtr(Context *ctx_) : ctx(ctx_) {}

      Context * operator -> () const { return ctx; }

      ContextPtr getAlloc() { return *this; }

      using AllocType = ContextPtr ;

      void * alloc(ulen len) { return ctx->alloc(len); }

      void free(void *ptr,ulen len) noexcept { ctx->free(ptr,len); }
    };

   using Gate = StepEvalGate<ContextPtr> ;

   struct Var : NoCopy
    {
     int value;
     Expr *expr;
     Gate *gate;

     Var() noexcept : value(0),expr(0),gate(0) {}
    };

   SimpleArray<Var> table;

  private:

   using Eval = StepEval<ContextPtr> ;

   using StepId = StepEvalStepId ;

   struct NegStep;

   struct AddStep;

   struct MulStep;

   struct GetVarStep;

   struct ExprStep;

   struct OpenGateStep;

   struct GateFinalStep;

  private:

#if 1

   MemPool step_pool;

   void * alloc(ulen len)
    {
     void *mem=step_pool.alloc(len);

     //Printf(Trace,"alloc(#;) -> #;\n",len,(uptr)mem);

     return mem;
    }

   void free(void *mem,ulen len) noexcept
    {
     Used(mem);
     Used(len);

     //Printf(Trace,"free(#;,#;)\n",(uptr)mem,len);
    }

#else

   void * alloc(ulen len)
    {
     return MemAlloc(len);
    }

   void free(void *ptr,ulen) noexcept
    {
     MemFree(ptr);
    }

#endif

  public:

   ulen count;

   explicit Context(ulen len) : table(len),count(0) {}

   ~Context() {}

   template <class T,class ... SS>
   T * create(SS && ... ss)
    {
     return pool.create<T>( std::forward<SS>(ss)... );
    }

   Expr * generate(ulen var_lim,ulen cap);

   void generate(ulen cap);

   void eval();
 };

/* struct Expr */

struct Expr : NoCopy
 {
  struct Const
   {
    int value;

    explicit Const(int value_) : value(value_) {}

    void print(PrinterType auto &out) const
     {
      Putobj(out,value);
     }
   };

  struct Var
   {
    ulen index;

    explicit Var(ulen index_) : index(index_) {}

    void print(PrinterType auto &out) const
     {
      Printf(out,"[#;]",index);
     }
   };

  struct Neg
   {
    Expr *arg;

    explicit Neg(Expr *arg_) : arg(arg_) {}

    void print(PrinterType auto &out) const
     {
      Printf(out,"- #;",*arg);
     }
   };

  struct Add
   {
    Expr *arg1;
    Expr *arg2;

    Add(Expr *arg1_,Expr *arg2_) : arg1(arg1_),arg2(arg2_) {}

    void print(PrinterType auto &out) const
     {
      Printf(out,"( #; + #; )",*arg1,*arg2);
     }
   };

  struct Mul
   {
    Expr *arg1;
    Expr *arg2;

    Mul(Expr *arg1_,Expr *arg2_) : arg1(arg1_),arg2(arg2_) {}

    void print(PrinterType auto &out) const
     {
      Printf(out,"( #; * #; )",*arg1,*arg2);
     }
   };

  AnyPtr<Const,Var,Neg,Add,Mul> ptr;

  void init_const(Context &ctx,int value)
   {
    ptr=ctx.create<Const>(value);
   }

  void init_var(Context &ctx,ulen index)
   {
    ptr=ctx.create<Var>(index);
   }

  void init_neg(Context &ctx,Expr *arg)
   {
    ptr=ctx.create<Neg>(arg);
   }

  void init_add(Context &ctx,Expr *arg1,Expr *arg2)
   {
    ptr=ctx.create<Add>(arg1,arg2);
   }

  void init_mul(Context &ctx,Expr *arg1,Expr *arg2)
   {
    ptr=ctx.create<Mul>(arg1,arg2);
   }

  void print(PrinterType auto &out) const
   {
    Putobj(out,ptr);
   }
 };

/* Context::generate() */

Expr * Context::generate(ulen var_lim,ulen cap)
 {
  Expr *expr=create<Expr>();

  switch( random.select( cap? 5 : 2 ) )
    {
     case 0 :
      {
       if( var_lim )
         {
          ulen index=random.select_uint<ulen>(var_lim);

          expr->init_var(*this,index);

          break;
         }
      }
     [[fallthrough]];

     case 1 :
      {
       int value=(int)random.select(0,100)-50;

       expr->init_const(*this,value);
      }
     break;

     case 2 :
      {
       expr->init_neg(*this,generate(var_lim,cap-1));
      }
     break;

     case 3 :
      {
       expr->init_add(*this,generate(var_lim,cap-1),generate(var_lim,cap-1));
      }
     break;

     case 4 :
      {
       expr->init_mul(*this,generate(var_lim,cap-1),generate(var_lim,cap-1));
      }
     break;
    }

  return expr;
 }

void Context::generate(ulen cap)
 {
  ulen len=table.getLen();

  for(ulen index=0; index<len ;index++)
    {
     Expr *expr=generate(index,cap);

     table[index].expr=expr;

     Printf(Trace,"[#;] = #;\n",index,*expr);
    }
 }

/* Context::eval() */

struct Context::NegStep
 {
  int arg = 0 ;
  int &ret;

  explicit NegStep(int &ret_) : ret(ret_) {}

  void operator () (Eval &eval)
   {
    Printf(Trace,"- #;\n",arg);

    ret=-arg;

    eval->count++;
   }
 };

struct Context::AddStep
 {
  int arg1 = 0 ;
  int arg2 = 0 ;
  int &ret;

  explicit AddStep(int &ret_) : ret(ret_) {}

  void operator () (Eval &eval)
   {
    Printf(Trace,"#; + #;\n",arg1,arg2);

    ret=arg1+arg2;

    eval->count++;
   }
 };

struct Context::MulStep
 {
  int arg1 = 0 ;
  int arg2 = 0 ;
  int &ret;

  explicit MulStep(int &ret_) : ret(ret_) {}

  void operator () (Eval &eval)
   {
    Printf(Trace,"#; * #;\n",arg1,arg2);

    ret=arg1*arg2;

    eval->count++;
   }
 };

struct Context::GetVarStep
 {
  const int &var;
  int &ret;

  GetVarStep(const int &var_,int &ret_) : var(var_),ret(ret_) {}

  void operator () (Eval &eval)
   {
    Printf(Trace,"[] #;\n",var);

    ret=var;

    eval->count++;
   }
 };

struct Context::ExprStep
 {
  Expr *expr;
  int &ret;

  ExprStep(Expr *expr_,int &ret_) : expr(expr_),ret(ret_) {}

  void operator () (Eval &,StepId,Expr::Const *ptr)
   {
    Printf(Trace,"#;\n",ptr->value);

    ret=ptr->value;
   }

  void operator () (Eval &eval,StepId dep,Expr::Var *ptr)
   {
    ulen index=ptr->index;
    Var &var=eval->table[index];

#if 1

    var.gate->createStep(GetVarStep(var.value,ret),dep);

#else

    auto step=eval.createStep(GetVarStep(var.value,ret),dep);

    var.gate->delay(step.id);

#endif
   }

  void operator () (Eval &eval,StepId dep,Expr::Neg *ptr)
   {
    auto step=eval.createStep(NegStep(ret),dep);

    eval.createStep(ExprStep(ptr->arg,step.obj.arg),step.id);
   }

  void operator () (Eval &eval,StepId dep,Expr::Add *ptr)
   {
    auto step=eval.createStep(AddStep(ret),dep);

    eval.createStep(ExprStep(ptr->arg1,step.obj.arg1),step.id);
    eval.createStep(ExprStep(ptr->arg2,step.obj.arg2),step.id);
   }

  void operator () (Eval &eval,StepId dep,Expr::Mul *ptr)
   {
    auto step=eval.createStep(MulStep(ret),dep);

    eval.createStep(ExprStep(ptr->arg1,step.obj.arg1),step.id);
    eval.createStep(ExprStep(ptr->arg2,step.obj.arg2),step.id);
   }

  void operator () (Eval &eval,StepId dep)
   {
    ElaborateAnyPtr(*this,eval,dep,expr->ptr);

    eval->count++;
   }
 };

struct Context::OpenGateStep
 {
  Gate *gate;
  ulen index;

  OpenGateStep(Gate *gate_,ulen index_) : gate(gate_),index(index_) {}

  void operator () (Eval &eval)
   {
    Printf(Trace,"[#;]\n",index);

    gate->open();

    eval->count++;
   }

  void final(Eval &)
   {
    Printf(Con,"Cyclic variable dependency #;\n",index);
   }
 };

struct Context::GateFinalStep
 {
  ulen index;

  explicit GateFinalStep(ulen index_) : index(index_) {}

  void operator () (Eval &eval)
   {
    Printf(Trace,"[#;]\n",index);

    eval->count++;
   }

  void final(Eval &)
   {
    Printf(Con,"Cyclic variable dependency #;\n",index);
   }
 };

void Context::eval()
 {
  Eval step_eval(this);

  ulen len=table.getLen();

  DynArray<ulen> order(len);

  for(ulen index=0; index<len ;index++) order[index]=index;

  for(ulen index=0; index+1<len ;index++)
    {
     ulen i=random.select_uint<ulen>(index,len-1);

     if( i!=index ) Swap(order[index],order[i]);
    }

  for(ulen index=0; index<len ;index++)
    {
     ulen i=order[index];

     Var &var=table[i];

#if 0

     auto *gate=step_eval.createGate();

     var.gate=gate;

     auto step=step_eval.createStep(OpenGateStep(gate,i));

     step_eval.createStep(ExprStep(var.expr,var.value),step.id);

#else

     var.gate=step_eval.createGate(GateFinalStep(i),ExprStep(var.expr,var.value));

#endif
    }

  step_eval.run();
 }

} // namespace Private_2053

using namespace Private_2053;

/* Testit<2053> */

template<>
const char *const Testit<2053>::Name="Test2053 StepEval";

template<>
bool Testit<2053>::Main()
 {
#if 1

  Context ctx(100);

  ctx.generate(20);

  ctx.eval();

#else

  MSecTimer::ValueType time=0;
  ulen count=0;

  for(ulen rep=10'000; rep ;rep--)
    {
     Context ctx(100);

     ctx.generate(20);

     MSecTimer timer;

     ctx.eval();

     time+=timer.get();
     count+=ctx.count;
    }

  Printf(Con,"time = #; msec count= #;\n",time,count);

#endif

  return true;
 }

} // namespace App

