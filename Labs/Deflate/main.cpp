/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Labs/Deflate
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Exception.h>

#include "AltInflator.h"

#include <CCore/inc/PlatformRandom.h>

#include <CCore/inc/Timer.h>
#include <CCore/inc/PrintTime.h>

#include <CCore/inc/FileToMem.h>
#include <CCore/inc/FileSystem.h>
#include <CCore/inc/MakeFileName.h>

namespace App {

/* Rat() */

unsigned Rat(ulen a,ulen b)
 {
  if( !b ) return 100;

  return (a*100)/b;
 }

/* class Transform<T> */

template <class T>
class Transform : public Funchor_nocopy
 {
   PtrLen<const uint8> data;

   T work;

   Collector<uint8> result;

  private:

   void out(PtrLen<const uint8> r)
    {
     result.extend_copy(r);
    }

   Function<void (PtrLen<const uint8>)> function_out() { return FunctionOf(this,&Transform::out); }

  public:

   template <class ... SS>
   Transform(PtrLen<const uint8> data_,SS && ... ss)
    : data(data_),
      work(function_out(), std::forward<SS>(ss)... )
    {
    }

   ~Transform()
    {
    }

   PtrLen<const uint8> getResult() { return result.flat(); }

   void run(PlatformRandom &random)
    {
     ulen off=0;

     while( off<data.len )
       {
        ulen len=Min<ulen>(data.len-off,random.select(1000));

        work.put(data.part(off,len));

        off+=len;
       }

     work.complete();
    }
 };

/* class Engine */

class Engine : NoCopy
 {
   PlatformRandom random;
   StrLen file_name;
   PtrLen<const uint8> data;
   Deflate::Param param;

  private:

   void test()
    {
     // 1

     Transform<Deflate::Deflator> zip(data,param);

     zip.run(random);

     auto zipped=zip.getResult();

     // 2

     Transform<App::Inflator> unzip(zipped);

     unzip.run(random);

     auto unzipped=unzip.getResult();

     // 3

     if( data.equal(unzipped) )
       {
        Printf(Con,"#; : #; -> #; (#; %)\n",file_name,data.len,zipped.len,Rat(zipped.len,data.len));
       }
     else
       {
        Printf(Exception,"#; : failed",file_name);
       }
    }

   void test(Deflate::Level level,Deflate::Log2WindowLen log2_window_len)
    {
     param.log2_window_len=log2_window_len;

     Printf(Con,"#;.#;) ",level,log2_window_len);

     test();
    }

   void test(Deflate::Level level)
    {
     param.level=level;

     test(level,Deflate::MinLog2WindowLen);
     test(level,Deflate::MaxLog2WindowLen);
    }

  public:

   Engine() {}

   ~Engine() {}

   void test(StrLen dir,StrLen file_name)
    {
     this->file_name=file_name;

     MakeFileName path(dir,file_name);
     FileToMem file(path.get());

     data=Range(file);
     param={};

     test(Deflate::MinLevel);
     test(Deflate::DefaultLevel);
     test(Deflate::EverdynamicLevel);
     test(Deflate::MaxLevel);

     Putch(Con,'\n');
    }
 };

/* Test() */

void Test(StrLen dir)
 {
  MSecTimer timer;

  FileSystem fs;
  FileSystem::DirCursor cur(fs,dir);

  Engine engine;

  cur.apply( [&] (StrLen file_name,FileType file_type)
                 {
                  if( file_type==FileType_file ) engine.test(dir,file_name);

                 } );

  Printf(Con,"time = #; msec\n",timer.get());
 }

/* Main() */

void Main()
 {
  Test("obj");
 }

} // namespace App

/* main() */

using namespace App;

int main()
 {
  try
    {
     ReportException report;

     Main();

     report.guard();

     return 0;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

