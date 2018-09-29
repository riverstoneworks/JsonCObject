/*
 * typeInfo.h
 *
 *  Created on: Jul 23, 2018
 *      Author: Like.Z
 */

#ifndef TYPE_INFO_H_
#define TYPE_INFO_H_
#include <stddef.h>
typedef struct OBJECT_INF ObjectInfo;

typedef struct TYPE_INF{
	const char* name;
	const enum {
		STRING = 1,
		ARRAY,
		OBJECT,
		BOOLEAN,
		ACHAR,
		INT8=11,
		INT16,
		INT32,
		INT64,
		FLOAT,
		DOUBLE,
		LDOUBLE,
		UINT8= 21,
		UINT16,
		UINT32,
		UINT64,
		PTR
	}type;
	//size[0] is size of base type (int,float etc.) or length of string or element size in array;  size[1] is element number in array
	const size_t size[2];
	//the *object int objectinfo array is the offset for attributes of STRUCT
	const ObjectInfo* const subObjInfo;
}TypeInf;


struct OBJECT_INF{
	const char* const name;
	const TypeInf* typeInf;
	//base address for STRUCT type
	void * offset; // when it is used as offset address , the type is int.
};


//base type info
extern const TypeInf T_INF_INT8;
extern const TypeInf T_INF_UINT8;
extern const TypeInf T_INF_INT16;
extern const TypeInf T_INF_UINT16;
extern const TypeInf T_INF_INT32;
extern const TypeInf T_INF_UINT32;
extern const TypeInf T_INF_INT64;
extern const TypeInf T_INF_UINT64;
extern const TypeInf T_INF_FLOAT;
extern const TypeInf T_INF_DOUBLE;
extern const TypeInf T_INF_LDOUBLE;
extern const TypeInf T_INF_BOOLEAN;
extern const TypeInf T_INF_ACHAR;


// init ObjectInfo
#define O(na,ty,offs){\
			.name=#na,\
			.typeInf=&ty,\
			.offset=(void*)(offs)\
		}

//init TYPE_INF
#define TG(na,ty,siz,sub_el_num,...) (TypeInf){\
		.name=#na,\
		.type=ty,\
		.size={siz,sub_el_num},\
		.subObjInfo=(const ObjectInfo*)&(ObjectInfo[]){__VA_ARGS__}\
}

//for Struct
#define	T(na,attr_num,...) \
		TG(na,OBJECT,sizeof(na),attr_num,__VA_ARGS__)

//for Array
#define T_ARRAY(el_ty_na,el_num,el_type) \
		TG(el_ty_na##el_num,ARRAY,sizeof(el_ty_na)*el_num,el_num,\
				O(,(el_type),0))

//for String
#define T_STRING(length) \
		TG(char##length,STRING,sizeof(char)*length,length)

#define O_STR(na,len,offs) \
		O(na,T_STRING(len),offs)

#define O_ARRAY(na,el_ty_na,el_num,el_type,offs) \
		O(na,T_ARRAY(el_ty_na,el_num,(el_type)),offs)

//for Pointer
#define	T_PTR_OBJ(na,ty) \
		TG(na,PTR,sizeof(char*),1,\
				O(,(ty),0))

#define	T_PTR_STR(na,len) \
		TG(na,PTR,sizeof(char*),1,\
				O_STR(,len,0))

#define	T_PTR_ARRAY(na,el_ty_na,el_num,el_type) \
		TG(na,PTR,sizeof(char*),1,\
				O_ARRAY(,el_ty_na,el_num,(el_type),0))

#define O_PTR_OBJ(na,ty,offs) \
		O(na,T_PTR_OBJ(,(ty)),offs)

#define O_PTR_STR(na,len,offs) \
		O(na,T_PTR_STR(,len),offs)

#define O_PTR_ARRAY(na,el_ty_na,el_num,el_type,offs) \
		O(na,T_PTR_ARRAY(,el_ty_na,el_num,(el_type)),offs)



#define O_INT8(na,offs) \
	O(na,T_INF_INT8,offs)

#define O_ARRAY_INT8(na,el_num,offs) \
		O_ARRAY(na,char,el_num,T_INF_INT8,offs)

#define O_UINT8(na,offs) \
	O(na,T_INF_UINT8,offs)

#define O_ARRAY_UINT8(na,el_num,offs) \
		O_ARRAY(na,char,el_num,T_INF_UINT8,offs)

#define O_INT16(na,offs) \
	O(na,T_INF_INT16,offs)

#define O_ARRAY_INT16(na,el_num,offs) \
		O_ARRAY(na,short,el_num,T_INF_INT16,offs)

#define O_UINT16(na,offs) \
	O(na,T_INF_UINT16,offs)

#define O_ARRAY_UINT16(na,el_num,offs) \
		O_ARRAY(na,short,el_num,T_INF_UINT16,offs)

#define O_INT32(na,offs) \
	O(na,T_INF_INT32,offs)

#define O_ARRAY_INT32(na,el_num,offs) \
		O_ARRAY(na,int,el_num,T_INF_INT32,offs)

#define O_UINT32(na,offs) \
	O(na,T_INF_UINT32,offs)

#define O_ARRAY_UINT32(na,el_num,offs) \
		O_ARRAY(na,int,el_num,T_INF_UINT32,offs)

#define O_INT64(na,offs) \
	O(na,T_INF_INT64,offs)

#define O_ARRAY_INT64(na,el_num,offs) \
		O_ARRAY(na,long long,el_num,T_INF_INT64,offs)

#define O_UINT64(na,offs) \
	O(na,T_INF_UINT64,offs)

#define O_ARRAY_UINT64(na,el_num,offs) \
		O_ARRAY(na,long long,el_num,T_INF_UINT64,offs)

#define O_FLOAT(na,offs) \
	O(na,T_INF_FLOAT,offs)

#define O_ARRAY_FLOAT(na,el_num,offs) \
		O_ARRAY(na,float,el_num,T_INF_FLOAT,offs)

#define O_DOUBLE(na,offs) \
	O(na,T_INF_DOUBLE,offs)

#define O_ARRAY_DOUBLE(na,el_num,offs) \
		O_ARRAY(na,double,el_num,T_INF_DOUBLE,offs)

#define O_LDOUBLE(na,offs) \
	O(na,T_INF_LDOUBLE,offs)

#define O_ARRAY_LDOUBLE(na,el_num,offs) \
		O_ARRAY(na,long double,el_num,T_INF_LDOUBLE,offs)

#define O_BOOLEAN(na,offs) \
	O(na,T_INF_BOOLEAN,offs)

#define O_ARRAY_BOOLEAN(na,el_num,offs) \
		O_ARRAY(na,char,el_num,T_INF_BOOLEAN,offs)

#define O_ACHAR(na,offs) \
	O(na,T_INF_ACHAR,offs)

#define O_ARRAY_ACHAR(na,el_num,offs) \
		O_ARRAY(na,char,el_num,T_INF_ACHAR,offs)

#endif /* TYPE_INFO_H_ */
