/* LangData.TypeSet.gen.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied CondLangLR1
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

/* struct TypeSet */

struct TypeSet : TypeDefCore
 {
  ulen indexes[11];
  DynArray<ulen> ind_map;

  DDL::FindNodeMap map;

  explicit TypeSet(ulen len)
   : ind_map(len)
   {
    Range(indexes).set(ulen(-1));

    map.add(1,"Action","Final");
    map.add(2,"Final");
    map.add(3,"Transition","State");
    map.add(4,"State");
    map.add(5,"TopRule");
    map.add(6,"Rule");
    map.add(7,"Element");
    map.add(8,"Kind");
    map.add(9,"Synt");
    map.add(10,"Atom");
    map.add(11,"Lang");

    map.complete();
   }

  DDL::MapSizeInfo structSizeInfo(DDL::StructNode *struct_node)
   {
    DDL::MapSizeInfo ret;

    switch( map.find(struct_node) )
      {
       case 1 :
        {
         indexes[0]=struct_node->index;
         ind_map[struct_node->index]=1;

         ret.set<S1>();

         DDL::SetFieldOffsets(struct_node,
                               "atom",offsetof(S1,atom),
                               "rule",offsetof(S1,rule)
                              );
        }
       return ret;

       case 2 :
        {
         indexes[1]=struct_node->index;
         ind_map[struct_node->index]=2;

         ret.set<S2>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S2,index),
                               "actions",offsetof(S2,actions)
                              );
        }
       return ret;

       case 3 :
        {
         indexes[2]=struct_node->index;
         ind_map[struct_node->index]=3;

         ret.set<S3>();

         DDL::SetFieldOffsets(struct_node,
                               "element",offsetof(S3,element),
                               "state",offsetof(S3,state)
                              );
        }
       return ret;

       case 4 :
        {
         indexes[3]=struct_node->index;
         ind_map[struct_node->index]=4;

         ret.set<S4>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S4,index),
                               "final",offsetof(S4,final),
                               "transitions",offsetof(S4,transitions)
                              );
        }
       return ret;

       case 5 :
        {
         indexes[4]=struct_node->index;
         ind_map[struct_node->index]=5;

         ret.set<S5>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S5,index),
                               "name",offsetof(S5,name),
                               "bottom",offsetof(S5,bottom),
                               "result",offsetof(S5,result),
                               "args",offsetof(S5,args)
                              );
        }
       return ret;

       case 6 :
        {
         indexes[5]=struct_node->index;
         ind_map[struct_node->index]=6;

         ret.set<S6>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S6,index),
                               "name",offsetof(S6,name),
                               "result",offsetof(S6,result),
                               "args",offsetof(S6,args)
                              );
        }
       return ret;

       case 7 :
        {
         indexes[6]=struct_node->index;
         ind_map[struct_node->index]=7;

         ret.set<S7>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S7,index),
                               "elem",offsetof(S7,elem)
                              );
        }
       return ret;

       case 8 :
        {
         indexes[7]=struct_node->index;
         ind_map[struct_node->index]=8;

         ret.set<S8>();

         DDL::SetFieldOffsets(struct_node,
                               "kindex",offsetof(S8,kindex),
                               "index",offsetof(S8,index),
                               "name",offsetof(S8,name),
                               "synt",offsetof(S8,synt),
                               "element",offsetof(S8,element),
                               "rules",offsetof(S8,rules)
                              );
        }
       return ret;

       case 9 :
        {
         indexes[8]=struct_node->index;
         ind_map[struct_node->index]=9;

         ret.set<S9>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S9,index),
                               "name",offsetof(S9,name),
                               "kinds",offsetof(S9,kinds),
                               "rules",offsetof(S9,rules)
                              );
        }
       return ret;

       case 10 :
        {
         indexes[9]=struct_node->index;
         ind_map[struct_node->index]=10;

         ret.set<S10>();

         DDL::SetFieldOffsets(struct_node,
                               "index",offsetof(S10,index),
                               "name",offsetof(S10,name),
                               "element",offsetof(S10,element)
                              );
        }
       return ret;

       case 11 :
        {
         indexes[10]=struct_node->index;
         ind_map[struct_node->index]=11;

         ret.set<S11>();

         DDL::SetFieldOffsets(struct_node,
                               "atoms",offsetof(S11,atoms),
                               "synts",offsetof(S11,synts),
                               "lang",offsetof(S11,lang),
                               "elements",offsetof(S11,elements),
                               "rules",offsetof(S11,rules),
                               "top_rules",offsetof(S11,top_rules),
                               "states",offsetof(S11,states),
                               "finals",offsetof(S11,finals)
                              );
        }
       return ret;

       default: Printf(Exception,"Unknown structure"); return ret;
      }
   }

  template <class T> struct IsStruct;

  template <class T>
  bool isStruct(DDL::StructNode *struct_node) const { return IsStruct<T>::Do(indexes,struct_node->index); }

  void guardFieldTypes(DDL::StructNode *struct_node) const
   {
    switch( ind_map[struct_node->index] )
      {
       case 1 :
        {
         DDL::GuardFieldTypes<
                               DDL::MapPtr< S10 >,
                               DDL::MapPtr< S6 >
                              >(*this,struct_node);
        }
       break;

       case 2 :
        {
         DDL::GuardFieldTypes<
                               A3,
                               DDL::MapRange< S1 >
                              >(*this,struct_node);
        }
       break;

       case 3 :
        {
         DDL::GuardFieldTypes<
                               DDL::MapPtr< S7 >,
                               DDL::MapPtr< S4 >
                              >(*this,struct_node);
        }
       break;

       case 4 :
        {
         DDL::GuardFieldTypes<
                               A4,
                               DDL::MapPtr< S2 >,
                               DDL::MapRange< S3 >
                              >(*this,struct_node);
        }
       break;

       case 5 :
        {
         DDL::GuardFieldTypes<
                               A5,
                               DDL::MapText,
                               DDL::MapPtr< S6 >,
                               DDL::MapPtr< S8 >,
                               DDL::MapRange< A1 >
                              >(*this,struct_node);
        }
       break;

       case 6 :
        {
         DDL::GuardFieldTypes<
                               A5,
                               DDL::MapText,
                               DDL::MapPtr< S8 >,
                               DDL::MapRange< A2 >
                              >(*this,struct_node);
        }
       break;

       case 7 :
        {
         DDL::GuardFieldTypes<
                               A6,
                               DDL::MapPolyPtr< S10 , S8 >
                              >(*this,struct_node);
        }
       break;

       case 8 :
        {
         DDL::GuardFieldTypes<
                               A7,
                               A7,
                               DDL::MapText,
                               DDL::MapPtr< S9 >,
                               DDL::MapPtr< S7 >,
                               DDL::MapRange< DDL::MapPtr< S5 > >
                              >(*this,struct_node);
        }
       break;

       case 9 :
        {
         DDL::GuardFieldTypes<
                               A8,
                               DDL::MapText,
                               DDL::MapRange< S8 >,
                               DDL::MapRange< DDL::MapPtr< S6 > >
                              >(*this,struct_node);
        }
       break;

       case 10 :
        {
         DDL::GuardFieldTypes<
                               A9,
                               DDL::MapText,
                               DDL::MapPtr< S7 >
                              >(*this,struct_node);
        }
       break;

       case 11 :
        {
         DDL::GuardFieldTypes<
                               DDL::MapRange< S10 >,
                               DDL::MapRange< S9 >,
                               DDL::MapRange< DDL::MapPtr< S9 > >,
                               DDL::MapRange< S7 >,
                               DDL::MapRange< S6 >,
                               DDL::MapRange< S5 >,
                               DDL::MapRange< S4 >,
                               DDL::MapRange< S2 >
                              >(*this,struct_node);
        }
       break;

       default: Printf(Exception,"Unknown structure");
      }
   }
 };

template <class T>
struct TypeSet::IsStruct
 {
  static bool Do(const ulen *,ulen) { return false; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S1>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[0]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S2>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[1]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S3>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[2]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S4>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[3]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S5>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[4]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S6>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[5]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S7>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[6]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S8>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[7]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S9>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[8]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S10>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[9]; }
 };

template <>
struct TypeSet::IsStruct<TypeDefCore::S11>
 {
  static bool Do(const ulen *indexes,ulen index) { return index==indexes[10]; }
 };

