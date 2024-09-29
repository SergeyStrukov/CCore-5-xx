/* Win64Types.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN64
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_win64_Win64Types_h
#define CCore_inc_win64_Win64Types_h

namespace Win64 {

/*--------------------------------------------------------------------------------------*/
/* basic types                                                                          */
/*--------------------------------------------------------------------------------------*/

/* type wchar */

using wchar = unsigned short ;

/* type bool_t */

using bool_t = int ;

/* type negbool_t */

using negbool_t = int ;

/* type error_t */

using error_t = int ;

/* type ushortlen_t */

using ushortlen_t = unsigned ;

/* type ulen_t */

using ulen_t = unsigned long ;

/* type index_t */

using index_t = unsigned ;

/* type flags_t */

using flags_t = unsigned ;

/* type options_t */

using options_t = int ;

/* type timeout_t */

using timeout_t = unsigned ;

/* type void_ptr */

using void_ptr = void * ;

/* type const_void_ptr */

using const_void_ptr = const void * ;

/* type handle_t */

using handle_t = long ;

/* type socket_t */

using socket_t = long ;

/* type numid_t */

using numid_t = unsigned ;

/* type file_time_t */

using file_time_t = unsigned long ;

/* type file_len_t */

using file_len_t = unsigned long ;

/* type codepage_t */

using codepage_t = unsigned ;

/* type lcid_t */

using lcid_t = unsigned ;

/* type sem_count_t */

using sem_count_t = int ;

} // namespace Win64

#endif

