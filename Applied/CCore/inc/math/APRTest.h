/* APRTest.h */
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

#ifndef CCore_inc_math_APRTest_h
#define CCore_inc_math_APRTest_h

#include <CCore/inc/math/ModEngine.h>
#include <CCore/inc/math/SmallPrimes.h>

#include <CCore/inc/Cmp.h>
#include <CCore/inc/FunctorType.h>
#include <CCore/inc/PrintSet.h>
#include <CCore/inc/Job.h>
#include <CCore/inc/Exception.h>

namespace CCore {
namespace Math {
namespace APRTest {

/* types */

using QType = uint64 ;

/* functions */

QType MulGuarded(QType a,QType b);

QType AddGuarded(QType a,QType b);

void GuardNoMemory();

/* classes */

//enum TestResult;

class SmallPrimesSet;

class QModEngine;

class PrimeQ;

class Jacobi;

class AltJacobi;

class JacobiSumTable;

class PrimeP;

class DataGen;

struct TestSet;

class TestData;

class NoReport;

template <class Integer> class TestEngine;

template <class Integer,template <class T,class F=GetNoThrowFlags<T> > class ArrayAlgo> class ParaTestEngine;

/* enum TestResult */

enum TestResult
 {
  IsPrime = 0,

  NoPrime,
  HasDivisor,
  HardCase,
  TooLarge,

  ExFlag
 };

const char * GetTextDesc(TestResult res);

/* class SmallPrimesSet */

class SmallPrimesSet // odd subsequent SmallPrimes 3, 5, 7, ...
 {
   RefArray<bool> flags;

  private:

   void forPrimes(FuncArgType<unsigned> auto &func) const
    {
     auto r=Range(flags);

     for(ulen i=0; i<r.len ;i++) if( r[i] ) func( SmallPrimes[i+1] );
    }

  public:

   explicit SmallPrimesSet(ulen n) : flags(n) {}

   ~SmallPrimesSet() {}

   bool operator [] (ulen ind) const { return flags[ind]; }

   ulen getSetNumber() const;

   bool next();

   template <FuncInitArgType<unsigned> FuncInit>
   void forOddPrimes(FuncInit func_init) const
    {
     FunctorTypeOf<FuncInit> func(func_init);

     forPrimes(func);
    }

   template <FuncInitArgType<unsigned> FuncInit>
   void forAllPrimes(FuncInit func_init) const
    {
     FunctorTypeOf<FuncInit> func(func_init);

     func(2);

     forPrimes(func);
    }

   QType getPrimeQ() const;
 };

/* class QModEngine */

class QModEngine // para-usable
 {
   QType mod;

  public:

   explicit QModEngine(QType mod_) : mod(mod_) {}

   QType mul(QType a,QType b) const;

   QType sq(QType a) const { return mul(a,a); }

   QType pow(QType a,QType d) const;

   QType mac(QType s,QType a,QType b) const;

   QType add(QType a,QType b) const;

   QType sub(QType a,QType b) const;

   QType inv(QType a) const;
 };

/* class PrimeQ */

class PrimeQ
 {
   SmallPrimesSet spset;

   QType prime; // 1+2*(odd p from spset)
   ulen set_number;
   QType gen;

  public:

   explicit PrimeQ(const SmallPrimesSet &spset);

   ~PrimeQ() {}

   // props

   bool hasP(ulen ind) const { return ind==0 || spset[ind-1]; }

   QType getPrime() const { return prime; }

   ulen getSetNumber() const { return set_number; }

   QType getGen() const { return gen; }

   // cmp objects

   CmpResult operator <=> (const PrimeQ &obj) const noexcept
    {
     return AlphaCmp(set_number,obj.set_number,prime,obj.prime);
    }

   bool operator == (const PrimeQ &obj) const noexcept
    {
     return set_number == obj.set_number && prime == obj.prime ;
    }

   // methods

   bool test();

   // print object

   void print(PrinterType auto &out) const
    {
     Printf(out,"q = #; : 1 + 2",prime);

     spset.forOddPrimes( [&] (unsigned p) { Printf(out," * #;",p); } );

     Printf(out,"\nset_number = #;\ngen = #;\n",set_number,gen);
    }

   // print cpp

   void printCpp(PrinterType auto &out) const
    {
     Printf(out,"{ #;u , #;u }",prime,set_number);
    }
 };

/* class Jacobi */

class Jacobi : QModEngine
 {
   QType prime;
   ulen set_number;
   QType gen;

   DynArray<ulen> table;

  private:

   unsigned log_table(QType x,unsigned p) const // 0 < x < prime , para-usable
    {
     return unsigned( table[ (ulen)x-1 ]%p );
    }

   class LogPow;

   class AddLogReport;

   static void GuardLog();

   void addLog(unsigned p,FuncArgType<unsigned,unsigned> auto func) const; // para-usable

   class LogCountTable;

  public:

   Jacobi(QType prime,ulen set_number,QType gen);

   Jacobi(const PrimeQ &q) : Jacobi(q.getPrime(),q.getSetNumber(),q.getGen()) {}

   ~Jacobi() {}

   QType getPrime() const { return prime; }

   ulen getSetNumber() const { return set_number; }

   QType getGen() const { return gen; }

   void operator () (unsigned p,QType result[ /* (p-2)*p */ ]) const; // p > 2 , result is erased , para-usable
 };

/* class AltJacobi */

class AltJacobi : NoCopy
 {
   QType prime;
   ulen set_number;
   QType gen;

  private:

   class Table;

  public:

   AltJacobi(QType prime,ulen set_number,QType gen);

   AltJacobi(const PrimeQ &q) : AltJacobi(q.getPrime(),q.getSetNumber(),q.getGen()) {}

   ~AltJacobi() {}

   QType getPrime() const { return prime; }

   ulen getSetNumber() const { return set_number; }

   QType getGen() const { return gen; }

   void operator () (unsigned p,QType result[ /* (p-2)*p */ ]) const; // p > 2 , result is erased , para-usable
 };

/* class JacobiSumTable */

class JacobiSumTable
 {
   QType prime_q;
   ulen set_number;

   unsigned prime_p;
   RefArray<QType> table;

  public:

   JacobiSumTable(const AltJacobi &jac,unsigned prime_p);

   ~JacobiSumTable() {}

   ulen getSetNumber() const { return set_number; }

   // print object

   void print(PrinterType auto &out) const
    {
     Printf(out,"  q = #;\n  set_number = #;\n",prime_q,set_number);

     if( prime_p>2 )
       {
        auto r=Range(table);

        while( +r )
          {
           auto j=(r+=prime_p);

           Printf(out,"  #;\n",PrintSet(j));
          }
       }
    }

   // print cpp

   void printCpp(PrinterType auto &out) const
    {
     if( prime_p>2 )
       {
        Printf(out,"{ #;u , #;u , JTable_#;_#; }",prime_q,set_number,prime_p,prime_q);
       }
     else
       {
        Printf(out,"{ #;u , #;u , 0 }",prime_q,set_number);
       }
    }

   void printCpp_jtable(PrinterType auto &out) const
    {
     if( prime_p>2 )
       {
        Printf(out,"static const QType JTable_#;_#;[]=\n",prime_p,prime_q);

        Putobj(out," {");

        {
         auto r=Range(table);

         bool comma=false;

         while( +r )
           {
            auto line=(r+=prime_p);

            if( comma )
              {
               Putch(out,',');
              }
            else
              {
               comma=true;
              }

            Printf(out,"\n  #;u",*line);

            for(++line; +line ;++line) Printf(out,", #;u",*line);
           }
        }

        Putobj(out,"\n };\n\n");
       }
    }
 };

/* class PrimeP */

class PrimeP
 {
   ulen ind;
   unsigned prime;

   RefArray<JacobiSumTable> jset;

  public:

   explicit PrimeP(ulen ind);

   ~PrimeP() {}

   // props

   unsigned getPrime() const { return prime; }

   bool testSet() const;

   // methods

   void append(const AltJacobi &jac) { jset.append_fill(jac,prime); }

   // print object

   void print(PrinterType auto &out) const
    {
     Printf(out,"p = #;\n\n",prime);

     for(const JacobiSumTable &jtable : jset ) Printf(out,"#;\n-----\n\n",jtable);
    }

   // print cpp

   void printCpp(PrinterType auto &out) const
    {
     Printf(out,"{ #;u , Range(JSet_#;) }",prime,prime);
    }

   void printCpp_jset(PrinterType auto &out) const
    {
     for(auto r=Range(jset); +r ;++r)
       {
        r->printCpp_jtable(out);
       }

     Printf(out,"static const TestSet::JTable JSet_#;[]=\n",prime);

     Putobj(out," {");

     {
      auto r=Range(jset);

      if( +r )
        {
         Putobj(out,"\n  ");

         r->printCpp(out);

         for(++r; +r ;++r)
           {
            Putobj(out,",\n  ");

            r->printCpp(out);
           }
        }
     }

     Putobj(out,"\n };\n\n");
    }
 };

/* class DataGen */

class DataGen : NoCopy
 {
   DynArray<PrimeQ> qset; // odd
   DynArray<PrimeP> pset;

  private:

   class JobControl;

  public:

   explicit DataGen(ulen n);

   ~DataGen();

   template <class Integer>
   void getCap(Integer &ret) const
    {
     ret=2;

     for(const PrimeQ &q : qset ) ret*=q.getPrime();

     ret=ret.sq();
    }

   bool testSet() const;

   // print object

   void print(PrinterType auto &out) const
    {
     for(const PrimeQ &q : qset ) Printf(out,"#;\n------------\n\n",q);

     for(const PrimeP &p : pset ) Printf(out,"#;\n------------\n\n",p);
    }

   // print cpp

   void printCpp(PrinterType auto &out) const
    {
     Putobj(out,"/* APRTestData.gen.cpp */\n\n");

     Putobj(out,"#include <CCore/inc/math/APRTest.h>\n\n");

     Putobj(out,"namespace CCore {\n");
     Putobj(out,"namespace Math {\n");
     Putobj(out,"namespace APRTest {\n\n");

     // qset

     Putobj(out,"static const TestSet::PrimeQ QSet[]=\n");

     Putobj(out," {");

     {
      auto r=Range(qset);

      if( +r )
        {
         Putobj(out,"\n  ");

         r->printCpp(out);

         for(++r; +r ;++r)
           {
            Putobj(out,",\n  ");

            r->printCpp(out);
           }
        }
     }

     Putobj(out,"\n };\n\n");

     // jset

     for(auto r=Range(pset); +r ;++r)
       {
        r->printCpp_jset(out);
       }

     // pset

     Putobj(out,"static const TestSet PSet[]=\n");

     Putobj(out," {");

     {
      auto r=Range(pset);

      if( +r )
        {
         Putobj(out,"\n  ");

         r->printCpp(out);

         for(++r; +r ;++r)
           {
            Putobj(out,",\n  ");

            r->printCpp(out);
           }
        }
     }

     Putobj(out,"\n };\n\n");

     // TestData

     Putobj(out,"/* class TestData */\n\n");

     Putobj(out,"TestData::TestData()\n {\n");

     Putobj(out,"  pset=PSet;\n");
     Putobj(out,"  qset=Range(QSet);\n");

     Putobj(out," }\n\n");

     Putobj(out,"} // namespace APRTest\n");
     Putobj(out,"} // namespace Math\n");
     Putobj(out,"} // namespace CCore\n\n");
    }
 };

/* struct TestSet */

struct TestSet
 {
  unsigned prime_p;

  struct PrimeQ
   {
    QType prime_q;
    unsigned set_number;
   };

  struct JTable : PrimeQ
   {
    const QType *table; // [ (prime_p-2)*prime_p ]

    JTable(QType prime_q_,unsigned set_number_,const QType *table_)
     {
      prime_q=prime_q_;
      set_number=set_number_;
      table=table_;
     }
   };

  PtrLen<const JTable> jset;
 };

/* class TestData */

class TestData : NoCopy
 {
  protected:

   const TestSet *pset;                // MaxN+1
   PtrLen<const TestSet::PrimeQ> qset; // odd

  public:

   static constexpr unsigned MaxN = 10 ;

   TestData();
 };

/* class NoReport */

class NoReport
 {
  public:

   NoReport() {}

   template <class Integer>
   void start(const Integer &N) { Used(N); }

   void sanity(const char *msg) { Used(msg); }

   void isSmallPrime() {}

   void testP(unsigned prime_p) { Used(prime_p); }

   void testQ(unsigned prime_p,QType prime_q) { Used(prime_p); Used(prime_q); }

   template <class Integer>
   void cappa(PtrLen<const Integer> cappa,const Integer &Nminus1) { Used(cappa); Used(Nminus1); }

   template <class Integer>
   void cappa2(const Integer &cappa,const Integer &Nminus1) { Used(cappa); Used(Nminus1); }

   void startProbe() {}

   template <class Integer>
   void probe(const Integer &cnt) { Used(cnt); }

   template <class Integer>
   void div(const Integer &D) { Used(D); }

   void hard() {}

   void isPrime() {}

   void noPrime() {}
 };

/* class TestEngine<Integer> */

template <class Integer>
class TestEngine : TestData
 {
   Integer P[MaxN];
   Integer Q[MaxN];
   Integer Q2[MaxN];

  private:

   using Ring = RefArray<Integer> ;

   class Engine : NoCopy
    {
      ModEngine<Integer> engine;

      Integer Nminus1;

     private:

      Ring prepare(const QType *table,unsigned prime,unsigned k) const
       {
        table+=k*prime;

        Ring ret=Ring(prime);

        auto r=ret.modify();

        for(; +r ;++r,++table) *r=engine.prepare(*table);

        return ret;
       }

      Ring mul(const Ring &a,const Ring &b) const
       {
        auto ra=Range(a);
        auto rb=Range(b);

        ulen len=ra.len;
        Ring ret(len);

        auto res=ret.modify();

        for(ulen i=0; i<len ;i++)
          for(ulen j=0; j<len ;j++)
            {
             ulen k=i+j;

             if( k>=len ) k-=len;

             res[k]=engine.mac(res[k],ra[i],rb[j]);
            }

        return ret;
       }

      Ring mul(Ring a,QType b) const
       {
        auto r=a.modify();
        auto b_=engine.prepare(b);

        for(; +r ;++r) *r=engine.mul(*r,b_);

        return a;
       }

      Ring one(ulen len) const
       {
        Ring ret=Ring(len);

        ret.modify()[0]=1;

        return ret;
       }

      Ring pow(const Ring &a,const Integer &d) const
       {
        IntegerBitScanner<Integer> scanner(d);

        if( +scanner )
          {
           Ring ret=a;

           for(++scanner; +scanner ;++scanner)
             {
              ret=mul(ret,ret);

              if( *scanner ) ret=mul(ret,a);
             }

           return ret;
          }

        return one(a.getLen());
       }

      int test(const Integer &a,const Integer &b) const // 0, 1, other=2 : a-b mod N
       {
        switch( Cmp(a,b) )
          {
           case CmpLess :
            {
             if( !a && b==Nminus1 ) return 1;

             return 2;
            }
           break;

           case CmpGreater :
            {
             if( a-b==1 ) return 1;

             return 2;
            }
           break;

           default: return 0;
          }
       }

      TestResult test(const Ring &cappa) const
       {
        auto r=Range(cappa);

        Integer a=r[0];
        Integer b=r[1];

        if( test(a,b)==1 )
          {
           for(r+=2; +r ;++r) if( *r!=b ) return NoPrime;

           return HardCase;
          }
        else
          {
           bool flag=true;

           for(++r; +r ;++r)
             {
              if( flag )
                {
                 switch( test(*r,a) )
                   {
                    case 0 : break;

                    case 1 : flag=false; break;

                    case 2 : return NoPrime;
                   }
                }
              else
                {
                 if( *r!=a ) return NoPrime;
                }
             }

           return flag?NoPrime:IsPrime;
          }
       }

      static TestResult Test(int cappa,int sign)
       {
        if( cappa==sign ) return HardCase;

        if( cappa==-sign ) return IsPrime;

        return NoPrime;
       }

      TestResult test(const Integer &cappa,int sign) const
       {
        if( cappa==1 ) return Test(1,sign);

        if( cappa==Nminus1 ) return Test(-1,sign);

        return NoPrime;
       }

     public:

      explicit Engine(const Integer &N) : engine(N),Nminus1(N-1) {}

      ~Engine() {}

      template <class Report>
      TestResult test(unsigned prime,const TestSet::JTable &jtable,Report &report) const
       {
        if( prime>2 )
          {
           DynArray<Ring> theta(prime-2);

           theta[0]=prepare(jtable.table,prime,0);

           for(unsigned k=1; k<prime-2 ;k++) theta[k]=mul(theta[k-1],prepare(jtable.table,prime,k));

           Ring omega=mul(theta[prime-3],jtable.prime_q);

           auto res=engine.getModule().divmod(prime);

           unsigned mod=res.mod.template cast<unsigned>();

           Ring cappa=pow(omega,res.div);

           if( mod>1 ) cappa=mul(cappa,theta[mod-2]);

           report.cappa(Range(cappa),Nminus1);

           return test(cappa);
          }
        else
          {
           Integer omega=engine.prepare(jtable.prime_q);
           Integer d=Nminus1>>1;

           Integer cappa=engine.pow(omega,d);

           report.cappa2(cappa,Nminus1);

           if( d.isEven() )
             {
              return test(cappa,1);
             }
           else
             {
              return test(cappa,-1);
             }
          }
       }

      template <class Report>
      TestResult test(unsigned set_number,const TestSet &tset,Report &report) const
       {
        if( tset.jset.len==0 || tset.jset[0].set_number>set_number ) return IsPrime;

        TestResult ret=HardCase;

        report.testP(tset.prime_p);

        for(ulen k=0; k<tset.jset.len && tset.jset[k].set_number<=set_number ;k++)
          {
           report.testQ(tset.prime_p,tset.jset[k].prime_q);

           switch( test(tset.prime_p,tset.jset[k],report) )
             {
              case IsPrime : ret=IsPrime; break;

              case NoPrime : report.noPrime(); return NoPrime;
             }
          }

        return ret;
       }
    };

  private:

   template <class Report>
   TestResult sanity(const Integer &N,unsigned &set_number,Report &report) const
    {
     if( N<=1 )
       {
        report.sanity("N <= 1 NoPrime");

        return NoPrime;
       }

     unsigned n=MaxN-1;

     if( N>=Q2[n] )
       {
        report.sanity("N >= Q*Q TooLarge");

        return TooLarge;
       }

     while( n>0 && N<Q2[n-1] ) n--;

     set_number=n;

     Integer D=GCDiv(N,P[n]*Q[n]);

     if( D>1 )
       {
        for(unsigned i=0; i<n+2 ;i++)
          {
           if( N==pset[i].prime_p )
             {
              report.isSmallPrime();

              return IsPrime;
             }
          }

        for(auto r=qset; +r && r->set_number<=n ;++r)
          {
           if( N==r->prime_q )
             {
              report.isSmallPrime();

              return IsPrime;
             }
          }

        report.sanity("HasDivisor from P or Q");

        return HasDivisor;
       }

     return HardCase;
    }

   static bool TestOrder(unsigned p,const Integer &N)
    {
     Integer M=p;

     ModEngine<Integer> engine(M.sq());

     return engine.template pow<unsigned>(engine.prepare(N),p-1)!=1;
    }

   template <class Report>
   TestResult finish(const Integer &N,unsigned set_number,Report &report) const
    {
     report.startProbe();

     ModEngine<Integer> engine(Q[set_number]);

     Integer D1=engine.prepare(N);
     Integer D=D1;

     for(Integer cnt=P[set_number]-1; cnt>0 ;cnt-=1,D=engine.mul(D,D1))
       {
        report.probe(cnt);

        if( D>1 && D<N && !(N%D) )
          {
           report.div(D);

           return HasDivisor;
          }
       }

     report.isPrime();

     return IsPrime;
    }

  public:

   TestEngine()
    {
     // 1
     {
      Integer prod=pset[0].prime_p;

      for(unsigned n=0; n<MaxN ;n++)
        {
         prod*=pset[n+1].prime_p;

         P[n]=prod;
        }
     }

     // 2
     {
      Integer prod=2;
      auto r=qset;

      for(unsigned n=0; n<MaxN ;n++)
        {
         for(; +r && r->set_number<=n ;++r) prod*=r->prime_q;

         Q[n]=prod;
         Q2[n]=prod.sq();
        }
     }
    }

   ~TestEngine() {}

   template <class Report>
   TestResult operator () (Integer N,Report &report) const
    {
     report.start(N);

     unsigned set_number=0;

     {
      TestResult ret=sanity(N,set_number,report);

      if( ret!=HardCase ) return ret;
     }

     DynArray<bool> flags(set_number+2);
     Engine engine(N);

     for(unsigned i=0; i<set_number+2 ;i++)
       {
        TestResult ret=engine.test(set_number,pset[i],report);

        if( ret==NoPrime ) return NoPrime;

        if( ret==IsPrime ) flags[i]=true;
       }

     for(unsigned i=0; i<set_number+2 ;i++)
       {
        if( !flags[i] )
          {
           if( !TestOrder(pset[i].prime_p,N) )
             {
              report.hard();

              return HardCase;
             }
          }
       }

     return finish(N,set_number,report);
    }
 };

/* class ParaTestEngine<Integer,ArrayAlgo> */

template <class Integer,template <class T,class F=GetNoThrowFlags<T> > class ArrayAlgo>
class ParaTestEngine : TestData
 {
   Integer P[MaxN];
   Integer Q[MaxN];
   Integer Q2[MaxN];

  private:

   using Ring = RefArray<Integer,ArrayAlgo<Integer> > ;

   class Engine : NoCopy
    {
      ModEngine<Integer> engine;

      Integer Nminus1;

     private:

      Ring prepare(const QType *table,unsigned prime,unsigned k) const
       {
        table+=k*prime;

        Ring ret=Ring(prime);

        auto r=ret.modify();

        for(; +r ;++r,++table) *r=engine.prepare(*table);

        return ret;
       }

      Ring mul(const Ring &a,const Ring &b) const
       {
        auto ra=Range(a);
        auto rb=Range(b);

        ulen len=ra.len;
        Ring ret(len);

        auto res=ret.modify();

        for(ulen i=0; i<len ;i++)
          for(ulen j=0; j<len ;j++)
            {
             ulen k=i+j;

             if( k>=len ) k-=len;

             res[k]=engine.mac(res[k],ra[i],rb[j]);
            }

        return ret;
       }

      Ring mul(Ring a,QType b) const
       {
        auto r=a.modify();
        auto b_=engine.prepare(b);

        for(; +r ;++r) *r=engine.mul(*r,b_);

        return a;
       }

      Ring one(ulen len) const
       {
        Ring ret=Ring(len);

        ret.modify()[0]=1;

        return ret;
       }

      Ring pow(const Ring &a,const Integer &d) const
       {
        IntegerBitScanner<Integer> scanner(d);

        if( +scanner )
          {
           Ring ret=a;

           for(++scanner; +scanner ;++scanner)
             {
              ret=mul(ret,ret);

              if( *scanner ) ret=mul(ret,a);
             }

           return ret;
          }

        return one(a.getLen());
       }

      int test(const Integer &a,const Integer &b) const // 0, 1, other=2 : a-b mod N
       {
        switch( Cmp(a,b) )
          {
           case CmpLess :
            {
             if( !a && b==Nminus1 ) return 1;

             return 2;
            }
           break;

           case CmpGreater :
            {
             if( a-b==1 ) return 1;

             return 2;
            }
           break;

           default: return 0;
          }
       }

      TestResult test(const Ring &cappa) const
       {
        auto r=Range(cappa);

        Integer a=r[0];
        Integer b=r[1];

        if( test(a,b)==1 )
          {
           for(r+=2; +r ;++r) if( *r!=b ) return NoPrime;

           return HardCase;
          }
        else
          {
           bool flag=true;

           for(++r; +r ;++r)
             {
              if( flag )
                {
                 switch( test(*r,a) )
                   {
                    case 0 : break;

                    case 1 : flag=false; break;

                    case 2 : return NoPrime;
                   }
                }
              else
                {
                 if( *r!=a ) return NoPrime;
                }
             }

           return flag?NoPrime:IsPrime;
          }
       }

      static TestResult Test(int cappa,int sign)
       {
        if( cappa==sign ) return HardCase;

        if( cappa==-sign ) return IsPrime;

        return NoPrime;
       }

      TestResult test(const Integer &cappa,int sign) const
       {
        if( cappa==1 ) return Test(1,sign);

        if( cappa==Nminus1 ) return Test(-1,sign);

        return NoPrime;
       }

     public:

      explicit Engine(const Integer &N) : engine(N),Nminus1(N-1) {}

      ~Engine() {}

      template <class Report>
      TestResult test(unsigned prime,const TestSet::JTable &jtable,Report &report) const
       {
        if( prime>2 )
          {
           DynArray<Ring> theta(prime-2);

           theta[0]=prepare(jtable.table,prime,0);

           for(unsigned k=1; k<prime-2 ;k++) theta[k]=mul(theta[k-1],prepare(jtable.table,prime,k));

           Ring omega=mul(theta[prime-3],jtable.prime_q);

           auto res=engine.getModule().divmod(prime);

           unsigned mod=res.mod.template cast<unsigned>();

           Ring cappa=pow(omega,res.div);

           if( mod>1 ) cappa=mul(cappa,theta[mod-2]);

           report.cappa(Range(cappa),Nminus1);

           return test(cappa);
          }
        else
          {
           Integer omega=engine.prepare(jtable.prime_q);
           Integer d=Nminus1>>1;

           Integer cappa=engine.pow(omega,d);

           report.cappa2(cappa,Nminus1);

           if( d.isEven() )
             {
              return test(cappa,1);
             }
           else
             {
              return test(cappa,-1);
             }
          }
       }

      template <class Report>
      TestResult test(unsigned set_number,const TestSet &tset,Report &report) const
       {
        if( tset.jset.len==0 || tset.jset[0].set_number>set_number ) return IsPrime;

        TestResult ret=HardCase;

        report.testP(tset.prime_p);

        for(ulen k=0; k<tset.jset.len && tset.jset[k].set_number<=set_number ;k++)
          {
           report.testQ(tset.prime_p,tset.jset[k].prime_q);

           switch( test(tset.prime_p,tset.jset[k],report) )
             {
              case IsPrime : ret=IsPrime; break;

              case NoPrime : report.noPrime(); return NoPrime;
             }
          }

        return ret;
       }
    };

  private:

   template <class Report>
   TestResult sanity(const Integer &N,unsigned &set_number,Report &report) const
    {
     if( N<=1 )
       {
        report.sanity("N <= 1 NoPrime");

        return NoPrime;
       }

     unsigned n=MaxN-1;

     if( N>=Q2[n] )
       {
        report.sanity("N >= Q*Q TooLarge");

        return TooLarge;
       }

     while( n>0 && N<Q2[n-1] ) n--;

     set_number=n;

     Integer D=GCDiv(N,P[n]*Q[n]);

     if( D>1 )
       {
        for(unsigned i=0; i<n+2 ;i++)
          {
           if( N==pset[i].prime_p )
             {
              report.isSmallPrime();

              return IsPrime;
             }
          }

        for(auto r=qset; +r && r->set_number<=n ;++r)
          {
           if( N==r->prime_q )
             {
              report.isSmallPrime();

              return IsPrime;
             }
          }

        report.sanity("HasDivisor from P or Q");

        return HasDivisor;
       }

     return HardCase;
    }

   static bool TestOrder(unsigned p,const Integer &N)
    {
     Integer M=p;

     ModEngine<Integer> engine(M.sq());

     return engine.template pow<unsigned>(engine.prepare(N),p-1)!=1;
    }

  private:

   template <class Report>
   class FinishJobControl : public Funchor_nocopy
    {
      Integer N;
      Integer Q;
      Integer D1;
      Report &report;

      struct Rec
       {
        Integer D;
        Integer cnt;

        void set(const Integer &D_,const Integer &cnt_)
         {
          D=D_;
          cnt=cnt_;
         }

        void cloneTo(Integer &D_,Integer &cnt_) const
         {
          D.cloneTo(D_);
          cnt.cloneTo(cnt_);
         }
       };

      DynArray<Rec> list;

      Mutex mutex;

      ulen cur = 0 ;

      TestResult result = IsPrime ;
      bool ex_flag = false ;

     private:

      static constexpr ulen ListLen = 1000 ;

      void job()
       {
        SilentReportException exreport;

        try
          {
           Integer N_;
           Integer Q_;
           Integer D1_;

           N.cloneTo(N_);
           Q.cloneTo(Q_);
           D1.cloneTo(D1_);

           ModEngine<Integer> engine(Q_);

           for(ulen i;;)
             {
              {
               Mutex::Lock lock(mutex);

               if( ex_flag || result!=IsPrime || cur>=ListLen ) return;

               i=cur++;
              }

              Integer D;
              Integer cnt;

              list[i].cloneTo(D,cnt);

              for(; cnt>0 ;cnt-=1,D=engine.mul(D,D1_))
                {
                 report.probe(cnt);

                 if( D>1 && D<N_ && !(N_%D) )
                   {
                    report.div(D);

                    Mutex::Lock lock(mutex);

                    result=HasDivisor;

                    return;
                   }
                }
             }
          }
        catch(...)
          {
           Mutex::Lock lock(mutex);

           ex_flag=true;
          }
       }

     public:

      FinishJobControl(const Integer &N_,const Integer &P_,const Integer &Q_,Report &report_)
       : N(N_),
         Q(Q_),
         D1(N_%Q_),
         report(report_),
         list(ListLen)
       {
        ModEngine<Integer> engine(Q_);

        Integer D=D1;
        Integer cnt=P_-1;

        Integer delta=cnt/ListLen;

        Integer E=engine.pow(D1,delta);

        for(ulen i=0; i<ListLen-1 ;i++)
          {
           list[i].set(D,delta);

           D=engine.mul(D,E);
          }

        list[ListLen-1].set(D,cnt-(ListLen-1)*delta);
       }

      ~FinishJobControl() {}

      Function<void (void)> function_job() { return FunctionOf(this,&FinishJobControl::job); }

      TestResult getResult() const { return result; }

      bool getExFlag() const { return ex_flag; }
    };

   template <class Report>
   TestResult finish(const Integer &N,unsigned set_number,Report &report) const
    {
     report.startProbe();

     FinishJobControl<Report> control(N,P[set_number],Q[set_number],report);

     {
      Job run_job(control.function_job());
     }

     if( control.getExFlag() ) return ExFlag;

     TestResult ret=control.getResult();

     if( ret==IsPrime ) report.isPrime();

     return ret;
    }

  private:

   template <class Report>
   class JobControl : public Funchor_nocopy
    {
      const TestSet *pset;
      Integer N;
      bool *flags;
      unsigned set_number;
      Report &report;

      Mutex mutex;

      unsigned cur = 1 ;
      bool no_prime = false ;
      bool ex_flag = false ;

     private:

      void job()
       {
        SilentReportException exreport;

        try
          {
           Integer N_;

           N.cloneTo(N_);

           Engine engine(N_);

           for(unsigned i;;)
             {
              {
               Mutex::Lock lock(mutex);

               if( no_prime || ex_flag || cur>=set_number+2 ) return;

               i=cur++;
              }

              TestResult ret=engine.test(set_number,pset[i],report);

              if( ret==NoPrime )
                {
                 Mutex::Lock lock(mutex);

                 no_prime=true;

                 return;
                }

              if( ret==IsPrime ) flags[i]=true;
             }
          }
        catch(...)
          {
           Mutex::Lock lock(mutex);

           ex_flag=true;
          }
       }

     public:

      JobControl(const TestSet *pset_,const Integer &N_,bool *flags_,unsigned set_number_,Report &report_)
       : pset(pset_),
         N(N_),
         flags(flags_),
         set_number(set_number_),
         report(report_)
       {
       }

      ~JobControl() {}

      Function<void (void)> function_job() { return FunctionOf(this,&JobControl::job); }

      bool getNoPrime() const { return no_prime; }

      bool getExFlag() const { return ex_flag; }
    };

  public:

   ParaTestEngine()
    {
     // 1
     {
      Integer prod=pset[0].prime_p;

      for(unsigned n=0; n<MaxN ;n++)
        {
         prod*=pset[n+1].prime_p;

         P[n]=prod;
        }
     }

     // 2
     {
      Integer prod=2;
      auto r=qset;

      for(unsigned n=0; n<MaxN ;n++)
        {
         for(; +r && r->set_number<=n ;++r) prod*=r->prime_q;

         Q[n]=prod;
         Q2[n]=prod.sq();
        }
     }
    }

   ~ParaTestEngine() {}

   template <class Report>
   TestResult operator () (Integer N,Report &report) const
    {
     report.start(N);

     unsigned set_number=0;

     {
      TestResult ret=sanity(N,set_number,report);

      if( ret!=HardCase ) return ret;
     }

     DynArray<bool> flags(set_number+2);
     Engine engine(N);

     {
      TestResult ret=engine.test(set_number,pset[0],report);

      if( ret==NoPrime ) return NoPrime;

      if( ret==IsPrime ) flags[0]=true;
     }

     {
      JobControl<Report> control(pset,N,flags.getPtr(),set_number,report);

      {
       Job run_job(control.function_job());
      }

      if( control.getExFlag() ) return ExFlag;

      if( control.getNoPrime() ) return NoPrime;
     }

     for(unsigned i=0; i<set_number+2 ;i++)
       {
        if( !flags[i] )
          {
           if( !TestOrder(pset[i].prime_p,N) )
             {
              report.hard();

              return HardCase;
             }
          }
       }

     return finish(N,set_number,report);
    }
 };

} // namespace APRTest
} // namespace Math
} // namespace CCore

#endif


