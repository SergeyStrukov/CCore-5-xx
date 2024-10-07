/* AltInflator.h */
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

#ifndef App_AltInflator_h
#define App_AltInflator_h

#include <CCore/inc/Deflate.h>
#include <CCore/inc/CoTask.h>

namespace App {

/* using */

using namespace CCore;
using namespace Deflate;

/* class Inflator */

class Inflator : CoTaskStack<10>
 {
   WindowOut out;
   BitReader reader;
   bool eof = false ;

   // params

   bool repeat;
   Function<void (void)> trigger;

   // decoders

   HuffmanDecoder dynamic_literal_decoder;
   HuffmanDecoder dynamic_distance_decoder;

  private:

   bool noeof = false ;

   CoTask<void> waitBits();

   CoTask<void> cowaitbits;

   auto wait(bool noeof_) { noeof=noeof_; return cocall(cowaitbits); }

  private:

   BitLen bitlen = 0 ;

   CoTask<void> processBits();

   CoTask<void> cobits;

   auto decodeBits(unsigned bitlen_) { bitlen=bitlen_; return cocall(cobits); }

   auto waitMoreBits() { return decodeBits(reader.bitsBuffered()+1); }

  private:

   CoTask<void> processCode();

   CoTask<void> cocode;

  private:

   bool last_block = false ;
   UCode block_type = 0 ;
   ulen stored_len = 0 ;

   CoTask<void> processHeader();

   CoTask<void> coheader;

  private:

   CoTask<void> processBody();

   CoTask<void> cobody;

  private:

   CoTask<void> process();

   CoTask<void> coproc;

  public:

   explicit Inflator(OutFunc out,bool repeat=false);

   ~Inflator();

   void setTrigger(Function<void (void)> trigger_) { trigger=trigger_; }

   void put(const uint8 *ptr,ulen len) { put({ptr,len}); }

   void put(PtrLen<const uint8> data);

   void complete();
 };

} // namespace App

#endif


