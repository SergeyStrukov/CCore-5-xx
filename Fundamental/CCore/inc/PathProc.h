/* PathProc.h */
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

#ifndef CCore_inc_PathProc_h
#define CCore_inc_PathProc_h

#include <CCore/inc/Path.h>

namespace CCore {

/* classes */

class PathProc;

/* class PathProc */

class PathProc : public NoCopyBase<PathBase>
 {
   static constexpr ulen StackLen = 10 ;

   //
   // --- begin ---
   //
   //   <empty>
   //
   //   /
   //
   //   device:/
   //
   // --- path ---
   //
   //   <begin>
   //
   //   <begin>.
   //
   //   <begin>dir1/dir2/.../dir_or_file , dir_or_file may == "."
   //

   char buf[MaxPathLen];
   ulen buf_len;

   ulen root_len;

   ulen stack[StackLen];
   ulen stack_len;

  private:

   void init()
    {
     stack[0]=root_len;
     stack_len=1;
    }

   void init(ulen root_len_)
    {
     root_len=root_len_;

     init();
    }

   ulen top() const { return stack[stack_len-1]; }

   void push(ulen len);

   void pop();

  public:

   enum BeginType
    {
     BeginEmpty,
     BeginRoot
    };

   explicit PathProc(BeginType=BeginRoot);

   explicit PathProc(StrLen dev_name);

   using PrintProxyType = StrLen ;

   operator StrLen() const { return StrLen(buf,buf_len); }

   bool isRoot() const { return buf_len==root_len ; }

   bool isRootDir() const { return buf_len==root_len+1 && buf[root_len]=='.' ; }

   void root();

   void rootDir();

   void add(StrLen name);

   void back();

   void addPath(StrLen path);

   void copyPath(const PathProc &path);
 };

} // namespace CCore

#endif

