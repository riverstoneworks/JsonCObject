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

struct TYPE_INF{
	const char* name;
	const enum {
		STRING = 1,
		ARRAY,
		OBJECT,
		BOOLEAN,
		INT = 11,
		LONG,
		LLONG,
		FLOAT,
		DOUBLE,
		LDOUBLE,
		UINT = 21,
		ULONG,
		ULLONG
	}type;
	//size[0] is size of base type (int,float etc.) or length of string or element size in array;  size[1] is element number in array
	const size_t size[2];
	//the *object int objectinfo array is the offset for attributes of STRUCT
	const ObjectInfo* const subObjInfo;
};

struct OBJECT_INF{
	const char* const name;
	const struct TYPE_INF* typeInf;
	//base address for STRUCT type
	void * obj_addr; // when it is used as offset address , the type is int.
};

extern const struct TYPE_INF T_INF_INT;
extern const struct TYPE_INF T_INF_UINT;
extern const struct TYPE_INF T_INF_ULONG;
extern const struct TYPE_INF T_INF_ULLONG;
extern const struct TYPE_INF T_INF_LONG;
extern const struct TYPE_INF T_INF_LLONG;
extern const struct TYPE_INF T_INF_FLOAT;
extern const struct TYPE_INF T_INF_DOUBLE;;
extern const struct TYPE_INF T_INF_LDOUBLE;
extern const struct TYPE_INF T_INF_BOOLEAN;

#endif /* TYPE_INFO_H_ */
