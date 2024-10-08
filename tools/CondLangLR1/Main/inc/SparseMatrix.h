/* SparseMatrix.h */
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

#ifndef CondLangLR1_SparseMatrix_h
#define CondLangLR1_SparseMatrix_h

#include "Set.h"

#include <CCore/inc/algon/BinarySearch.h>
#include <CCore/inc/algon/SortUnique.h>

namespace App {

/* classes */

template <class I,class T> class SparseVector;

template <class I,class T,class Row,class Cell> class VectorMulBuilder;

template <class I,class T,class Row,class Cell> class MatrixMulBuilder;

template <class I,class T> struct MatrixPosition;

template <class I,class T> class SparseMatrix;

/* class SparseVector<I,T> */

template <class I,class T>
SparseVector<I,T> operator * (const SparseMatrix<I,T> &a,const SparseVector<I,T> &b);

template <class I,class T>
class SparseVector
 {
  public:

   using Cell = IndexPair<I,T> ;

  private:

   Set<Cell,Joiner> cellset;

  private:

   explicit SparseVector(const Set<Cell,Joiner> &cellset_) : cellset(cellset_) {}

   template <class Builder>
   SparseVector(DoBuildType,Builder builder) : cellset(DoBuild,builder) {}

   friend SparseVector<I,T> operator * (const SparseMatrix<I,T> &a,const SparseVector<I,T> &b);

  public:

   // constructors

   SparseVector() noexcept {}

   ~SparseVector() {}

   // unsafe constructors

   explicit SparseVector(PtrLen<const Cell> range) : cellset(range) {} // strictly weak ordered input

   // methods

   T operator [] (I index) const
    {
     auto range=Range(cellset);

     Algon::BinarySearch_if(range, [=] (const Cell &obj) { return index<=obj.index; } );

     if( +range && index==range->index ) return range->object;

     return T();
    }

   // cmp objects

   bool operator == (const SparseVector<I,T> &obj) const noexcept { return cellset == obj.cellset ; }

   CmpResult operator <=> (const SparseVector<I,T> &obj) const noexcept { return cellset <=> obj.cellset ; }

   // operators

   friend SparseVector<I,T> operator + (const SparseVector<I,T> &a,const SparseVector<I,T> &b)
    {
     return SparseVector<I,T>(a.cellset+b.cellset);
    }

   friend SparseVector<I,T> operator * <> (const SparseMatrix<I,T> &a,const SparseVector<I,T> &b);

   // print object

   void print(PrinterType auto &out) const
    {
     for(auto &cell : Range(cellset) ) Printf(out,"#;\n",cell);

     Printf(out,"-----\n");
    }
 };

/* class VectorMulBuilder<I,T,Row,Cell> */

template <class I,class T,class Row,class Cell>
class VectorMulBuilder
 {
   PtrLen<const Row> a;
   PtrLen<const Cell> b;

  private:

   static T Prod(PtrLen<const Cell> a,PtrLen<const Cell> b);

  public:

   VectorMulBuilder(PtrLen<const Row> a_,PtrLen<const Cell> b_) : a(a_),b(b_) {}

   ulen getLen() const { return a.len; }

   PtrLen<Cell> operator () (Place<void> place) const;
 };

template <class I,class T,class Row,class Cell>
T VectorMulBuilder<I,T,Row,Cell>::Prod(PtrLen<const Cell> a,PtrLen<const Cell> b)
 {
  typename T::Accumulator ret;

  while( +a && +b )
    switch( (*a).weakCmp(*b) )
      {
       case CmpLess :
        {
         ++a;
        }
       break;

       case CmpGreater :
        {
         ++b;
        }
       break;

       default: // case CmpEqual
        {
         ret+=(a->object)*(b->object);

         ++a;
         ++b;
        }
      }

  return ret;
 }

template <class I,class T,class Row,class Cell>
PtrLen<Cell> VectorMulBuilder<I,T,Row,Cell>::operator () (Place<void> place) const
 {
  typename ArrayAlgo<Cell>::BuildGuard guard(place);

  for(auto p=a; +p ;++p)
    {
     T prod=Prod(Range(p->object),b);

     if( +prod )
       {
        new(guard.at()) Cell(p->index,prod);

        ++guard;
       }
    }

  return guard.disarm();
 }

/* class MatrixMulBuilder<I,T,Row,Cell> */

template <class I,class T,class Row,class Cell>
class MatrixMulBuilder
 {
   PtrLen<const Row> a;
   PtrLen<const Row> b;

  private:

   class FillRow : NoCopy
    {
      Collector<Cell> collector;
      DynArray<Cell> array;

      PtrLen<Cell> result;

     public:

      FillRow(PtrLen<const Cell> a,PtrLen<const Row> b);

      ~FillRow() {}

      PtrLen<const Cell> getResult() const { return Range_const(result); }
    };

  public:

   MatrixMulBuilder(PtrLen<const Row> a_,PtrLen<const Row> b_) : a(a_),b(b_) {}

   ulen getLen() const { return a.len; }

   PtrLen<Row> operator () (Place<void> place) const;
 };

template <class I,class T,class Row,class Cell>
MatrixMulBuilder<I,T,Row,Cell>::FillRow::FillRow(PtrLen<const Cell> a,PtrLen<const Row> b)
 {
  for(; +a && +b ;)
    switch( Cmp(a->index,b->index) )
      {
       case CmpLess :
        {
         ++a;
        }
       break;

       case CmpGreater :
        {
         ++b;
        }
       break;

       default: // case CmpEqual
        {
         T ma=a->object;

         for(auto p=Range(b->object); +p ;++p)
           {
            T prod=ma*p->object;

            if( +prod ) collector.append_fill(p->index,prod);
           }

         ++a;
         ++b;
        }
    }

  PtrLen<Cell> range=collector.flat();

  array.extend_default(range.len);

  result=JoinSort<Cell,Joiner>(range.ptr,array.getPtr(),range.len);
 }

template <class I,class T,class Row,class Cell>
PtrLen<Row> MatrixMulBuilder<I,T,Row,Cell>::operator () (Place<void> place) const
 {
  typename ArrayAlgo<Row>::BuildGuard guard(place);

  for(auto p=a; +p ;++p)
    {
     FillRow fill(Range(p->object),b);

     auto result=fill.getResult();

     if( +result )
       {
        new(guard.at()) Row(p->index,Set<Cell,Joiner>(result));

        ++guard;
       }
    }

  return guard.disarm();
 }

/* struct MatrixPosition<I,T> */

template <class I,class T>
struct MatrixPosition
 {
  I i;
  I j;
  T object;

  // constuctors

  MatrixPosition() noexcept( Meta::HasNothrowDefaultCtor<I> && Meta::HasNothrowDefaultCtor<T> ) : i(),j(),object() {}

  MatrixPosition(I i_,I j_,const T &object_) : i(i_),j(j_),object(object_) {}

  // cmp objects

  bool operator == (const MatrixPosition<I,T> &obj) const noexcept
   {
    return i==obj.i && j==obj.j ;
   }

  CmpResult operator <=> (const MatrixPosition<I,T> &obj) const noexcept
   {
    return AlphaCmp(i,obj.i,j,obj.j);
   }

  // print object

  void print(PrinterType auto &out) const
   {
    Printf(out,"( #; , #; ) #;",i,j,object);
   }
 };

/* class SparseMatrix<I,T> */

template <class I,class T>
class SparseMatrix
 {
  public:

   using Cell = IndexPair<I,T> ;

   using Row = IndexPair<I,Set<Cell,Joiner> > ;

  private:

   Set<Row,Joiner> rowset;

  private:

   explicit SparseMatrix(const Set<Row,Joiner> &rowset_) : rowset(rowset_) {}

   template <class Builder>
   SparseMatrix(DoBuildType,Builder builder) : rowset(DoBuild,builder) {}

  public:

   // constructors

   class PosFill;

   SparseMatrix() noexcept {}

   explicit SparseMatrix(PosFill &fill);

   ~SparseMatrix() {}

   // cmp objects

   bool operator == (const SparseMatrix<I,T> &obj) const noexcept { return rowset == obj.rowset ; }

   CmpResult operator <=> (const SparseMatrix<I,T> &obj) const { return rowset <=> obj.rowset ; }

   // operators

   friend SparseMatrix<I,T> operator + (const SparseMatrix<I,T> &a,const SparseMatrix<I,T> &b)
    {
     return SparseMatrix<I,T>(a.rowset+b.rowset);
    }

   friend SparseMatrix<I,T> operator * (const SparseMatrix<I,T> &a,const SparseMatrix<I,T> &b)
    {
     return SparseMatrix<I,T>(DoBuild,MatrixMulBuilder<I,T,Row,Cell>(Range(a.rowset),Range(b.rowset)));
    }

   friend SparseVector<I,T> operator * (const SparseMatrix<I,T> &a,const SparseVector<I,T> &b)
    {
     return SparseVector<I,T>(DoBuild,VectorMulBuilder<I,T,typename SparseMatrix<I,T>::Row,
                                                           typename SparseVector<I,T>::Cell>(Range(a.rowset),Range(b.cellset)));
    }

   // print object

   void print(PrinterType auto &out) const
    {
     for(auto &row : Range(rowset) )
       {
        Printf(out,"#; -----\n",row.index);

        for(auto &cell : Range(row.object) ) Printf(out,"  #;\n",cell);
       }

     Printf(out,"-----\n");
    }
 };

/* class SparseMatrix<I,T>::PosFill */

template <class I,class T>
class SparseMatrix<I,T>::PosFill : NoCopy
 {
   using Pos = MatrixPosition<I,T> ;

   DynArray<Pos> buf;
   DynArray<PtrLen<const Pos> > rows;

  private:

   class RowBuilder
    {
      PtrLen<const Pos> row;

     private:

      static T SumOf(PtrLen<const Pos> range)
       {
        typename T::Accumulator acc;

        for(; +range ;++range) acc+=range->object;

        return acc;
       }

     public:

      explicit RowBuilder(PtrLen<const Pos> row_) : row(row_) {}

      ulen getLen() const { return row.len; }

      PtrLen<Cell> operator () (Place<void> place) const
       {
        typename ArrayAlgo<Cell>::BuildGuard guard(place);

        Algon::ApplyUniqueRangeBy(row, [] (const Pos &pos) { return pos.j; } ,
                                       [&] (PtrLen<const Pos> toadd)
                                           {
                                            new(guard.at()) Cell(toadd->j,RowBuilder::SumOf(toadd));

                                            ++guard;
                                           }
                                 );

        return guard.disarm();
       }
    };

   class Builder
    {
      PtrLen<const PtrLen<const Pos> > rows;

     public:

      explicit Builder(PtrLen<const PtrLen<const Pos> > rows_) : rows(rows_) {}

      ulen getLen() const { return rows.len; }

      PtrLen<Row> operator () (Place<void> place) const
       {
        typename ArrayAlgo<Row>::CreateGuard guard(place,getLen());

        for(auto p=rows; +p ;++p,++guard) new(guard.at()) Row((*p)->i,Set<Cell,Joiner>(DoBuild,RowBuilder(*p)));

        return guard.disarm();
       }
    };

   void complete()
    {
     rows.erase();

     auto range=Range(buf);

     Sort(range);

     Algon::ApplyUniqueRangeBy(range, [] (const Pos &pos) { return pos.i; } ,
                                      [this] (PtrLen<Pos> row) { rows.append_copy(Range_const(row)); } );
    }

  public:

   explicit PosFill(ulen capacity) : buf(DoReserve,capacity) {}

   ~PosFill() {}

   void add(const Pos &pos) { if( +pos.object ) buf.append_copy(pos); }

   Builder getBuilder()
    {
     complete();

     return Builder(Range_const(rows));
    }
 };

template <class I,class T>
SparseMatrix<I,T>::SparseMatrix(PosFill &fill) : rowset(DoBuild,fill.getBuilder()) {}

} // namespace App

#endif


