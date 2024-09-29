/* Complex.h */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_math_Complex_h
#define CCore_inc_math_Complex_h

#include <cmath>
#include <numbers>

#include <CCore/inc/Gadget.h>

namespace CCore {
namespace Math {

/* classes */

template <class T> struct Complex;

/* struct Complex<T> */

template <class T>
struct Complex
 {
  T re;
  T im;

  constexpr Complex() noexcept : re(0),im(0) {}

  template <class Re> requires ( ConstructibleType<T,Re> )
  constexpr Complex(Re re_) : re(re_),im(0) {}

  template <class Re,class Im> requires ( ConstructibleType<T,Re> && ConstructibleType<T,Im> )
  constexpr Complex(Re re_,Im im_) : re(re_),im(im_) {}

  static constexpr Complex<T> I{0,1};

  T norm() const { return Sq(re)+Sq(im); }

  Complex<T> operator - () const { return {-re,-im}; }

  Complex<T> conj() const { return {re,-im}; }

  Complex<T> mulI() const { return {-im,re}; }

  Complex<T> & neg() { (*this)=-(*this); return *this; }

  Complex<T> & revsub(Complex<T> b) { (*this)=b-(*this); return *this; }

  Complex<T> & operator += (Complex<T> b) { (*this)=(*this)+b; return *this; }

  Complex<T> & operator -= (Complex<T> b) { (*this)=(*this)-b; return *this; }

  Complex<T> & operator *= (Complex<T> b) { (*this)=(*this)*b; return *this; }

  Complex<T> & operator /= (Complex<T> b) { (*this)=(*this)/b; return *this; }

  static Complex<T> UniExp(ulen ind,ulen len); // exp(2*pi*(ind/len))
 };

template <class T>
Complex<T> operator + (Complex<T> a,Complex<T> b)
 {
  return {a.re+b.re,a.im+b.im};
 }

template <class T>
Complex<T> operator - (Complex<T> a,Complex<T> b)
 {
  return {a.re-b.re,a.im-b.im};
 }

template <class T>
Complex<T> operator * (Complex<T> a,Complex<T> b)
 {
  return {a.re*b.re-a.im*b.im,a.re*b.im+a.im*b.re};
 }

template <class T>
Complex<T> operator / (Complex<T> a,Complex<T> b)
 {
  return a*b.conj()*(1/b.norm());
 }


template <class T,class B> requires( ConstructibleType<Complex<T>,B> )
Complex<T> operator + (Complex<T> a,B b) { return a+Complex<T>(b); }

template <class T,class B> requires( ConstructibleType<Complex<T>,B> )
Complex<T> operator - (Complex<T> a,B b) { return a-Complex<T>(b); }

template <class T,class B> requires( ConstructibleType<Complex<T>,B> )
Complex<T> operator * (Complex<T> a,B b) { return a*Complex<T>(b); }

template <class T,class B> requires( ConstructibleType<Complex<T>,B> )
Complex<T> operator / (Complex<T> a,B b) { return a/Complex<T>(b); }


template <class T,class A> requires( ConstructibleType<Complex<T>,A> )
Complex<T> operator + (A a,Complex<T> b) { return Complex<T>(a)+b; }

template <class T,class A> requires( ConstructibleType<Complex<T>,A> )
Complex<T> operator - (A a,Complex<T> b) { return Complex<T>(a)-b; }

template <class T,class A> requires( ConstructibleType<Complex<T>,A> )
Complex<T> operator * (A a,Complex<T> b) { return Complex<T>(a)*b; }

template <class T,class A> requires( ConstructibleType<Complex<T>,A> )
Complex<T> operator / (A a,Complex<T> b) { return Complex<T>(a)/b; }

/* double functions */

inline double SupNorm(Complex<double> c)
 {
  return Max( std::abs(c.re) , std::abs(c.im) );
 }

inline double Norm(Complex<double> c)
 {
  return c.norm();
 }

inline double Abs(Complex<double> c)
 {
  return std::sqrt(Norm(c));
 }

inline Complex<double> UniExp(double t)
 {
  double x=2*std::numbers::pi*t;

  return { std::cos(x) , std::sin(x) };
 }

template <>
inline Complex<double> Complex<double>::UniExp(ulen ind,ulen len)
 {
  return Math::UniExp(double(ind)/len);
 }

} // namespace Math
} // namespace CCore

#endif

