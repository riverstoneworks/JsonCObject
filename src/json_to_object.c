/*
 * Parse.c
 *
 *  Created on: Jun 27, 2018
 *      Author: Like.Z
 */

#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <uchar.h>
#include <errno.h>

#include "json_type.h"
#include "jsonCObject.h"

static int convert(const regex_t*,char* , ObjectInfo* const );

static const ObjectInfo* const getValueByKey(const char* key,const ObjectInfo* const obj ){
	int i=0,list_size=obj->typeInf->size[1];

	while(list_size>i&&strcmp(key,obj->typeInf->subObjInfo[i].name))++i;

	return i<list_size?obj->typeInf->subObjInfo+i:NULL;
}

static inline int verify(const char *_string, const regex_t *p_re){
	int err_no = p_re?regexec(p_re, _string, 0, NULL, 0):-1;
	return err_no;
}

static inline void freeRegex(size_t num,regex_t * p_re){
	while(num--){
		regfree(p_re+num);
	}
	free(p_re);
}

static inline regex_t * initRegex(const size_t num,const char *pattern[],const int flag[],int *err_no){
	regex_t *p_re = calloc(num, sizeof(regex_t));
	if(!p_re){
		*err_no=-1;
		free(p_re);
		p_re=NULL;
	}else{
		for(int i=0;i<num;i++){
			if((*err_no=regcomp(p_re+i,pattern[i],flag[i]))){
				freeRegex(i,p_re);
				p_re=NULL;
				break;
			}
		}
	}
	return p_re;
}


static int numeric_convert(char* _string, ObjectInfo* const numeric,const regex_t *p_re){
	char * p;
	switch(numeric->typeInf->type){
		case FLOAT: *(float*)(numeric->offset)=strtof(_string,&p); break;
		case DOUBLE: *(double*)(numeric->offset)=strtod(_string,&p); break;
		case LDOUBLE: *(long double*)(numeric->offset)=strtold(_string,&p);break;
		case CHAR: *(char*)(numeric->offset)=strtol(_string,&p,0); break;
		case UCHAR: *(unsigned char*)(numeric->offset)=strtol(_string,&p,0); break;
		case SHORT: *(short*)(numeric->offset)=strtol(_string,&p,0); break;
		case USHORT: *(unsigned short*)(numeric->offset)=strtol(_string,&p,0); break;
		case INT: *(int*)(numeric->offset)=strtol(_string,&p,0); break;
		case UINT: *(unsigned int*)(numeric->offset)=strtol(_string,&p,0); break;
		case LONG: *(long*)(numeric->offset)=strtol(_string,&p,0);  break;
		case ULONG: *(unsigned long*)(numeric->offset)=strtoul(_string,&p,0); break;
		case LLONG: *(long long*)(numeric->offset)=strtoll(_string,&p,0); break;
		case ULLONG: *(unsigned long long*)(numeric->offset)=strtoull(_string,&p,0); break;
		default:
			return -1;
	}

	if(errno)
		return 0;
	else{
		puts(strerror(errno));
		return -1;
	}
}

//string[size,pointer_to_string] the format of _string: "..."
static int string_convert(char* _string, ObjectInfo* const string,const regex_t *p_re){

	int l=strlen(++_string)-1,n=string->typeInf->size[1]-1;

	char * c=(char*)(string->offset), tc,*tp;
	mbstate_t state;
	int i=0,j=0;
	while(i<l&&j<n){
		if(_string[i]=='\\')
			switch (_string[++i]){
				case 'u':
					if(n-j<3){
						i=l;
						continue;
					}
					++i;
					tc=_string[i+4];
					_string[i+4]='\0';
					char16_t c16=strtol(_string+i,&tp,16);
					_string[i+4]=tc;
					size_t rc = c16rtomb(c+j, c16, &state);
					j+=rc;
					i+=3;
					break;
				case 'b':c[j++]='\b'; break;
				case 'f':c[j++]='\f'; break;
				case 'r':c[j++]='\r'; break;
				case 'n':c[j++]='\n'; break;
				case 't':c[j++]='\t'; break;
				case '"':c[j++]='"'; break;
				case '\\':c[j++]='\\'; break;
				default:--i;
			}
		else{
			c[j++]=_string[i];
		}
		++i;
	}

	c[j]='\0';
	return 0;
}

// the format of _string is: true OR false
static int boolean_convert(char* _string, ObjectInfo* const boolean,const regex_t *p_re){

	*(unsigned char*)(boolean->offset)=(_string[0]=='t'?1:0);

	return 0;
}

// the format of _string is: "c"
static int char_convert(char* _string, ObjectInfo* const _char,const regex_t *p_re){

	return strlen(_string)==3?(*(char*)(_char->offset)=_string[1])&&0:-1;

}

// array[array_size, ele_size, poiter_to_array]
static int array_convert(char* _string, ObjectInfo* const array,const regex_t *p_re){
	++_string;
	_string[strlen(_string)-1]='\0';

	ObjectInfo o=*array, so=*(array->typeInf->subObjInfo);
	TypeInf t={
			.name=array->typeInf->name,
			.size={array->typeInf->size[0],array->typeInf->size[1]},
			.subObjInfo=&so,
			.type=array->typeInf->type
	};
	o.typeInf=&t;

	void* base_addr=o.offset ,
			**el_offset=&(so.offset);
	int m_size=1,err_no = 0,
			cap_size=t.size[0],
			ele_size=cap_size/t.size[1];

	regmatch_t match={0};
	char tmp;
	while(!(err_no = regexec(p_re+J_R_GET_ARRAY_ELEMENT, _string, m_size, &match, 0))&&(long)(*el_offset)<cap_size) {
		tmp=_string[match.rm_eo];
		_string[match.rm_eo]='\0';

		if (verify(_string + match.rm_so, p_re + J_R_ARRAY)) {
			(*el_offset)+=(long)base_addr;
			if(-1<convert(p_re,_string+match.rm_so,(ObjectInfo* const)&so)){
				(*el_offset) += ele_size;
			}
			(*el_offset)-=(long)base_addr;
		} else {
			(*el_offset)=NULL+(long)ele_size*convert(p_re,_string+match.rm_so,&o);
		}

		_string[match.rm_eo]=tmp;

		if ('\0' == (*(_string += match.rm_eo))) {
			break;
		}
	}
	_string[strlen(_string)]=']';
	return (long)(*el_offset)/ele_size;
}

static int object_convert(char* _string, ObjectInfo* const object,const regex_t *p_re){
	#define M_SIZE 3
	ObjectInfo *o;
	regmatch_t match[M_SIZE];
	long base_addr=(long)object->offset;
	int err_no = 0;
	char tmp;
	while (!(err_no = regexec(p_re+J_R_GET_KEY_VALUE, _string, M_SIZE, match, 0))) {
		tmp=_string[match[1].rm_eo];
		_string[match[1].rm_eo]='\0';

		o=getValueByKey(_string+match[1].rm_so,object);

		_string[match[1].rm_eo]=tmp;

		if(o){
			ObjectInfo obj=*o;
			obj.offset+=base_addr;				//base + offset == attribute address

			tmp=_string[match[2].rm_eo];
			_string[match[2].rm_eo]='\0';

			convert(p_re,_string+match[2].rm_so,&obj);

			_string[match[2].rm_eo]=tmp;
		}
		if ('\0' == (*(_string += match[0].rm_eo))) {
			break;
		}
	}
	return 0;
	#undef M_SIZE
}

// the memory which pointed by object->offset is only accessed with this pointer
static int ptr_convert(char* _string, ObjectInfo* const ptr,const regex_t *p_re){
	void**p=(void**)ptr->offset;
	if(!p)
		return -1;
	if(!*p){
		*p=malloc(ptr->typeInf->subObjInfo->typeInf->size[0]);
	}
	ObjectInfo o=*ptr->typeInf->subObjInfo;
	o.offset=*p;
	convert(p_re,_string, &o);
	return 0;
}

// the memory which pointed by object->offset is only accessed with this pointer
static inline int null_convert(char* _string, ObjectInfo* const ptr,const regex_t *p_re){
	void**p=(void**)ptr->offset;
	if(*p){
		free(*p);
		*p=NULL;
	}
	return 0;
}
static int (* const conver_by_type[6])(char* _string, ObjectInfo* const data,const regex_t *p_re)={
		numeric_convert,
		string_convert,
		array_convert,
		object_convert,
		boolean_convert,
};

static int convert(const regex_t *p_re, char* _string, ObjectInfo* const objectInfo){

	int t=objectInfo->typeInf->type;

	int i=J_R_NUMBER;
	while(i<=J_R_NULL&&verify(_string,p_re+i)) ++i;

	if(i>J_R_NULL)
		return -1;
	else if(i==J_R_NULL){
		if(t==PTR&&objectInfo->typeInf->subObjInfo->typeInf->type==OBJECT)
			return null_convert(_string,objectInfo,p_re);
		else
			return -1;
	}else if((i==J_R_NUMBER&&t>=CHAR&&t<=ULLONG)
			||(i!=0&&i==t)){
		return conver_by_type[i](_string,objectInfo,p_re);
	}else if(i==J_R_STRING&&t==ACHAR)
		return char_convert(_string,objectInfo,p_re);
	else if(i==J_R_OBJECT&&t==PTR&&objectInfo->typeInf->subObjInfo->typeInf->type==OBJECT)
		return ptr_convert(_string,objectInfo,p_re);
	else
		return -1;

}


static const char regx_numeric[]= "^(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?$";
static const char regx_string[]= "^\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\"$";
static const char regx_boolean[]= "^true|false$";
static const char regx_null[]= "^null$";

static const char regx_array[]=
		"^\\[\\s*"
			"("
				"("
					"(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?"
					"|true|false|null"
					"|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\""
					"|\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						"("
							"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								"("
									"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										"("
											"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\}"
											"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										")*"
									"\\}"
									"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								")*"
							"\\}"
							"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						")*"
					"\\}"
					"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						"("
							"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								"("
									"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										"("
											"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\]"
											"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										")*"
									"\\]"
									"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								")*"
							"\\]"
							"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						")*"
					"\\]"
				")\\s*,\\s*"
			")*("
					"(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?"
					"|true|false|null"
					"|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\""
					"|\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						"("
							"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								"("
									"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										"("
											"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\}"
											"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										")*"
									"\\}"
									"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								")*"
							"\\}"
							"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						")*"
					"\\}"
					"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						"("
							"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								"("
									"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										"("
											"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\]"
											"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
										")*"
									"\\]"
									"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								")*"
							"\\]"
							"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						")*"
					"\\]"
				")"
		"\\s*\\]$";

static const char regx_object[]=
		"^\\{\\s*"
			"("
				"\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])+\""
				"\\s*:\\s*("
						"(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?"
						"|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\""
						"|true|false|null"
						"|\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							"("
								"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									"("
										"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											"("
												"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\}"
												"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											")*"
										"\\}"
										"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									")*"
								"\\}"
								"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							")*"
						"\\}"
						"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							"("
								"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									"("
										"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											"("
												"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\]"
												"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											")*"
										"\\]"
										"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									")*"
								"\\]"
								"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							")*"
						"\\]"
					")\\s*,\\s*"
				")*(\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])+\")\\s*:"
				"\\s*("
						"(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?"
						"|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\""
						"|true|false|null"
						"|\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							"("
								"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									"("
										"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											"("
												"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\}"
												"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											")*"
										"\\}"
										"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									")*"
								"\\}"
								"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							")*"
						"\\}"
						"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							"("
								"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									"("
										"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											"("
												"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\]"
												"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
											")*"
										"\\]"
										"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									")*"
								"\\]"
								"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							")*"
						"\\]"
					")"
		"\\s*\\}$";

static const char regx_grep_array_el[]=
		"("
			"(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?"
			"|true|false|null"
			"|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\""
			"|\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
				"("
					"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						"("
							"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								"("
									"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\}"
									"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								")*"
							"\\}"
							"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						")*"
					"\\}"
					"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
				")*"
			"\\}"
			"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
				"("
					"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						"("
							"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								"("
									"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\]"
									"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
								")*"
							"\\]"
							"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
						")*"
					"\\]"
					"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
				")*"
			"\\]"
		")";

static const char regx_grep_object_attr[]=
		"\"([^\"]+)\""
		"\\s*:\\s*("
				"(\\-)?([1-9][0-9]*|0)(\\.[0-9]+)?((e|E)(\\-|\\+)?[0-9]+)?"
				"|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\""
				"|true|false|null"
				"|\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
					"("
						"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							"("
								"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									"("
										"\\{([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\}"
										"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									")*"
								"\\}"
								"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							")*"
						"\\}"
						"([^\"\\{\\}]|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
					")*"
				"\\}"
				"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
					"("
						"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							"("
								"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									"("
										"\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*\\]"
										"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
									")*"
								"\\]"
								"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
							")*"
						"\\]"
						"([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\")*"
					")*"
				"\\]"
			")";

static const char regx_test[]="^\"([^\"\\\\\f\n\r\b\t]|\\\\u[0-9A-Fa-f]{4}|\\\\[\\\\\"/bnrtf])*\"$";

static const char *pattern[]= {
		regx_numeric,
		regx_string,
		regx_array,
		regx_object,
		regx_boolean,
		regx_null,
		regx_grep_array_el,
		regx_grep_object_attr,
		regx_test
};
static const int flag[]={
		REG_EXTENDED|REG_NOSUB,
		REG_EXTENDED|REG_NOSUB,
		REG_EXTENDED|REG_NOSUB,
		REG_EXTENDED|REG_NOSUB,
		REG_EXTENDED|REG_NOSUB,
		REG_EXTENDED|REG_NOSUB,
		REG_EXTENDED,
		REG_EXTENDED,
		REG_EXTENDED|REG_NOSUB
};
static regex_t* reg_p=NULL;

int json_cto_object_init(){
	if(reg_p!=NULL)
		return -1;

	int erro_no;
	reg_p=initRegex(9,pattern,flag,&erro_no);

	return erro_no;
}

int json_cto_object_destroy(){
	if(reg_p!=NULL){
		freeRegex(9,reg_p);
		reg_p=NULL;
		return 0;
	}else
		return -1;
}

int json_cto_object(char* string, ObjectInfo* objectInfo){
	return convert(reg_p,string,objectInfo)>-1?0:-1;
}

int verify_test(int i,char* s){
	return verify(s,reg_p+i);
}
