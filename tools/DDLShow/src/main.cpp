/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: DDLShow 1.00
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2021 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/Print.h>
#include <CCore/inc/Exception.h>

#include <CCore/inc/String.h>
#include <CCore/inc/FileName.h>
#include <CCore/inc/FileToMem.h>

#include <CCore/inc/ddl/DDLEngine.h>

namespace App {

/* using */

using namespace CCore;

/* type Engine */

using Engine = DDL::FileEngine<FileName,FileToMem> ;

/* Process() */

void Process(StrLen file_name)
 {
  String temp=StringCat(file_name,".txt"_str);
  StrLen txt_file_name=Range(temp);

  Printf(Con,"#; -> #;\n",file_name,txt_file_name);

  PrintFile out(txt_file_name);
  Engine engine(out);

  Printf(out,"/* #; */\n\n",file_name);

  auto result=engine.process(file_name);

  if( !result )
    {
     Putobj(out,"\n\n");
    }
  else
    {
     Printf(out,"/* result */\n\n");

     DDL::ConstResult::PrintOptType opt(*result.eval);

     for(const auto &res : result.eval->const_table )
       {
        Printf(out,"#;\n\n",BindOpt(opt,res));
       }

     Putobj(out,"\n\n");
    }
 }

} // namespace App

/* main() */

using namespace App;

int main(int argc,const char *argv[])
 {
  try
    {
     ReportException report;

     {
      Putobj(Con,"--- DDLShow 1.00 ---\n--- Copyright (c) 2021 Sergey Strukov. All rights reserved. ---\n\n");

      for(int i=1; i<argc ;i++) Process(argv[i]);
     }

     report.guard();

     return 0;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

