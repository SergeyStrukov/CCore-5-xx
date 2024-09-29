/* DDLEngine.h */
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
//  Copyright (c) 2021 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_ddl_DDLEngine_h
#define CCore_inc_ddl_DDLEngine_h

#include <CCore/inc/StrKey.h>

#include <CCore/inc/ddl/DDLParser.h>
#include <CCore/inc/ddl/DDLEval.h>

#include <CCore/inc/Tuple.h>
#include <CCore/inc/TreeMap.h>

namespace CCore {
namespace DDL {

/* concept FileNameType<FileName> */

template <class FileName>
concept FileNameType = MovableType<FileName> && requires(FileName &obj,PrintBase &out,TextPos pos,StrLen path,StrLen file_name)
 {
  FileName();

  FileName(file_name);

  FileName(path,file_name);

  Ground<bool>( +obj );

  Ground<bool>( !obj );

  Ground<StrLen>( obj.getStr() );

  Ground<StrLen>( obj.getPath() );

  Ground<StrLen>( obj.getFile() );

  obj.printPos(out,pos);
 } ;

/* classes */

struct EngineResult;

class TextEngine;

template <FileNameType FileName,class FileText,class ... SS> class FileEngine;

/* struct EngineResult */

struct EngineResult
 {
  EvalResult *eval;
  BodyNode *body;

  EngineResult() : eval(0),body(0) {}

  EngineResult(NothingType) : EngineResult() {}

  EngineResult(EvalResult *eval_,BodyNode *body_) : eval(eval_),body(body_) {}

  EvalResult * operator + () const { return eval; }

  bool operator ! () const { return !eval; }
 };

/* class TextEngine */

class TextEngine : ParserContext
 {
   FileId id;
   StrLen pretext;
   StrLen text;

   EvalResult result;

  private:

   virtual File openFile(StrLen file_name);

  public:

   TextEngine(PrintBase &msg,StrLen text,StrLen pretext=Empty);

   ~TextEngine();

   EngineResult process();
 };

/* class FileEngine<FileName,FileText> */

template <FileNameType FileName,class FileText,class ... SS>
class FileEngine : ParserContext
 {
   struct FileRec : FileId
    {
     FileName file_name;
     FileText file_text;

     FileRec(const SS & ... ss,FileName &&file_name_,ulen max_file_len)
      : file_name(std::move(file_name_)),
        file_text(file_name.getStr(),max_file_len,ss...)
      {
      }

     StrLen getText() const { return Mutate<const char>(Range(file_text)); }

     virtual void printPos(PrintBase &out,TextPos pos) { file_name.printPos(out,pos); }
    };

   RBTreeMap<StrKey,FileRec> map;
   Tuple<SS...> args;

   ulen max_files;
   ulen max_inc;
   ulen max_file_len;

   ulen file_count;
   ulen inc_count;

   EvalResult result;

  private:

   static File Make(FileRec *rec);

   File open(FileName &&file_name);

   virtual File openFile(StrLen file_name);

   virtual File openFile(FileId *file_id,const Token &name);

  public:

   static constexpr ulen DefaultMaxFiles    =    1000 ;
   static constexpr ulen DefaultMaxIncludes =     100 ;
   static constexpr ulen DefaultMaxFileLen  = MaxULen ;

   explicit FileEngine(const SS & ... args,PrintBase &msg,ulen mem_cap=MaxULen,
                                                          ulen max_files=DefaultMaxFiles,
                                                          ulen max_inc=DefaultMaxIncludes,
                                                          ulen max_file_len=DefaultMaxFileLen);

   ~FileEngine();

   using ParserContext::reset;

   void purge();

   EngineResult process(StrLen file_name,StrLen pretext);

   EngineResult process(StrLen file_name);
 };

template <FileNameType FileName,class FileText,class ... SS>
auto FileEngine<FileName,FileText,SS...>::Make(FileRec *rec) -> File
 {
  return File(rec,rec->getText());
 }

template <FileNameType FileName,class FileText,class ... SS>
auto FileEngine<FileName,FileText,SS...>::open(FileName &&file_name) -> File
 {
  StrKey key(file_name.getStr());

  FileRec *rec=0;

  args.call( [&] (const SS & ... ss)
                 {
                  if( file_count>=max_files )
                    {
                     rec=map.find(key);

                     if( !rec ) error("Too many open files");
                    }
                  else
                    {
                     auto result=map.find_or_add(key,ss...,std::move(file_name),max_file_len);

                     if( result.new_flag ) file_count++;

                     rec=result.obj;
                    }
                 } );

  if( rec ) return Make(rec);

  return Nothing;
 }

template <FileNameType FileName,class FileText,class ... SS>
auto FileEngine<FileName,FileText,SS...>::openFile(StrLen file_name_) -> File
 {
  inc_count=0;

  FileName file_name(file_name_);

  if( !file_name )
    {
     error("Bad file name #.q;",file_name_);

     return Nothing;
    }

  return open(std::move(file_name));
 }

template <FileNameType FileName,class FileText,class ... SS>
auto FileEngine<FileName,FileText,SS...>::openFile(FileId *file_id,const Token &name) -> File
 {
  if( inc_count>=max_inc )
    {
     error("Too many included files");

     return Nothing;
    }

  inc_count++;

  FileRec *base=static_cast<FileRec *>(file_id);

  StrLen file_name_=name.str.inner(1,1);

  FileName file_name(base->file_name.getPath(),file_name_);

  if( !file_name )
    {
     error("Bad include file name #; #.q;",PrintPos(file_id,name.pos),file_name_);

     return Nothing;
    }

  return open(std::move(file_name));
 }

template <FileNameType FileName,class FileText,class ... SS>
FileEngine<FileName,FileText,SS...>::FileEngine(const SS & ... args_,PrintBase &msg,ulen mem_cap,ulen max_files_,ulen max_inc_,ulen max_file_len_)
 : ParserContext(msg,mem_cap),
   args(args_...),
   max_files(max_files_),
   max_inc(max_inc_),
   max_file_len(max_file_len_),
   file_count(0),
   inc_count(0)
 {
 }

template <FileNameType FileName,class FileText,class ... SS>
FileEngine<FileName,FileText,SS...>::~FileEngine()
 {
 }

template <FileNameType FileName,class FileText,class ... SS>
void FileEngine<FileName,FileText,SS...>::purge()
 {
  map.erase();
 }

template <FileNameType FileName,class FileText,class ... SS>
EngineResult FileEngine<FileName,FileText,SS...>::process(StrLen file_name,StrLen pretext)
 {
  if( BodyNode *body_node=parseFile(file_name,pretext) )
    {
     if( EvalContext::Process(error,pool,body_node,result) )
       {
        return EngineResult(&result,body_node);
       }
    }

  return Nothing;
 }

template <FileNameType FileName,class FileText,class ... SS>
EngineResult FileEngine<FileName,FileText,SS...>::process(StrLen file_name)
 {
  if( BodyNode *body_node=parseFile(file_name) )
    {
     if( EvalContext::Process(error,pool,body_node,result) )
       {
        return EngineResult(&result,body_node);
       }
    }

  return Nothing;
 }

} // namespace DDL
} // namespace CCore

#endif

