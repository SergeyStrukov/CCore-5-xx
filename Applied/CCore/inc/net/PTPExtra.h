/* PTPExtra.h */
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

#ifndef CCore_inc_net_PTPExtra_h
#define CCore_inc_net_PTPExtra_h

#include <CCore/inc/SaveLoad.h>

namespace CCore {
namespace Net {
namespace PTP {

/* types */

using ServiceIdType = uint32 ;

using FunctionIdType = uint32 ;

using LenType = uint32 ;

using BoolType = uint32 ;

using FlagType = uint32 ;

using ErrorIdType = uint32 ;

/* consts */

inline constexpr ErrorIdType NoError          = 0 ;
inline constexpr ErrorIdType Error_BadInput   = 1 ;
inline constexpr ErrorIdType Error_NoFunction = 2 ;
inline constexpr ErrorIdType Error_Exhausted  = 3 ;
inline constexpr ErrorIdType Error_Unknown    = 4 ;

/* classes */

struct ServiceFunction;

struct Result;

struct Empty;

/* struct ServiceFunction */

struct ServiceFunction
 {
  ServiceIdType service_id;
  FunctionIdType function_id;

  // constructors

  ServiceFunction() : service_id(),function_id() {}

  ServiceFunction(ServiceIdType service_id_,FunctionIdType function_id_)
   : service_id(service_id_),function_id(function_id_) {}

  // save/load object

  using SaveLoadOrder = BeOrder ;

  template <template <class ...> class Ret,class T>
  static constexpr auto Fold()
   {
    return Ret(&T::service_id,&T::function_id);
   }
 };

/* struct Result */

struct Result
 {
  ServiceFunction serv_func;
  ErrorIdType error_id;

  // constructors

  Result() : serv_func(),error_id() {}

  Result(ServiceFunction serv_func_,ErrorIdType error_id_) : serv_func(serv_func_),error_id(error_id_) {}

  // save/load object

  using SaveLoadOrder = BeOrder ;

  template <template <class ...> class Ret,class T>
  static constexpr auto Fold()
   {
    return Ret(&T::serv_func,&T::error_id);
   }
 };

/* struct Empty */

struct Empty
 {
  // constructors

  Empty() {}

  // save/load object

  static constexpr ulen SaveLoadLen = 0 ;

  void save(SaveDevType auto &) const {}

  void load(LoadDevType auto &) {}
 };

} // namespace PTP
} // namespace Net
} // namespace CCore

#endif


