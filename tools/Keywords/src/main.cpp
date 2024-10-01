/* main.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: Keywords 1.00
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

#include <CCore/inc/algon/SortUnique.h>

#include <CCore/inc/ddl/DDLEngine.h>
#include <CCore/inc/ddl/DDLTypedMap.h>
#include <CCore/inc/ddl/DDLMapBase.h>

#include <CCore/inc/FileName.h>
#include <CCore/inc/FileToMem.h>

namespace App {

/* using */

using namespace CCore;

/* class Engine */

class Engine : NoCopy
 {
   PrintFile out;

  private:

   struct Char
    {
     char ch;
     bool exist;

     Char(StrLen str,ulen off)
      {
       if( str.len>off )
         {
          ch=str[off];
          exist=true;
         }
       else
         {
          ch=0;
          exist=false;
         }
      }

     bool operator < (Char obj) const
      {
       if( !obj.exist ) return false;
       if( !exist ) return true;

       return ch<obj.ch;
      }

     bool operator != (Char obj) const
      {
       return exist!=obj.exist || ch!=obj.ch ;
      }
    };

   void gen(PtrLen<DDL::MapText> list,ulen off=0); // non-empty , str.len >= off

  public:

   explicit Engine(StrLen file_name);

   ~Engine();

   void run(PtrLen<DDL::MapText> list);
 };

void Engine::gen(PtrLen<DDL::MapText> list,ulen off)
 {
  ulen indent_len=2+5*off;
  RepeatChar indent(indent_len,' ');

  if( list.len<2 )
    {
     StrLen str=list[0];
     StrLen part=str.part(off);

     Printf(out,"#;if( str.len==#; ",indent,part.len);

     for(ulen i=0; i<part.len ;i++) Printf(out,"&& str[#;]=='#.c;' ",i,part[i]);

     Printf(out,") return Atom_#;; else return Atom_Name;\n",str);

     return;
    }

  bool sw=false;

  Algon::IncrSortThenApplyUniqueRangeBy(list, [=] (DDL::MapText str) { return Char(str,off); } ,
                                              [&] (PtrLen<DDL::MapText> sublist)
                                                  {
                                                   StrLen str=sublist[0];

                                                   if( str.len==off )
                                                     {
                                                      Printf(out,"#;if( !str ) return Atom_#;;\n\n",indent,str);

                                                      Printf(out,"#;switch( str[0] )\n",indent);
                                                      Printf(out,"#;  {\n",indent);

                                                      sw=true;
                                                      indent=RepeatChar(indent_len+3,' ');
                                                     }
                                                   else
                                                     {
                                                      if( !sw )
                                                        {
                                                         Printf(out,"#;if( !str ) return Atom_Name;\n\n",indent);

                                                         Printf(out,"#;switch( str[0] )\n",indent);
                                                         Printf(out,"#;  {\n",indent);

                                                         sw=true;
                                                         indent=RepeatChar(indent_len+3,' ');
                                                        }

                                                      char ch=str[off];

                                                      Printf(out,"#;case '#.c;' :\n",indent,ch);
                                                      Printf(out,"#; {\n",indent);

                                                      Printf(out,"#;  ++str;\n\n",indent);

                                                      gen(sublist,off+1);

                                                      Printf(out,"#; }\n\n",indent);
                                                     }
                                                  }
                                       );

  indent=RepeatChar(indent_len,' ');

  Printf(out,"#;   default: return Atom_Name;\n",indent);
  Printf(out,"#;  }\n",indent);
 }

Engine::Engine(StrLen file_name)
 : out(file_name,Open_ToWrite|Open_AutoDelete)
 {
 }

Engine::~Engine()
 {
 }

void Engine::run(PtrLen<DDL::MapText> list) // non-empty
 {
  Printf(out,"AtomClass Atom::GetWordClass(StrLen str)\n {\n");

  gen(list);

  Printf(out," }\n\n");

  out.preserveFile();
 }

/* class TypeSet */

struct TypeSet
 {
  explicit TypeSet(ulen) {}

  DDL::MapSizeInfo structSizeInfo(DDL::StructNode *)
   {
    DDL::MapSizeInfo ret;

    Printf(Exception,"Unknown structure");

    return ret;
   }

  template <class T>
  bool isStruct(DDL::StructNode *) const { return false; }

  void guardFieldTypes(DDL::StructNode *) const
   {
    Printf(Exception,"Unknown structure");
   }
 };

/* Process() */

void Process(StrLen input_file_name,StrLen output_file_name)
 {
  PrintCon msg;
  DDL::FileEngine<FileName,FileToMem> ddl_engine(msg);

  auto result=ddl_engine.process(input_file_name);

  msg.flush();

  if( !result ) return;

  DDL::TypedMap<TypeSet> map(result);
  DDL::MapBase map_base(map);

  auto input=map.takeConst< DDL::MapRange<DDL::MapText> >("Keywords").getRange();

  if( !input )
    {
     Printf(Exception,"Bad input file : empty Keywords list");
    }

  Engine engine(output_file_name);

  engine.run(input);
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
      Putobj(Con,"--- Keywords 1.00 ---\n--- Copyright (c) 2015 Sergey Strukov. All rights reserved. ---\n\n");

      if( argc!=3 )
        {
         Printf(Con,"Usage: CCore-Keywords <input-file> <output-file>\n");

         return 1;
        }

      Process(argv[1],argv[2]);
     }

     report.guard();

     return 0;
    }
  catch(CatchType)
    {
     return 1;
    }
 }

