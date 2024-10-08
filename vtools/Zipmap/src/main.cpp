/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Zipmap 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/MakeString.h>
#include <CCore/inc/Path.h>

#include <CCore/inc/video/Bitmap.h>

namespace App {

/* using */

using namespace CCore;

/* Zip() */

void Zip(StrLen src_file,StrLen dst_file)
 {
  Printf(Con,"zip #.q; -> #.q;\n\n",src_file,dst_file);

  Video::BitmapData data;

  data.loadBitmap(src_file);
  data.saveZipmap(dst_file);
 }

/* Unzip() */

void Unzip(StrLen src_file,StrLen dst_file)
 {
  Printf(Con,"unzip #.q; -> #.q;\n\n",src_file,dst_file);

  Video::BitmapData data;

  data.loadZipmap(src_file);
  data.saveBitmap(dst_file);
 }

/* Main() */

StrLen ChangeExt(MakeString<MaxPathLen> &buf,StrLen src_file,StrLen new_ext)
 {
  StrLen ext=SuffixExt(src_file);

  if( !buf.add(src_file.inner(0,ext.len),new_ext) )
    {
     Printf(Exception,"App::ChangeExt(...) : too long file name");
    }

  return buf.get();
 }

void Main(int argc,const char *argv[],StrLen cmd)
 {
  MakeString<MaxPathLen> temp;

  if( cmd.equal("-z"_c) )
    {
     StrLen src_file=argv[2];

     StrLen dst_file;

     if( argc==3 )
       {
        dst_file=ChangeExt(temp,src_file,".zipmap"_c);
       }
     else if( argc==4 )
       {
        dst_file=argv[3];
       }
     else
       {
        Printf(Exception,"App::Main(...) : bad arguments number");
       }

     Zip(src_file,dst_file);

     return;
    }

  if( cmd.equal("-x"_c) )
    {
     StrLen src_file=argv[2];

     StrLen dst_file;

     if( argc==3 )
       {
        dst_file=ChangeExt(temp,src_file,".bitmap"_c);
       }
     else if( argc==4 )
       {
        dst_file=argv[3];
       }
     else
       {
        Printf(Exception,"App::Main(...) : bad arguments number");
       }

     Unzip(src_file,dst_file);

     return;
    }

  Printf(Exception,"App::Main(...) : bad command");
 }

int Main(int argc,const char *argv[])
 {
  if( argc<3 )
    {
     if( argc==1 )
       {
        Putobj(Con,"Usage: CCore-Zipmap -z <src-file> [<zipmap-file>]\n");
        Putobj(Con,"OR     CCore-Zipmap -x <src-file> [<bitmap-file>]\n\n");

        return 1;
       }

     Printf(Exception,"App::Main(...) : bad arguments number");
    }

  Main(argc,argv,argv[1]);

  return 0;
 }

} // namespace App

/* main() */

using namespace App;

int main(int argc,const char *argv[])
 {
  try
    {
     ReportException report;
     int ret;

     {
      Putobj(Con,"--- Zipmap 1.00 ---\n--- Copyright (c) 2022 Sergey Strukov. All rights reserved. ---\n\n");

      ret=Main(argc,argv);
     }

     report.guard();

     return ret;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

