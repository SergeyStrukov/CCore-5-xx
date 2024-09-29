/* DirTreeRun.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_DirTreeRun_h
#define CCore_inc_DirTreeRun_h

#include <CCore/inc/FileSystem.h>
#include <CCore/inc/Path.h>
#include <CCore/inc/MemBase.h>

namespace CCore {

/* concept DirTreeProcType<Proc> */

template <class Proc,class DataType>
concept DirTreeProcType2 = requires(Proc &obj,StrLen path,DataType *data)
 {
  Ground<DataType *>( obj.dir(path) );

  Ground<DataType *>( obj.dir(path,path,data) );

  obj.file(path,path,data);

  obj.enddir(path,path,data);
 } ;

template <class Proc>
concept DirTreeProcType = requires()
 {
  typename Proc::DataType;

  requires DirTreeProcType2<Proc,typename Proc::DataType> ;
 } ;

/* classes */

class DirTreeRun;

/* class DirTreeRun */

 //
 // class Proc
 //  {
 //   public:
 //
 //    using DataType = ??? ;
 //
 //    DataType * dir(StrLen root); // name=="." , parent_data==0
 //
 //    DataType * dir(StrLen path,StrLen name,DataType *parent_data);
 //
 //    void file(StrLen path,StrLen name,DataType *parent_data);
 //
 //    void enddir(StrLen path,StrLen name,DataType *data);
 //  };
 //

class DirTreeRun : NoCopyBase<PathBase>
 {
   class Path;
   class Node;

   FileSystem fs;
   StrLen root;
   Node *top;

  private:

   void push(StrLen dir,void *data);

   void push(StrLen base,StrLen dir,void *data);

   void pop();

  public:

   explicit DirTreeRun(StrLen root);

   ~DirTreeRun();

   StrLen pathOfRoot(char buf[MaxPathLen+1]) { return fs.pathOf(root,buf); }

   void apply(DirTreeProcType auto &proc);
 };

class DirTreeRun::Path : NoCopy
 {
   char buf[MaxPathLen];
   ulen baselen;
   ulen off;
   ulen len;

  public:

   explicit Path(StrLen dir);

   Path(StrLen base,StrLen dir);

   StrLen getPath() const { return StrLen(buf,len); }

   StrLen getBase() const { return StrLen(buf,baselen); }

   StrLen getDir() const { return (off<len)? StrLen(buf+off,len-off) : "."_c ; }
 };

class DirTreeRun::Node : public MemBase_nocopy
 {
   Node *next;
   Path path;
   FileSystem::DirCursor cur;
   void *data;

  public:

   Node(Node *next,FileSystem &fs,StrLen dir,void *data);

   Node(Node *next,FileSystem &fs,StrLen base,StrLen dir,void *data);

   ~Node();

   Node * destroy();

   StrLen getPath() const { return path.getPath(); }

   StrLen getBase() const { return path.getBase(); }

   StrLen getDir() const { return path.getDir(); }

   bool nextFile() { return cur.next(); }

   StrLen getFileName() const { return cur.getFileName(); }

   FileType getFileType() const { return cur.getFileType(); }

   template <class DataType>
   DataType * getData() const { return static_cast<DataType *>(data); }
 };

template <DirTreeProcType Proc>
void DirTreeRun::apply(Proc &proc)
 {
  using DataType = typename Proc::DataType ;

  push(root,proc.dir(root));

  while( top )
    {
     while( top->nextFile() )
       {
        StrLen path=top->getPath();
        StrLen name=top->getFileName();
        DataType *parent_data=top->getData<DataType>();

        if( top->getFileType()==FileType_file )
          {
           proc.file(path,name,parent_data);
          }
        else
          {
           if( IsSpecial(name) ) continue;

           push(path,name, proc.dir(path,name,parent_data) );
          }
       }

     proc.enddir(top->getBase(),top->getDir(),top->getData<DataType>());

     pop();
    }
 }

} // namespace CCore

#endif


