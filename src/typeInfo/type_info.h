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

// init ObjectInfo
#define O(na,ty,offs){\
			.name=#na,\
			.typeInf=&ty,\
			.offset=(void*)(offs)\
		}

//init TYPE_INF
#define TG(na,ty,siz_ty,sub_el_num,...) (TypeInf){\
		.name=#na,\
		.type=ty,\
		.size={siz_ty,sub_el_num},\
		.subObjInfo=(const ObjectInfo*)&(ObjectInfo[]){__VA_ARGS__}\
}

//for Struct
#define	T(na,attr_num,...) \
		TG(na,OBJECT,sizeof(na),attr_num,__VA_ARGS__)

//for Array
#define T_ARRAY(el_type_name,el_num,el_type) \
		TG(el_type_name##el_num,ARRAY,sizeof(el_type_name)*el_num,el_num,\
				O(el_type_name,el_type,0))\
//for String
#define T_STRING(length)\
		TG(char##length,STRING,sizeof(char)*length,length)

#define	T_PTR(na,...) \
		TG(na,PTR,sizeof(char*),1,\
				__VA_ARGS__)


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

#endif /* TYPE_INFO_H_ */
