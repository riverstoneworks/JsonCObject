/*
 * Parse_to_json.c
 *
 *  Created on: Aug 2, 2018
 *      Author: Like.Z
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Parse.h"

enum{
	J_R_NUMBER=0,
	J_R_STRING,
	J_R_ARRAY,
	J_R_OBJECT,
	J_R_BOOLEAN,
	J_R_GET_ARRAY_ELEMENT,
	J_R_GET_KEY_VALUE,
};

static int convert(char* ,size_t *, ObjectInfo* const );

static int numeric_conver(char* _string,size_t * str_len, ObjectInfo* const numeric){
	char s[24];
	int n;
	switch(numeric->typeInf->type){
		case FLOAT:
			sprintf(s,"%G%n",*(float*)(numeric->offset),&n); break;
		case DOUBLE:
			sprintf(s,"%lG%n",*(double*)(numeric->offset),&n); break;
		case LDOUBLE:
			sprintf(s,"%.16LG%n",*(long double*)(numeric->offset),&n);break;
		case INT:
			sprintf(s,"%d%n",*(int*)(numeric->offset),&n); break;
		case UINT:
			sprintf(s,"%u%n",*(unsigned int*)(numeric->offset),&n); break;
		case LONG:
			sprintf(s,"%ld%n",*(long*)(numeric->offset),&n);  break;
		case ULONG:
			sprintf(s,"%ld%n",*(unsigned long*)(numeric->offset),&n); break;
		case LLONG:
			sprintf(s,"%lld%n",*(long long*)(numeric->offset),&n); break;
		case ULLONG:
			sprintf(s,"%lld%n",*(unsigned long long*)(numeric->offset),&n); break;
		default:
			return -1;
	}
	if(n>*str_len)
		return -1;
	strncpy(_string,s,n);
	_string+=n;
	*str_len-=n;
	return 0;
}

//string[size,pointer_to_string] the format of _string: "..."
static int string_conver(char* _string, size_t* str_len, ObjectInfo* const string){
	if(!string->offset){
		if(*str_len>3)
			strncpy(_string,"null",4);
		else
			return -1;
	}

	size_t l=*str_len-2,n=strlen((const char*)(string->offset));

	if(l<n)
		return -1;
	else if(n>0)
		strncpy(_string+1,(char*)(string->offset),n);


	_string[0]=_string[n+1]='"';
	_string+=(n+2);
	*str_len-=(n+2);
	return 0;
}

// the format of _string is: true OR false
static int boolean_conver(char* _string,size_t* str_len, ObjectInfo* const boolean){

	int i=0;
	char* s=*(char*)(boolean->offset)?*str_len>3&&(i=3)?"true":NULL:*str_len>4&&(i=4)?"false":NULL;

	if(!s)
		return -1;

	strncpy(_string,s,i);
	*str_len-=i;
	_string+=i;
	return 0;
}

// array[array_size, ele_size, poiter_to_array]
static int array_conver(char* _string,size_t* str_len, ObjectInfo* const array){
	if(*str_len<2||!array->offset)
		return -1;
	else{
		*_string++='[';
		*str_len-=2;
	}

	size_t n=array->typeInf->size[1];
	if(n<1)
		return 0;
	else{
		size_t el_size=array->typeInf->size[0]/n;
		ObjectInfo ot=*array->typeInf->subObjInfo;
		ot.offset=array->offset;

		int i=0;
		while(i<n&&!(ot.offset+=i*el_size)) ++i;
		if(i<n){
			do{
				if(0>convert(_string,str_len,&ot))
							return -1;
				if(n-->0){
					if(*str_len<2)
						return -1;
					ot.offset+=el_size;
					*_string++=',';
					--*str_len;
				}else
					break;
			}while(1);
		}
	}
	*_string++=']';
	return 0;
}

static int object_conver(char* _string,size_t * str_len, ObjectInfo* const object){
	if(*str_len<2)
		return -1;
	else{
		*_string++='{';
		*str_len-=2;
	}

	const ObjectInfo *o = object->typeInf->subObjInfo;
	size_t n=object->typeInf->size[1];
	if(n>0&&o){

		int t;
		long base_addr = (long) object->offset;

		do{
			ObjectInfo obj = *o;
			obj.offset += base_addr;		//base + offset == attribute address
			t=strlen(obj.name)+3;
			if (*str_len < t + 1)
				return -1;

			sprintf(_string, "\"%s\":", obj.name);
			*str_len-=t;
			if(0>convert(_string, str_len, &obj))
				return -1;
			else if(--n<1)
				break;
			else{
				if(*str_len<6){
					return -1;
				}else{
					++o;
					*_string++ = ',';
					--*str_len;
				}
			}
		}while(1);
	}

	*_string++='}';
	return 0;
}

static int (* const conver_by_type[5])(char* _string,size_t* str_len, ObjectInfo* const data)={
		numeric_conver,
		string_conver,
		array_conver,
		object_conver,
		boolean_conver,
};

static int convert(char* _string,size_t * str_len, ObjectInfo* const objectInfo){

	int i=J_R_STRING;
	if(objectInfo->typeInf->type>=INT)
		i=J_R_NUMBER;
	else
		while(i<=J_R_BOOLEAN&&objectInfo->typeInf->type==i) ++i;

	if(i>J_R_BOOLEAN)
		return -1;
	else
		return conver_by_type[i](_string,str_len,objectInfo);
}


int struct_cto_json(char* string, ObjectInfo* objectInfo){
	size_t str_len=strlen(string);
	return convert(string,&str_len,objectInfo);
}
