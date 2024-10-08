/* SmoothAlgo.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/video/SmoothAlgo.h>

namespace CCore {
namespace Video {
namespace Smooth {

/* class ArcDriver */

void ArcDriver::arc(MPoint a,MPoint b,MPoint c,MCoord radius,unsigned fineness)
 {
  buf[0]=a;
  buf[Len]=b;
  level=0;

  unsigned len1=Len;
  unsigned len2=len1/2;

  for(; level<MaxLevel ;level++,len1=len2,len2/=2)
    {
     MPoint P=buf[0]-c;
     MPoint Q=buf[len1]-c;

     if( AbsDist(P,Q)<=fineness ) break;

     DCoord R=Length(P+Q);

     if( !R ) break;

     for(unsigned i=0; i<Len ;i+=len1)
       {
        MPoint p=buf[i]-c;
        MPoint q=buf[i+len1]-c;

        buf[i+len2]=c+Rational(radius,R)*(p+q);
       }
    }
 }

/* struct LineRound */

LineRound::LineRound(MPoint a,MCoord radius)
 {
  MCoord x=radius/2;
  MCoord y=Ratio(56756,16)*radius;

  buf[0]=a+MPoint(0,radius);
  buf[1]=a+MPoint(-x,y);
  buf[2]=a+MPoint(-y,x);
  buf[3]=a+MPoint(-radius,0);
  buf[4]=a+MPoint(-y,-x);
  buf[5]=a+MPoint(-x,-y);
  buf[6]=a+MPoint(0,-radius);
  buf[7]=a+MPoint(x,-y);
  buf[8]=a+MPoint(y,-x);
  buf[9]=a+MPoint(radius,0);
  buf[10]=a+MPoint(y,x);
  buf[11]=a+MPoint(x,y);
 }

/* struct LineArc */

DCoord LineArc::Sigma(MPoint a,MPoint b)
 {
  return Prod(a.x,a.y,b.y,-b.x);
 }

DCoord LineArc::Sigma(MPoint a,MPoint b,MPoint c)
 {
  return Sigma(b-a,c-a);
 }

MPoint LineArc::Bisect(MPoint a,MPoint b,MCoord radius)
 {
  if( a==b ) return a;

  MPoint p=a+b;

  if( p==Null ) return Rotate90(a);

  {
   DCoord L=Length(p);

   if( L>=radius ) return Rational(radius,L)*p;
  }

  MPoint q=a-b;

  {
   DCoord L=Length(q);

   if( L>=radius ) return Rotate90(Rational(radius,L)*q);
  }

  return a;
 }

unsigned LineArc::SBits(DCoord a)
 {
  uDCoord u=a;

  if( a<0 ) u=~u;

  return UIntBitsOf(u)+1;
 }

MPoint LineArc::Sect(DCoord A,DCoord B,MPoint p)
 {
  B+=A;

  MCoord a;
  MCoord b;

  unsigned n=Max(SBits(A),SBits(B));
  unsigned bits=Meta::UIntBits<uMCoord>;

  if( n<=bits )
    {
     a=MCoord(A);
     b=MCoord(B);
    }
  else
    {
     n-=bits;

     a=MCoord(IntRShift(A,n));
     b=MCoord(IntRShift(B,n));
    }

  if( !b ) return Null;

  return Rational(a,b)*p;
 }

void LineArc::intersect(MPoint a,MPoint b,MPoint c,MPoint d,MPoint base)
 {
  DCoord A=Sigma(a,c,d);
  DCoord B=Sigma(b,d,c);
  DCoord C=Sigma(c,a,b);
  DCoord D=Sigma(d,b,a);

  if( ( C<0 && D>0 ) || ( C>0 && D<0 ) || ( A<0 && B>0 ) || ( A>0 && B<0 ) )
    {
     p=base+a;
     q=base+c;

     type=TwoPoint;
    }
  else
    {
     p=base+c+Sect(C,D,d-c);

     type=OnePoint;
    }
 }

LineArc::LineArc(MPoint a,MPoint b,MPoint c,MCoord radius)
 {
  a-=b;
  c-=b;

  if( a==Null || c==Null )
    {
     p=b;

     type=OnePoint;

     return;
    }

  if( Sigma(a,c)>0 )
    {
     MPoint A=Rotate(a)(0,radius);
     MPoint B=A+a;

     MPoint C=Rotate(c)(0,-radius);
     MPoint D=C+c;

     intersect(A,B,C,D,b);
    }
  else
    {
     MPoint P=Rotate(a)(0,radius);
     MPoint R=Rotate(c)(0,-radius);

     p=b+P;
     r=b+R;
     q=b+Bisect(P,R,radius);

     type=Arc;
    }
 }

} // namespace Smooth
} // namespace Video
} // namespace CCore


