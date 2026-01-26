/* test7102.UserPref.cpp */ 
//----------------------------------------------------------------------------------------
//
//  Project: CCore 5.01
//
//  Tag: Desktop
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003 
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2025 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/test/test.h>

#include <CCore/inc/video/HomeFile.h>
#include <CCore/inc/video/DesktopKey.h>

#include <CCore/inc/FileName.h>
#include <CCore/inc/FileToMem.h>

#include <CCore/inc/ddl/DDLEngine.h>

namespace App {

namespace Private_7102 {


} // namespace Private_7102
 
using namespace Private_7102; 
 
/* Testit<7101> */ 
static StrLen Pretext()
 {
  return
"type Bool = uint8 ;"

"Bool True = 1 ;"

"Bool False = 0 ;"

"type Coord = sint32 ;"

"type MCoord = sint32 ; "

"type VColor = uint32 ;"

"type Clr = uint8 ;"

"struct Point"
" {"
"  Coord x;"
"  Coord y;"
" };"

"struct Ratio"
" {"
"  sint32 ratio;"
" }; "

"struct Font"
" {"
"  type EngineType = int ;"

"  const EngineType EngineDefault  = 0 ;"
"  const EngineType EngineFreeType = 1 ;"

"  EngineType engine_type;"

"  text file_name;"

"  type SizeType = int ;"

"  const SizeType SizeXY    = 0 ;"
"  const SizeType SizePoint = 1 ;"
"  const SizeType SizeIndex = 2 ;"

"  SizeType size_type;"
"  int dx;"
"  int dy;"

"  struct Config"
"   {"
"    type FontHintType = int ;"

"    const FontHintType FontHintNone   = 0 ;"
"    const FontHintType FontHintNative = 1 ;"
"    const FontHintType FontHintAuto   = 2 ;"

"    FontHintType fht;"

"    type FontSmoothType = int ;"

"    const FontSmoothType FontSmoothNone    = 0 ;"
"    const FontSmoothType FontSmooth        = 1 ;"
"    const FontSmoothType FontSmoothLCD_RGB = 2 ;"
"    const FontSmoothType FontSmoothLCD_BGR = 3 ;"

"    FontSmoothType fsm;"

"    Bool use_kerning;"
"    int strength;"
"    int gamma_order;"
"   };"

"  Config cfg;"
" };"_c;
 }

template<>
const char *const Testit<7102>::Name="Test7102 UserPref";

template<>
bool Testit<7102>::Main() 
 {
  using namespace Video;

  HomeFile home_file(HomeKey(),"/UserPreference.ddl"_c);

  SimpleArray<char> temp(4_KByte);
  PrintBuf eout(Range(temp));

  DDL::FileEngine<FileName,FileToMem> engine(eout);

  auto result=engine.process(home_file.get(),Pretext());

  for(const auto &res : result.eval->const_table )
    {
     auto *node=res.node;

     if( node->depth ) continue;

     StrLen name=node->name.getStr();

     Printf(Con,"#; = ???\n",name);
    }

  return true;
 }

} // namespace App
