/*
 * Parse.h
 *
 *  Created on: Jun 27, 2018
 *      Author: xpc
 */

#ifndef PARSE_H_
#define PARSE_H_

//typedef struct OBJECT_INFO ObjectInfo;
struct OBJECT_INFO{
	const char* name;
	enum E{
		STRING=1,
		ARRAY,
		OBJECT,
		BOOLEAN,
		INT=11,
		LONG,
		LLONG,
		FLOAT,
		DOUBLE,
		LDOUBLE,
		UINT=21,
		ULONG,
		ULLONG
	}const type[2];
	//size[0] is size of base type (int,float etc.) or length of string or element size in array;  size[1] is element number in array
	size_t size[2];
	//the *object int objectinfo array is the offset for attributes of STRUCT
	ObjectInfo* const subObjInfo;
	//base address for STRUCT type
	void* obj_addr; // when it is used as offset address , the type is int.
};

//Improve
typedef struct OBJECT_INF ObjectInfo;
struct TYPE_INF{
	const char* name;
	enum E const  type[2];
	//size[0] is size of base type (int,float etc.) or length of string or element size in array;  size[1] is element number in array
	const size_t size[2];
	//the *object int objectinfo array is the offset for attributes of STRUCT
	const struct ObjectInfo* const subObjInfo;
};

struct OBJECT_INF{
	const char* name;
	const struct TYPE_INF* type_inf;
	//base address for STRUCT type
	void* obj_addr; // when it is used as offset address , the type is int.
};


int json_convert_init();
int json_convert_to_obj(char* , ObjectInfo*);
int json_convert_from_obj(char* , ObjectInfo*);
int json_convert_destroy();

int verify_test(int i,char* s);
#endif /* PARSE_H_ */
