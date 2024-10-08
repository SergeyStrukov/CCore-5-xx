/* test3009.PacketSet.cpp */
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

#include <CCore/inc/PacketSet.h>
#include <CCore/inc/Task.h>
#include <CCore/inc/Abort.h>
#include <CCore/inc/PlatformRandom.h>
#include <CCore/inc/Counters.h>
#include <CCore/inc/UIntSplit.h>

namespace App {

namespace Private_3009 {

/* enum Status */

enum Status
 {
  StatusOk,
  StatusInitial,
  StatusPending,
  StatusWorking,
  StatusBadFormat,
  StatusFail,
  StatusCancelled,
  StatusNoPacket,
  StatusPacket,
  StatusPacketIsShort,

  StatusLim
 };

const char * GetTextDesc(Status st)
 {
  static const char *const Table[]=
   {
    "Ok",
    "Initial",
    "Pending",
    "Working",
    "BadFormat",
    "Fail",
    "Cancelled",
    "NoPacket",
    "Packet",
    "Packet is short",

    ""
   };

  return Table[st];
 }

/* enum Event */

enum Event
 {
  EventPut,
  EventCancelOnPut,
  EventCancelOnGet,
  EventCancelPending,
  EventCancelWorking,

  EventLim
 };

const char * GetTextDesc(Event ev)
 {
  static const char *const Table[]=
   {
    "Put",
    "Cancel on put",
    "Cancel on get",
    "Cancel pending",
    "Cancel working",

    ""
   };

  return Table[ev];
 }

/* struct Ext */

struct Ext
 {
  Status status;

  Ext() : status(StatusInitial) {}
 };

/* class DevBase */

class DevBase : public Funchor_nocopy
 {
   Mutex mutex;

   PacketList list;
   bool run_flag;
   Counters<Event,EventLim> stat;

   Sem sem;

   Sem stop_sem;

  private:

   void event_locked(Event ev)
    {
     Mutex::Lock lock(mutex);

     stat.count(ev);
    }

   virtual bool process(Packet<uint8,Ext> packet)=0;

   struct ProcessResult
    {
     Packet<uint8,Ext> packet;
     bool stop;

     bool finish()
      {
       if( +packet ) packet.complete();

       return stop;
      }
    };

   struct ResultStop : ProcessResult
    {
     ResultStop()
      {
       stop=true;
      }
    };

   struct ResultContinue : ProcessResult
    {
     ResultContinue()
      {
       stop=false;
      }
    };

   struct ResultComplete : ProcessResult
    {
     explicit ResultComplete(Packet<uint8,Ext> packet_)
      {
       packet=packet_;
       stop=false;
      }
    };

   ProcessResult process_locked()
    {
     Mutex::Lock lock(mutex);

     Packet<uint8,Ext> packet=list.get();

     if( !packet )
       {
        if( !run_flag ) return ResultStop();
       }
     else
       {
        packet.getExt()->status=StatusWorking;

        if( packet.clearCancelFunction()==Packet_Cancelled )
          {
           stat.count(EventCancelOnGet);
          }
        else
          {
           if( process(packet) ) return ResultComplete(packet);
          }
       }

     return ResultContinue();
    }

   void do_process()
    {
     for(;;)
       {
        sem.take();

        if( process_locked().finish() ) return;
       }
    }

   void cancel_locked(Packet<uint8,Ext> packet)
    {
     Mutex::Lock lock(mutex);

     if( packet.getExt()->status==StatusPending )
       {
        stat.count(EventCancelPending);

        list.del(packet);
       }
     else
       {
        stat.count(EventCancelWorking);
       }

     packet.getExt()->status=StatusCancelled;
    }

   void cancel(PacketHeader *packet_)
    {
     Packet<uint8,Ext> packet=packet_;

     cancel_locked(packet);

     packet.complete();
    }

   Function<void (void)> function_process() { return FunctionOf(this,&DevBase::do_process); }

   PacketFunction function_cancel() { return FunctionOf(this,&DevBase::cancel); }

   bool put_locked(Packet<uint8,Ext> packet)
    {
     Mutex::Lock lock(mutex);

     if( packet.setCancelFunction(function_cancel())==Packet_Cancelled )
       {
        stat.count(EventCancelOnPut);

        packet.getExt()->status=StatusCancelled;

        return false;
       }
     else
       {
        packet.getExt()->status=StatusPending;

        list.put(packet);

        return true;
       }
    }

   void stop_locked()
    {
     Mutex::Lock lock(mutex);

     run_flag=false;
    }

  protected:

   PacketFormat format;
   TextLabel name;

  public:

   DevBase(TextLabel name_)
    : name(name_)
    {
     run_flag=true;
    }

   ~DevBase()
    {
    }

   PacketFormat getFormat() const { return format; }

   void put(Packet<uint8,Ext> packet)
    {
     event_locked(EventPut);

     if( packet.checkRange(format) )
       {
        if( put_locked(packet) )
          sem.give();
        else
          packet.complete();
       }
     else
       {
        packet.getExt()->status=StatusBadFormat;

        packet.complete();
       }
    }

   void do_run()
    {
     run_flag=true;

     RunFuncTask(function_process(),stop_sem.function_give());
    }

   void do_stop()
    {
     stop_locked();

     sem.give();

     stop_sem.take();

     Printf(Con,"#; stat\n\n#15l;\n\n",name,stat);
    }
 };

/* functions */

uint32 LoadUInt(PtrLen<uint8> data) // data.len == 4
 {
  UIntSplit<uint32,uint8> split;

  split[0]=data[0];
  split[1]=data[1];
  split[2]=data[2];
  split[3]=data[3];

  return split.get();
 }

void SaveUInt(uint32 value,PtrLen<uint8> data) // data.len == 4
 {
  UIntSplit<uint32,uint8> split;

  split.set(value);

  data[0]=split[0];
  data[1]=split[1];
  data[2]=split[2];
  data[3]=split[3];
 }

/* class Code */

class Code : NoCopy
 {
   uint32 mod;
   uint32 value;

   uint32 table[256];

  private:

   uint32 div(uint8 top)
    {
     return table[top];
    }

   static uint32 Mul2(uint32 t,uint32 mod)
    {
     if( t&0x8000'0000u )
       return (t<<1)^mod;
     else
       return (t<<1);
    }

  public:

   explicit Code(uint32 mod_)
    : mod(mod_),
      value(0)
    {
     table[0]=0;
     table[1]=mod;

     for(unsigned t=2; t<256u ;t<<=1)
       table[t]=Mul2(table[t>>1],mod);

     for(unsigned t=3; t<256u ;t++)
       table[t]=table[t&1u  ]^table[t&2u  ]^table[t&4u  ]^table[t&8u  ]^
                table[t&16u ]^table[t&32u ]^table[t&64u ]^table[t&128u];                     ;
    }

   void add(uint8 byte)
    {
     value=(value<<8)^byte^div(value>>24);
    }

   template <class R>
   void addRange(R r)
    {
     for(; +r ;++r) add(*r);
    }

   operator uint32() const { return value; }
 };

/* class CheckDev */

class CheckDev : public DevBase
 {
   Status check(PtrLen<uint8> prefix,PtrLen<uint8> data,PtrLen<uint8> suffix)
    {
     Code code(LoadUInt(prefix));
     uint32 result=LoadUInt(suffix);

     code.addRange(data);

     if( code==result ) return StatusOk;

     return StatusFail;
    }

   Status check(PtrLen<uint8> data)
    {
     if( data.len<8 ) return StatusBadFormat;

     return check(data.prefix(4),data.inner(4,4),data.suffix(4));
    }

   virtual bool process(Packet<uint8,Ext> packet)
    {
     packet.getExt()->status=check(packet.getRange());

     return true;
    }

  public:

   CheckDev()
    : DevBase("CheckDev")
    {
     format.prefix=0;
     format.suffix=0;
     format.max_data=1000;
    }

   ~CheckDev()
    {
    }

   class Run : NoCopy
    {
      CheckDev &dev;

     public:

      explicit Run(CheckDev &dev_) : dev(dev_) { dev_.do_run(); }

      ~Run() { dev.do_stop(); }
    };
 };

/* class SignDev */

class SignDev : public DevBase
 {
   CheckDev dev;

   PlatformRandom random;

  private:

   void sign(PtrLen<uint8> prefix,PtrLen<uint8> data,PtrLen<uint8> suffix)
    {
     uint32 mod=random.next32();

     Code code(mod);

     code.addRange(data);

     SaveUInt(mod,prefix);
     SaveUInt(code,suffix);
    }

   virtual bool process(Packet<uint8,Ext> packet)
    {
     sign(packet.getPrefix(format),packet.getRange(format),packet.getSuffix(format));

     dev.put(packet);

     return false;
    }

  public:

   SignDev()
    : DevBase("SignDev")
    {
     format=dev.getFormat().addPrefixSuffix(4,4);
    }

   ~SignDev()
    {
    }

   class Run : CheckDev::Run
    {
      SignDev &dev;

     public:

      explicit Run(SignDev &dev_) : CheckDev::Run(dev_.dev),dev(dev_) { dev_.do_run(); }

      ~Run() { dev.do_stop(); }
    };
 };

/* class FillDev */

class FillDev : public DevBase
 {
   SignDev dev;

   PlatformRandom random;

  private:

   virtual bool process(Packet<uint8,Ext> packet)
    {
     random.fill(packet.getRange(format));

     dev.put(packet);

     return false;
    }

  public:

   FillDev()
    : DevBase("FillDev")
    {
     format=dev.getFormat();
    }

   ~FillDev()
    {
    }

   class Run : SignDev::Run
    {
      FillDev &dev;

     public:

      explicit Run(FillDev &dev_) : SignDev::Run(dev_.dev),dev(dev_) { dev_.do_run(); }

      ~Run() { dev.do_stop(); }
    };
 };

/* class Engine */

class Engine : public Funchor_nocopy
 {
   FillDev dev;

   PlatformRandom random;
   PacketSet<uint8> pset;

   Mutex mutex;

   Counters<Status,StatusLim> stat;

  private:

   void status(Status st)
    {
     Mutex::Lock lock(mutex);

     stat.count(st);
    }

   void complete(PacketHeader *packet_)
    {
     Packet<uint8,Ext> packet=packet_;

     status(packet.getExt()->status);

     packet.popExt().complete();
    }

   PacketFunction function_complete() { return FunctionOf(this,&Engine::complete); }

   Packet<uint8,Ext> prepare(Packet<uint8> packet_)
    {
     Packet<uint8,Ext> packet=packet_.pushExt<Ext>();

     packet.pushCompleteFunction(function_complete());

     return packet;
    }

   void process(Packet<uint8> packet)
    {
     auto result=packet.getMaxDataLen(dev.getFormat());

     if( result.isTooShort() )
       {
        status(StatusPacketIsShort);

        packet.complete();
       }
     else
       {
        status(StatusPacket);

        ulen max_len=result.len;

        ulen len=random.select(0,max_len);

        packet.setDataLen(dev.getFormat(),len);

        dev.put(prepare(packet));
       }
    }

   void step(ulen pcount)
    {
     TimeScope scope(5_msec);

     for(; pcount ;pcount--)
       {
        Packet<uint8> packet=pset.get(scope);

        if( !packet )
          {
           status(StatusNoPacket);

           break;
          }

        process(packet);
       }

     pset.wait(scope);
    }

   void step_ext(ulen pcount)
    {
     TimeScope scope(5_msec);
     PacketSet<uint8>::ExtTop list;

     for(; pcount ;pcount--)
       {
        Packet<uint8> packet=pset.get(list,scope);

        if( !packet )
          {
           status(StatusNoPacket);

           break;
          }

        process(packet);
       }

     pset.wait(scope);
    }

   void step_ext2(ulen pcount)
    {
     TimeScope scope(5_msec);
     PacketSet<uint8>::ExtTop list;

     for(; pcount ;pcount--)
       {
        Packet<uint8> packet=pset.get(list,scope);

        if( !packet )
          {
           status(StatusNoPacket);

           break;
          }

        process(packet);
       }

     pset.cancel(list);

     pset.wait(scope);
    }

  public:

   Engine()
    {
    }

   ~Engine()
    {
    }

   void run(ulen count)
    {
     FillDev::Run run(dev);

     for(; count ;count--)
       {
        step(random.select(10,50));
       }
    }

   void run_ext(ulen count)
    {
     FillDev::Run run(dev);

     for(; count ;count--)
       {
        step_ext(random.select(10,50));
       }
    }

   void run_ext2(ulen count)
    {
     FillDev::Run run(dev);

     for(; count ;count--)
       {
        step_ext2(random.select(10,50));
       }
    }

   void showStatus()
    {
     Printf(Con,"#15l;\n",stat);

     stat.reset();
    }
 };

} // namespace Private_3009

using namespace Private_3009;

/* Testit<3009> */

template<>
const char *const Testit<3009>::Name="Test3009 PacketSet";

template<>
bool Testit<3009>::Main()
 {
  Engine engine;

  Printf(Con,"#;\n\n",Title("run"));

  engine.run(10'000);

  engine.showStatus();

  Printf(Con,"#;\n\n",Title("run_ext"));

  engine.run_ext(10'000);

  engine.showStatus();

  Printf(Con,"#;\n\n",Title("run_ext2"));

  engine.run_ext2(10'000);

  engine.showStatus();

  return true;
 }

} // namespace App

