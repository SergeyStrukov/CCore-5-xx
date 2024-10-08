/* EventRecorder.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_EventRecorder_h
#define CCore_inc_EventRecorder_h

#include <CCore/inc/task/Atomic.h>
#include <CCore/inc/Timer.h>
#include <CCore/inc/Array.h>
#include <CCore/inc/String.h>
#include <CCore/inc/FunctorType.h>
#include <CCore/inc/TreeMap.h>
#include <CCore/inc/SaveLoad.h>
#include <CCore/inc/TextLabel.h>
#include <CCore/inc/algon/ApplyToRange.h>

namespace CCore {

/* consts */

inline constexpr ulen MaxEventLen = 256 ;

inline constexpr unsigned DefaultGuardCount = 1'000'000 ;

/* types */

using EventTimeType = uint32 ;

using EventIdType = uint16 ;

/* functions */

void WaitAtomicZero(Atomic &count);

/* classes */

//enum EventMarker;

/* enum EventMarker */

enum EventMarker
 {
  EventMarker_None,

  EventMarker_Up,
  EventMarker_Down,

  EventMarker_UpBlock,
  EventMarker_DownBlock,

  EventMarker_UpUp,
  EventMarker_UpPush,
  EventMarker_UpUpPush,

  EventMarker_Push,

  EventMarker_Inc,
  EventMarker_Dec,

  EventMarker_Wait,
  EventMarker_Pass,

  EventMarker_Tick,

  EventMarker_Stop,

  EventMarker_Error
 };

const char * GetTextDesc(EventMarker marker);

/* concept EventValueType<T> */

template <class T,class V>
concept EventValueType2 = OneOfTypes<V,uint8,uint16,uint32> && requires()
 {
  Ground<V>( T::Base );
  Ground<V>( T::Lim );

  requires ( T::Base < T::Lim );

  Ground<EventMarker>( T::Marker );
 } ;

template <class T>
concept EventValueType = requires()
 {
  typename T::ValueType;

  requires EventValueType2<T,typename T::ValueType> ;
 } ;

//----------------------------------------------------------------------------------------

/* classes */

struct EventRecordPos;

struct EventPrefix;

template <EventValueType T> class EventEnumValue;

template <auto Field> struct OffsetFuncCtor;

class EventMetaInfo;

/* struct EventRecordPos */

struct EventRecordPos
 {
  ulen pos;
  EventTimeType time;
 };

/* struct EventPrefix */

struct EventPrefix // each event type must be layout-compatible with EventPrefix
 {
  EventTimeType time;
  EventIdType id;
 };

/* class EventEnumValue<T> */

 //
 // struct Value
 //  {
 //   using ValueType = uint16 ;
 //
 //   static constexpr ValueType Base = 10 ;
 //   static constexpr ValueType Lim = 100 ;
 //   static constexpr EventMarker Marker = EventMarker_None ;
 //  };
 //

template <EventValueType T>
class EventEnumValue
 {
   using ValueType = typename T::ValueType ;

   static constexpr ulen Len = UIntConstMul<ulen,T::Lim-T::Base,sizeof (TextLabel)> ;

   ValueType value;

  private:

   static Sys::Atomic Next;
   static Sys::Atomic Total;

   static InitStorage<Len> Storage;

   static ValueType Reserve(TextLabel name);

  public:

   explicit EventEnumValue(TextLabel name) : value(Reserve(name)) {}

   operator ValueType() const { return value; }

   template <class Desc>
   static void Append(Desc &desc);
 };

template <EventValueType T>
Sys::Atomic EventEnumValue<T>::Next{T::Base};

template <EventValueType T>
Sys::Atomic EventEnumValue<T>::Total{0};

template <EventValueType T>
InitStorage<EventEnumValue<T>::Len> EventEnumValue<T>::Storage;

template <EventValueType T>
auto EventEnumValue<T>::Reserve(TextLabel name) -> ValueType
 {
  ValueType ret=Next++;

  if( ret<T::Lim )
    {
     new(Storage.getPlace()+(ret-T::Base)*sizeof (TextLabel)) TextLabel(name);

     Total++;

     return ret;
    }

  Next--;

  return T::Lim;
 }

template <EventValueType T>
template <class Desc>
void EventEnumValue<T>::Append(Desc &desc)
 {
  ValueType count=Total;

  for(ValueType val=0; val<count ;val++)
    {
     TextLabel *name=Storage.getPlace()+val*sizeof (TextLabel);

     desc.addValueName(val+T::Base,StringCat(*name),T::Marker);
    }
 }

/* struct OffsetFuncCtor<Field> */

template <class T,class F,F T::* Field>
struct OffsetFuncCtor<Field>
 {
  static void * Func(void *ptr_)
   {
    T *ptr=static_cast<T *>(ptr_);

    return &(ptr->*Field);
   }
 };

template <auto Field>
constexpr auto OffsetFuncFor = OffsetFuncCtor<Field>::Func ;

/* class EventMetaInfo */

class EventMetaInfo : NoCopy
 {
  public:

   enum Kind
    {
     Kind_uint8,
     Kind_uint16,
     Kind_uint32,

     Kind_enum_uint8,
     Kind_enum_uint16,
     Kind_enum_uint32,

     Kind_struct
    };

   friend const char * GetTextDesc(Kind kind);

   static uint32 MaxValue(Kind kind)
    {
     switch( kind )
       {
        case Kind_enum_uint8 : return MaxUInt<uint8>;

        case Kind_enum_uint16 : return MaxUInt<uint16>;

        default: return MaxUInt<uint32>;
       }
    }

   static void Save(const String &name,SaveDevType auto &dev)
    {
     dev.template use<BeOrder>((uint32)name.getLen());

     dev.put(Mutate<const uint8>(Range(name)));
    }

   template <class Array>
   static void SaveArray(const Array &array,SaveDevType auto &dev)
    {
     dev.template use<BeOrder>((uint32)array.getLen());

     SaveRange_use<BeOrder>(Range(array),dev);
    }

   struct ValueDesc
    {
     String name;
     EventMarker marker;

     // constructors

     ValueDesc(const String &name_,EventMarker marker_) : name(name_),marker(marker_) {}

     // save/load object

     void save(SaveDevType auto &dev) const
      {
       Save(name,dev);

       dev.template use<BeOrder>((uint8)marker);
      }

     // print object

     void print(PrinterType auto &out,uint32 value) const
      {
       if( marker!=EventMarker_None )
         Printf(out,"  #; = #; [#;]\n",name,value,marker);
       else
         Printf(out,"  #; = #;\n",name,value);
      }
    };

   class EnumDesc : NoCopy
    {
      using AppendFunc = void (*)(EnumDesc &desc) ;

      EventIdType id;
      String name;
      Kind kind;
      AppendFunc append_func;

      RadixTreeMap<uint32,ValueDesc> map;

     public:

      // constructors

      EnumDesc(EventIdType id_,const String &name_,Kind kind_)
       : id(id_),name(name_),kind(kind_),append_func(0),map(KeyRange<uint32>(MaxValue(kind_))) {}

      ~EnumDesc() {}

      // props

      EventIdType getId() const { return id; }

      const String & getName() const { return name; }

      Kind getKind() const { return kind; }

      const String * findValueName(uint32 value) const;

      EnumDesc & addValueName(uint32 value,const String &name,EventMarker marker=EventMarker_None);

      EnumDesc & setAppendFunc(AppendFunc append_func_) { append_func=append_func_; return *this; }

      void append()
       {
        if( append_func )
          {
           append_func(*this);

           append_func=0;
          }
       }

      // save/load object

      void save(SaveDevType auto &dev) const
       {
        dev.template use<BeOrder>((uint8)kind);

        Save(name,dev);

        dev.template use<BeOrder>((uint32)map.getCount());

        map.applyIncr( [&dev] (uint32 value,const ValueDesc &desc) { dev.template use<BeOrder>(value,desc); } );
       }

      // swap/move objects

      void objSwap(EnumDesc &obj)
       {
        Swap(id,obj.id);
        Swap(name,obj.name);
        Swap(kind,obj.kind);
        Swap(append_func,obj.append_func);
        Swap(map,obj.map);
       }

      explicit EnumDesc(ToMoveCtor<EnumDesc> obj)
       : id(obj->id),
         name(ToMoveCtor(obj->name)),
         kind(obj->kind),
         append_func(obj->append_func),
         map(ToMoveCtor(obj->map))
       {
       }

      // print object

      void print(PrinterType auto &out) const
       {
        Printf(out,"#; #;\n {\n",kind,name);

        map.applyIncr( [&out] (uint32 value,const ValueDesc &desc) { desc.print(out,value); } );

        Printf(out," }\n\n");
       }

      void print(PrinterType auto &out,uint32 value) const
       {
        if( const String *name=findValueName(value) )
          {
           Putobj(out,*name);
          }
        else
          {
           Putobj(out,value);
          }
       }
    };

   using OffsetFunc = void * (*)(void *) ;

   class FieldDesc : NoCopy
    {
      Kind kind;
      EventIdType id;
      String name;
      OffsetFunc offset;

     public:

      // constructors

      FieldDesc(Kind kind_,EventIdType id_,const String &name_,OffsetFunc offset_) : kind(kind_),id(id_),name(name_),offset(offset_) {}

      ~FieldDesc() {}

      // save/load object

      void save(SaveDevType auto &dev) const
       {
        dev.template use<BeOrder>((uint8)kind,id);

        Save(name,dev);
       }

      void save(SaveDevType auto &dev,const EventMetaInfo &info,void *ptr) const
       {
        ptr=offset(ptr);

        switch( kind )
          {
           case Kind_uint8 :
           case Kind_enum_uint8 :
            {
             dev.template use<BeOrder>(*static_cast<uint8 *>(ptr));
            }
           break;

           case Kind_uint16 :
           case Kind_enum_uint16 :
            {
             dev.template use<BeOrder>(*static_cast<uint16 *>(ptr));
            }
           break;

           case Kind_uint32 :
           case Kind_enum_uint32 :
            {
             dev.template use<BeOrder>(*static_cast<uint32 *>(ptr));
            }
           break;

           case Kind_struct :
            {
             info.getStruct(id).save(dev,info,ptr);
            }
           break;
          }
       }

      uint32 saveLen(const EventMetaInfo &info) const
       {
        switch( kind )
          {
           case Kind_uint8 :
           case Kind_enum_uint8 :
            {
             return (uint32)SaveLenCounter<uint8>::SaveLoadLen;
            }
           break;

           case Kind_uint16 :
           case Kind_enum_uint16 :
            {
             return (uint32)SaveLenCounter<uint16>::SaveLoadLen;
            }
           break;

           case Kind_uint32 :
           case Kind_enum_uint32 :
            {
             return (uint32)SaveLenCounter<uint32>::SaveLoadLen;
            }
           break;

           case Kind_struct :
            {
             return info.getStruct(id).saveLen(info);
            }
           break;
          }

        return 0;
       }

      // swap/move objects

      void objSwap(FieldDesc &obj)
       {
        Swap(kind,obj.kind);
        Swap(id,obj.id);
        Swap(name,obj.name);
        Swap(offset,obj.offset);
       }

      explicit FieldDesc(ToMoveCtor<FieldDesc> obj)
       : kind(obj->kind),
         id(obj->id),
         name(ToMoveCtor(obj->name)),
         offset(obj->offset)
       {
       }

      // print object

      void print(PrinterType auto &out,const EventMetaInfo &info) const
       {
        switch( kind )
          {
           case Kind_uint8 : Printf(out,"  uint8"); break;
           case Kind_uint16 : Printf(out,"  uint16"); break;
           case Kind_uint32 : Printf(out,"  uint32"); break;

           case Kind_enum_uint8 : Printf(out,"  enum uint8 #;",info.getEnum(id).getName()); break;
           case Kind_enum_uint16 : Printf(out,"  enum uint16 #;",info.getEnum(id).getName()); break;
           case Kind_enum_uint32 : Printf(out,"  enum uint32 #;",info.getEnum(id).getName()); break;

           case Kind_struct : Printf(out,"  struct #;",info.getStruct(id).getName()); break;
          }

        Printf(out," #;;\n",name);
       }

      void print(PrinterType auto &out,const EventMetaInfo &info,void *ptr,ulen off) const
       {
        ptr=offset(ptr);

        out.put(' ',off);

        switch( kind )
          {
           case Kind_uint8 :
            {
             Printf(out,"  uint8 #; = #; ;\n",name,*static_cast<uint8 *>(ptr));
            }
           break;

           case Kind_uint16 :
            {
             Printf(out,"  uint16 #; = #; ;\n",name,*static_cast<uint16 *>(ptr));
            }
           break;

           case Kind_uint32 :
            {
             Printf(out,"  uint32 #; = #; ;\n",name,*static_cast<uint32 *>(ptr));
            }
           break;

           case Kind_enum_uint8 :
            {
             Printf(out,"  enum uint8 #; = ",name);

             info.getEnum(id).print(out,*static_cast<uint8 *>(ptr));

             Printf(out," ;\n");
            }
           break;

           case Kind_enum_uint16 :
            {
             Printf(out,"  enum uint16 #; = ",name);

             info.getEnum(id).print(out,*static_cast<uint16 *>(ptr));

             Printf(out," ;\n");
            }
           break;

           case Kind_enum_uint32 :
            {
             Printf(out,"  enum uint32 #; = ",name);

             info.getEnum(id).print(out,*static_cast<uint32 *>(ptr));

             Printf(out," ;\n");
            }
           break;

           case Kind_struct :
            {
             auto &desc=info.getStruct(id);

             Printf(out,"  struct #; #; =\n",desc.getName(),name);

             out.put(' ',off);

             Printf(out,"   {\n");

             desc.print(out,info,ptr,off+2);

             out.put(' ',off);

             Printf(out,"   };\n");
            }
           break;
          }
       }
    };

   class StructDesc : NoCopy
    {
      EventIdType id;
      String name;
      DynArray<FieldDesc> field_list;

     public:

      // constructors

      StructDesc(EventIdType id_,const String &name_) : id(id_),name(name_) {}

      ~StructDesc() {}

      // props

      EventIdType getId() const { return id; }

      const String & getName() const { return name; }

      PtrLen<const FieldDesc> getFieldList() const { return Range(field_list); }

      // add field

      StructDesc & addField(Kind kind,EventIdType id,const String &name,OffsetFunc offset) { field_list.append_fill(kind,id,name,offset); return *this; }

      StructDesc & addField_uint8(const String &name,OffsetFunc offset) { return addField(Kind_uint8,0,name,offset); }

      StructDesc & addField_uint16(const String &name,OffsetFunc offset) { return addField(Kind_uint16,0,name,offset); }

      StructDesc & addField_uint32(const String &name,OffsetFunc offset) { return addField(Kind_uint32,0,name,offset); }

      StructDesc & addField_enum_uint8(EventIdType id,const String &name,OffsetFunc offset) { return addField(Kind_enum_uint8,id,name,offset); }

      StructDesc & addField_enum_uint16(EventIdType id,const String &name,OffsetFunc offset) { return addField(Kind_enum_uint16,id,name,offset); }

      StructDesc & addField_enum_uint32(EventIdType id,const String &name,OffsetFunc offset) { return addField(Kind_enum_uint32,id,name,offset); }

      StructDesc & addField_struct(EventIdType id,const String &name,OffsetFunc offset) { return addField(Kind_struct,id,name,offset); }

      // add field by address

      template <auto Field>
      StructDesc & addField(Kind kind,EventIdType id,const String &name) { return addField(kind,id,name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_uint8(const String &name) { return addField_uint8(name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_uint16(const String &name) { return addField_uint16(name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_uint32(const String &name) { return addField_uint32(name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_enum_uint8(EventIdType id,const String &name) { return addField_enum_uint8(id,name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_enum_uint16(EventIdType id,const String &name) { return addField_enum_uint16(id,name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_enum_uint32(EventIdType id,const String &name) { return addField_enum_uint32(id,name,OffsetFuncFor<Field>); }

      template <auto Field>
      StructDesc & addField_struct(EventIdType id,const String &name) { return addField_struct(id,name,OffsetFuncFor<Field>); }

      // save/load object

      void save(SaveDevType auto &dev) const
       {
        Save(name,dev);

        SaveArray(field_list,dev);
       }

      void save(SaveDevType auto &dev,const EventMetaInfo &info,void *ptr) const
       {
        for(const auto &desc : field_list ) desc.save(dev,info,ptr);
       }

      uint32 saveLen(const EventMetaInfo &info) const
       {
        uint32 ret=0;

        for(const auto &desc : field_list ) ret+=desc.saveLen(info);

        return ret;
       }

      // swap/move objects

      void objSwap(StructDesc &obj)
       {
        Swap(id,obj.id);
        Swap(name,obj.name);
        Swap(field_list,obj.field_list);
       }

      explicit StructDesc(ToMoveCtor<StructDesc> obj)
       : id(obj->id),
         name(ToMoveCtor(obj->name)),
         field_list(ToMoveCtor(obj->field_list))
       {
       }

      // print object

      void print(PrinterType auto &out,const EventMetaInfo &info) const
       {
        Printf(out,"struct #;\n {\n",name);

        for(const auto &desc : field_list ) desc.print(out,info);

        Printf(out," }\n\n");
       }

      void print(PrinterType auto &out,const EventMetaInfo &info,void *ptr,ulen off=0) const
       {
        if( !off ) Printf(out,"event #;\n {\n",name);

        for(const auto &desc : field_list ) desc.print(out,info,ptr,off);

        if( !off ) Printf(out," }\n\n");
       }
    };

   class EventDesc
    {
      EventIdType id;
      ulen alloc_len;
      EventIdType class_id;
      EventIdType struct_id;
      uint32 save_len;
      EventIdType (*class_id_func)();

     public:

      // constructors

      EventDesc(EventIdType id_,ulen alloc_len_,EventIdType class_id_)
       : id(id_),alloc_len(alloc_len_),class_id(class_id_),struct_id(0),save_len(0),class_id_func(0) {}

      ~EventDesc() {}

      // props

      EventIdType getId() const { return id; }

      ulen getAllocLen() const { return alloc_len; }

      EventIdType getClassId() const { return class_id; }

      EventIdType getStructId() const { return struct_id; }

      void setStructId(const EventMetaInfo &info,EventIdType struct_id_)
       {
        save_len=info.getStruct(struct_id_).saveLen(info);
        struct_id=struct_id_;
       }

      template <class T>
      void classId();

      void setClassId() { if( class_id_func ) class_id=class_id_func(); }

      // save/load object

      void save(SaveDevType auto &dev) const
       {
        dev.template use<BeOrder>(struct_id,class_id,save_len);
       }

      void save(SaveDevType auto &dev,const EventMetaInfo &info,void *ptr) const
       {
        info.getStruct(struct_id).save(dev,info,ptr);
       }
    };

  private:

   uint64 time_freq;

   DynArray<EnumDesc> enum_list;
   DynArray<StructDesc> struct_list;
   DynArray<EventDesc> event_list;

  private:

   static EventIdType IndexToId(ulen index);

  public:

   explicit EventMetaInfo(uint64 time_freq);

   ~EventMetaInfo();

   // add type

   EnumDesc & addEnum(Kind kind,const String &name);

   EnumDesc & addEnum_uint8(const String &name) { return addEnum(Kind_enum_uint8,name); }

   EnumDesc & addEnum_uint16(const String &name) { return addEnum(Kind_enum_uint16,name); }

   EnumDesc & addEnum_uint32(const String &name) { return addEnum(Kind_enum_uint32,name); }

   StructDesc & addStruct(const String &name);

   EventDesc & addEvent(ulen alloc_len);

   void setClassId();

   void appendEnums();

   // meta info

   const EnumDesc & getEnum(EventIdType id) const;

   const StructDesc & getStruct(EventIdType id) const;

   const EventDesc & getEvent(EventIdType id) const;

   // save/load object

   void save(SaveDevType auto &dev) const
    {
     dev.template use<BeOrder>(time_freq);

     SaveArray(enum_list,dev);
     SaveArray(struct_list,dev);
     SaveArray(event_list,dev);
    }

   void saveEvent(SaveDevType auto &dev,const EventDesc &desc,void *ptr) const
    {
     desc.save(dev,*this,ptr);
    }

   // print object

   void print(PrinterType auto &out) const
    {
     Printf(out,"#;\n\nfreq = #;\n\n",Title("Event record"_c),time_freq);

     for(const auto &desc : enum_list )
       {
        Putobj(out,desc);
       }

     for(const auto &desc : struct_list )
       {
        desc.print(out,*this);
       }

     for(const auto &desc : event_list )
       {
        Printf(out,"event #;\n\n",getStruct(desc.getStructId()).getName());
       }

     Printf(out,"#;\n\n",TextDivider());
    }

   void printEvent(PrinterType auto &out,const EventDesc &desc,void *ptr) const
    {
     getStruct(desc.getStructId()).print(out,*this,ptr);
    }
 };

/* concept EventType<T> */

template <class T>
concept EventType = PODType<T> && requires()
 {
  Ground<EventTimeType T::*>( &T::time );

  Ground<EventIdType T::*>( &T::id );

  Ground< void (*)(EventMetaInfo &info,EventMetaInfo::EventDesc &desc) >( &T::Register );
 } ;

/* concept EventInitArg<T,SS> */

template <class T,class ... SS>
concept EventInitArg = EventType<T> && requires(T &obj,EventTimeType time,EventIdType id,SS && ... ss)
 {
  obj.init(time,id, std::forward<SS>(ss)... );
 } ;

/* concept RegisterEventType<T> */

template <class T>
concept RegisterEventType = requires(EventMetaInfo &info)
 {
  Ground< EventIdType (*)(EventMetaInfo &info) >( &T::Register );
 } ;

//----------------------------------------------------------------------------------------

/* classes */

class EventIdNode;

template <EventType T> class EventId;

class EventTypeIdNode;

template <RegisterEventType T> class EventTypeId;

struct EventControl;

template <class Algo> class EventRecorder;

template <class Recorder,unsigned GuardCount=DefaultGuardCount> class EventRecorderHost;

/* class EventIdNode */

class EventIdNode
 {
  public:

   using RegisterFunc = void (*)(EventMetaInfo &info,EventMetaInfo::EventDesc &desc) ;

  private:

   EventIdNode *next;
   EventIdType id;
   ulen size_of;
   RegisterFunc func;

   static EventIdNode * List;

  private:

   void reg(EventMetaInfo &info,ulen align);

  public:

   EventIdNode(RegisterFunc func,ulen size_of);

   EventIdType getId() const { return id; }

   static void Register(EventMetaInfo &info,ulen align);
 };

/* class EventId<T> */

template <EventType T>
class EventId
 {
   static EventIdNode Node;

  public:

   static EventIdType GetId() { return Node.getId(); }
 };

template <EventType T>
EventIdNode EventId<T>::Node(T::Register,sizeof (T));

template <class T>
void EventMetaInfo::EventDesc::classId() { class_id_func=&EventId<T>::GetId; }

/* class EventTypeIdNode */

class EventTypeIdNode
 {
  public:

   using RegisterFunc = EventIdType (*)(EventMetaInfo &info) ;

  private:

   enum State
    {
     None,
     InProgress,
     Done
    };

   EventTypeIdNode *next;
   RegisterFunc func;
   EventIdType id;
   State state;

   static EventTypeIdNode * List;

  public:

   explicit EventTypeIdNode(RegisterFunc func);

   EventIdType getId() const { return id; }

   EventIdType getId(EventMetaInfo &info);

   static void Register(EventMetaInfo &info);
 };

/* class EventTypeId<T> */

template <RegisterEventType T>
class EventTypeId
 {
   static EventTypeIdNode Node;

  public:

   static EventIdType GetId() { return Node.getId(); }

   static EventIdType GetId(EventMetaInfo &info) { return Node.getId(info); }
 };

template <RegisterEventType T>
EventTypeIdNode EventTypeId<T>::Node(T::Register);

/* struct EventControl */

struct EventControl
 {
  EventTimeType time;
  EventIdType id;
  uint8 type;

  enum Type
   {
    Type_Start,
    Type_Tick,
    Type_Stop,
    Type_End
   };

  void init(EventTimeType time_,EventIdType id_,Type type_)
   {
    time=time_;
    id=id_;

    type=type_;
   }

  static void Register(EventMetaInfo &info,EventMetaInfo::EventDesc &desc);
 };

/* class EventRecorder<Algo> */

 //
 // struct EventRecorderAlgo
 //  {
 //   static constexpr ulen RecordAlign = 4 ;
 //
 //   static constexpr uint64 TimeFreq = ??? ;
 //
 //   class AllocPos : NoCopy
 //    {
 //      ulen off;
 //
 //     public:
 //
 //      AllocPos() : off(0) {}
 //
 //      ~AllocPos() {}
 //
 //      operator ulen() const { return off; }
 //
 //      EventRecordPos alloc(ulen len);
 //
 //      void back(ulen len);
 //    };
 //  };
 //

template <class Algo>
class EventRecorder : public EventMetaInfo
 {
   void *buf;
   ulen buf_len;

   typename Algo::AllocPos pos;

  public:

   static constexpr ulen RecordAlign = Algo::RecordAlign ;

   explicit EventRecorder(ulen buf_len_)
    : EventMetaInfo(Algo::TimeFreq)
    {
     Replace_max(buf_len_,MaxEventLen);

     buf=MemAlloc(buf_len_);
     buf_len=buf_len_;
    }

   ~EventRecorder()
    {
     MemFree(buf);
    }

   void setTime(EventTimeType t) { pos.setTime(t); }

   template <class T,class ... SS>
   void add(SS && ... ss) requires ( EventInitArg<T,SS...> )
    {
     static_assert( RecordAlign%alignof(T)==0 ,"CCore::EventRecorder<Algo>::add(...) : T has too large alignment");

     const ulen len=Align(sizeof (T),RecordAlign);

     static_assert( len<=MaxEventLen ,"CCore::EventRecorder<Algo>::add(...) : T is too large");

     EventRecordPos recpos=pos.alloc(len);

     if( recpos.pos>buf_len-len )
       {
        pos.back(len);
       }
     else
       {
        static_cast<T *>(PtrAdd(buf,recpos.pos))->init( recpos.time , EventId<T>::GetId() , std::forward<SS>(ss)... );
       }
    }

   template <FuncInitArgType<const EventRecorder &,const EventDesc &,void *> FuncInit>
   void parse(FuncInit func_init) const
    {
     ulen off=0;
     ulen lim=pos;

     FunctorTypeOf<FuncInit> func(func_init);

     while( off<lim )
       {
        void *ptr=PtrAdd(buf,off);
        EventPrefix *prefix=static_cast<EventPrefix *>(ptr);

        auto &desc=getEvent(prefix->id);

        func(*this,desc,ptr);

        off+=desc.getAllocLen();
       }
    }

   // save/load object

   void save(SaveDevType auto &dev) const
    {
     EventMetaInfo::save(dev);

     parse( [&] (const EventMetaInfo &info,const EventDesc &desc,void *ptr) { info.saveEvent(dev,desc,ptr); } );

     EventControl end;

     end.init(0,EventId<EventControl>::GetId(),EventControl::Type_End);

     saveEvent(dev,getEvent(end.id),&end);
    }

   // print object

   void print(PrinterType auto &out) const
    {
     EventMetaInfo::print(out);

     parse( [&] (const EventMetaInfo &info,const EventDesc &desc,void *ptr) { info.printEvent(out,desc,ptr); } );

     Printf(out,"#;\n\n",TextDivider());
    }
 };

/* class EventRecorderHost<Recorder,unsigned GuardCount> */

template <class Recorder,unsigned GuardCount>
class EventRecorderHost : NoCopy
 {
   Recorder *obj;
   Atomic count;

  private:

   Recorder * lock()
    {
     if( (count++)<GuardCount )
       {
        count--;

        return 0;
       }
     else
       {
        return obj;
       }
    }

   void unlock()
    {
     count--;
    }

   void start(Recorder *obj_)
    {
     obj=obj_;

     obj_->template add<EventControl>(EventControl::Type_Start);

     count+=GuardCount;
    }

   void stop()
    {
     count-=GuardCount;

     Recorder *temp=Replace_null(obj);

     WaitAtomicZero(count);

     temp->template add<EventControl>(EventControl::Type_Stop);

     temp->appendEnums();
    }

  public:

   EventRecorderHost()
    {
     obj=0;
     count=0;
    }

   class StartStop : NoCopy
    {
      EventRecorderHost &host;

     public:

      StartStop(EventRecorderHost &host_,Recorder *obj)
       : host(host_)
       {
        EventTypeIdNode::Register(*obj);

        EventIdNode::Register(*obj,Recorder::RecordAlign);

        host_.start(obj);
       }

      ~StartStop()
       {
        host.stop();
       }
    };

   template <class T,class ... SS>
   void add(SS && ... ss) requires ( EventInitArg<T,SS...> )
    {
     if( Recorder *obj=lock() )
       {
        obj->template add<T>( std::forward<SS>(ss)... );

        unlock();
       }
    }

   void tick()
    {
     add<EventControl>(EventControl::Type_Tick);
    }
 };

} // namespace CCore

#endif


