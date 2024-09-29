/* test3032.FileNameMatch.cpp */
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

#include <CCore/test/test.h>

#include <CCore/inc/FileNameMatch.h>
#include <CCore/inc/PlatformRandom.h>

#include <CCore/inc/Timer.h>
#include <CCore/inc/PrintTime.h>

namespace App {

namespace Private_3032 {

/* Match() */

bool Match(StrLen filter,StrLen file) // basic chars only
 {
  for(; +filter ;++filter)
    {
     switch( char ch=*filter )
       {
        case '*' :
         {
          StrLen next=filter.part(1);

          for(;;++file)
            {
             if( Match(next,file) ) return true;

             if( !file ) return false;
            }
         }
        break;

        case '?' :
         {
          if( !file ) return false;

          ++file;
         }
        break;

        default:
         {
          if( !file || (*file)!=ch ) return false;

          ++file;
         }
       }
    }

  return !file;
 }

/* class Engine */

class Engine : NoCopy
 {
   static constexpr ulen Len = 100 ;

   PlatformRandom random;

   StrLen filter_sym = "1234567890?*"_c ;
   StrLen file_sym = "1234567890"_c ;

   char filter[Len];
   ulen filter_len = 0 ;

   char file[Len];
   ulen file_len = 0 ;

  private:

   char genChar(PtrLen<const char> sym)
    {
     return sym[random.select(sym.len)];
    }

   void fill(PtrLen<char> buf,PtrLen<const char> sym)
    {
     for(char &ch : buf ) ch=genChar(sym);
    }

   void genFilter()
    {
     filter_len=random.select(1,Len);

     fill(Range(filter,filter_len),filter_sym);
    }

   void genFile()
    {
     file_len=random.select(1,Len);

     fill(Range(file,file_len),file_sym);
    }

   StrLen getFilter() const { return StrLen(filter,filter_len); }

   StrLen getFile() const { return StrLen(file,file_len); }

   void genFilterFile()
    {
     ulen len=Len-filter_len;

     file_len=0;

     for(char ch : getFilter() )
       {
        switch( ch )
          {
           case '?' :
            {
             file[file_len++]=genChar(file_sym);
            }
           break;

           case '*' :
            {
             ulen count=random.select(0,len);

             len-=count;

             for(; count ;count--)
               file[file_len++]=genChar(file_sym);
            }
           break;

           default:
            {
             file[file_len++]=ch;
            }
          }
       }
    }

  public:

   Engine()
    {
    }

   ~Engine()
    {
    }

   template <class T>
   void step(ulen rep)
    {
     genFilter();

     T filter(getFilter());

     for(; rep ;rep--)
       {
        genFilterFile();

        if( !filter(getFile()) )
          {
           Printf(Exception,"fail 1 #; #;",getFilter(),getFile());
          }

        genFile();

        if( filter(getFile())!=Match(getFilter(),getFile()) )
          {
           Printf(Exception,"fail 2 #; #;",getFilter(),getFile());
          }
       }
    }

   template <class T>
   void run(ulen count=10000,ulen rep=1000)
    {
     for(; count ;count--)
       {
        Printf(Con,"#;     \r",count);

        step<T>(rep);
       }

     Printf(Con,"\ndone\n");
    }
 };

} // namespace Private_3032

using namespace Private_3032;

/* Testit<3032> */

template<>
const char *const Testit<3032>::Name="Test3032 FileNameMatch";

template<>
bool Testit<3032>::Main()
 {
  Engine engine;

  SecTimer timer;

  engine.run<FileNameFilter>(1000);

  Printf(Con,"time = #;\n\n",PrintTime(timer.get()));

  timer.reset();

  engine.run<SlowFileNameFilter>(1000);

  Printf(Con,"time = #;\n\n",PrintTime(timer.get()));

  engine.run<ComboFileNameFilter>(100);

  return true;
 }

} // namespace App

