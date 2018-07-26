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
	p_re=NULL;
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
				free(p_re);
				break;
			}
		}
	}
	return p_re;
}


static int numeric_conver(char* _string, ObjectInfo* const numeric,const regex_t *p_re){
	int c=1,i=0;
	long long lh=0;
	long double ld=(long double)0.0;

	if(*_string=='-'){
		if(numeric->typeInf->type>=UINT)	return -1;
		++_string;
		c=-1;
	}else if(*_string=='+'){
		++_string;
	}

	while((*_string)!='\0'&&(*_string)!='.')	lh=(*_string++)-48+(lh*10);

	if (*_string == '.'&&(i=strlen(_string))>1) {
		while((--i)>0)
			ld=(_string[i]-48+ld)/10;
	}
	ld = (lh*=c) + ld * c;

	switch(numeric->typeInf->type){
		case FLOAT: *(float*)(numeric->offset)=ld; break;
		case DOUBLE: *(double*)(numeric->offset)=ld; break;
		case LDOUBLE: *(long double*)(numeric->offset)=ld;break;
		case INT: *(int*)(numeric->offset)=lh; break;
		case UINT: *(unsigned int*)(numeric->offset)=lh; break;
		case LONG: *(long*)(numeric->offset)=lh;  break;
		case ULONG: *(unsigned long*)(numeric->offset)=lh; break;
		case LLONG: *(long long*)(numeric->offset)=lh; break;
		case ULLONG: *(unsigned long long*)(numeric->offset)=lh; break;
		default:
			return -1;
	}

	return 0;
}

//string[size,pointer_to_string] the format of _string: "..."
static int string_conver(char* _string, ObjectInfo* const string,const regex_t *p_re){

	int l=strlen(_string)-2,n=string->typeInf->size[0]-1;

	if((l=l>n?n:l)<0)
		return -1;

	strncpy((char*)(string->offset),_string+1,l)[l]='\0';

	return 0;
}

// the format of _string is: true OR false
static int boolean_conver(char* _string, ObjectInfo* const boolean,const regex_t *p_re){

	*(char*)(boolean->offset)=_string[0]=='t'?1:0;

	return 0;
}

// array[array_size, ele_size, poiter_to_array]
static int array_conver(char* _string, ObjectInfo* const array,const regex_t *p_re){
	++_string;
	_string[strlen(_string)-1]='\0';

	void* base_addr=array->offset;
	struct TYPE_INF it=*(array->typeInf->subObjInfo->typeInf);
	ObjectInfo sot={
			.typeInf=&it,
			.offset=base_addr
	};
	ObjectInfo ot={
				.typeInf=&(struct TYPE_INF){
					.type=array->typeInf->type,
					.size={array->typeInf->size[0],array->typeInf->size[1]},
					.subObjInfo=&sot
				},
				.offset=base_addr
	};

	int m_size=1,err_no = 0,ii,i=0,ele_size=sot.typeInf->size[0];
	size_t *cap_left=(size_t*)ot.typeInf->size+1;

	regmatch_t match={0};
	char tmp;
	while(!(err_no = regexec(p_re+J_R_GET_ARRAY_ELEMENT, _string, m_size, &match, 0))&&*cap_left>0) {
		tmp=_string[match.rm_eo];
		_string[match.rm_eo]='\0';

		if (verify(_string + match.rm_so, p_re + J_R_ARRAY)) {
			ii=convert(p_re,_string+match.rm_so,&sot);
			if(ii>-1){
				i++;
				sot.offset = base_addr+i*ele_size;
				(*cap_left)--;
			}
		} else {
			ii=convert(p_re,_string+match.rm_so,&ot);
			if(ii>-1){
				i+=ii;
				sot.offset = base_addr+ ele_size * i;
				(*cap_left)-=ii;
			}
		}

		_string[match.rm_eo]=tmp;

		if ('\0' == (*(_string += match.rm_eo))) {
			break;
		}
	}
//	array->offset-=ele_size*i;
//	((enum E*)(array->typeInf->size))[1]+=i;

	_string[strlen(_string)]=']';
	return i;
}

static int object_conver(char* _string, ObjectInfo* const object,const regex_t *p_re){
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

static int (* const conver_by_type[5])(char* _string, ObjectInfo* const data,const regex_t *p_re)={
		numeric_conver,
		string_conver,
		array_conver,
		object_conver,
		boolean_conver,
};

static int convert(const regex_t *p_re, char* _string, ObjectInfo* const objectInfo){

	int i=J_R_NUMBER;
	while(i<=J_R_BOOLEAN&&verify(_string,p_re+i)) ++i;
	if(i>J_R_BOOLEAN)
		return -1;

	if((i==J_R_NUMBER&&objectInfo->typeInf->type>=INT)
			||(i!=0&&i==objectInfo->typeInf->type)){
		return conver_by_type[i](_string,objectInfo,p_re);
	}else
		return -1;

}


static const char regx_numeric[]= "^(\\-|\\+)?[0-9]+(\\.[0-9]+)?$";
static const char regx_string[]= "^\"[^\"\\]*\"$";
static const char regx_boolean[]= "^true|false$";

static const char regx_array[]=
		"^\\[\\s*"
			"("
				"("
					"(\\-|\\+)?[0-9]+(\\.[0-9]+)?"
					"|true|false"
					"|\"[^\\\\\"]*\""
					"|\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*\\}"
					"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}"
					"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}"
					"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*\\]"
					"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}"
					"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}"
				")\\s*,\\s*"
			")*("
					"(\\-|\\+)?[0-9]+(\\.[0-9]+)?"
					"|true|false"
					"|\"[^\\\\\"]*\""
					"|\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*\\}"
					"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}"
					"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}"
					"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*\\]"
					"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}"
					"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}"
				")"
		"\\s*\\]$";

static const char regx_object[]=
		"^\\{\\s*"
			"("
				"(\"[^\"\\\\]+\")\\s*:"
				"\\s*("
						"(\\-|\\+)?[0-9]+(\\.[0-9]+)?"
						"|\"[^\"\\\\]*\""
						"|true|false"
						"|\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*\\}"
						"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}"
						"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}"
						"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*\\]"
						"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}"
						"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}"
					")\\s*,\\s*"
				")*(\"[^\"\\\\]+\")\\s*:"
				"\\s*("
						"(\\-|\\+)?[0-9]+(\\.[0-9]+)?"
						"|\"[^\"\\\\]*\""
						"|true|false"
						"|\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*\\}"
						"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){2}"
						"|(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}"
						"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*\\]"
						"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}"
						"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}"
			")"
		"\\s*\\}$";

static const char regx_grep_array_el[]=
		"("
			"(\\-|\\+)?[0-9]+(\\.[0-9]+)?"
			"|true|false"
			"|\"[^\"\\\\]*\""
			"|\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*\\}"
			"|(\\{([^\"\\{\\}]|\"[^\"\\\\]*\")*){2}(([^\"\\{\\}]|\"[^\"\\\\]*\")*\\}){2}"
			"|(\\{([^\"\\{\\}]|\"[^\"\\\\]*\")*){3}(([^\"\\{\\}]|\"[^\"\\\\]*\")*\\}){3}"
			"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*\\]"
			"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}"
			"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}"
		")";

static const char regx_grep_object_attr[]=
		"\"([^\"\\\\]+)\"\\s*"
		":\\s*("
				"(\\-|\\+)?[0-9]+(\\.[0-9]+)?"
				"|\"[^\"\\\\]*\""
				"|true|false"
				"|\\{([^\"\\{\\}]|\"[^\"\\\\]*\")*\\}"
				"|(\\{([^\"\\{\\}]|\"[^\"\\\\]*\")*){2}(([^\"\\{\\}]|\"[^\"\\\\]*\")*\\}){2}"
				"|(\\{([^\"\\{\\}]|\"[^\"\\\\]*\")*){3}(([^\"\\{\\}]|\"[^\"\\\\]*\")*\\}){3}"
				"|\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*\\]"
				"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){2}"
				"|(\\[([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}(\\]([A-Za-z0-9,:\\.\\+\\{\\}]|\\s|\\-|\"[^\\\\\"]*\")*){3}"
			")";

static const char regx_test[]="^(\\{([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}(\\}([^\\{\\}\"]|\"[^\\\\\"]*\")*){3}$";
static const char *pattern[]= {
		regx_numeric,
		regx_string,
		regx_array,
		regx_object,
		regx_boolean,
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
		REG_EXTENDED,
		REG_EXTENDED,
		REG_EXTENDED|REG_NOSUB
};
static regex_t* reg_p=NULL;

int json_convert_init(){
	if(reg_p!=NULL)
		return -1;

	int erro_no;
	reg_p=initRegex(8,pattern,flag,&erro_no);

	return erro_no;
}

int json_convert_destroy(){
	if(reg_p!=NULL){
		freeRegex(1,reg_p);
		return 0;
	}else
		return -1;
}

int json_convert_to_obj(char* string, ObjectInfo* objectInfo){
	return convert(reg_p,string,objectInfo);
}

int verify_test(int i,char* s){
	return verify(s,reg_p+i);
}

//validate array [...]
// ^\[\s*(((\-|\+)?[0-9]+(\.[0-9]+)?|true|false|"[^\\"]*"|\{([^\{\}"]|"[^\\"]*")*\}|(\{([^\{\}"]|"[^\\"]*")*){2}.*(\}([^\{\}"]|"[^\\"]*")*){2}|(\{([^\{\}"]|"[^\\"]*")*){3}.*(\}([^\{\}"]|"[^\\"]*")*){3}|\[([^\[\]"]|"[^\\"]*")*\]|(\[([^\[\]"]|"[^\\"]*")*){2}.*(\]([^\[\]"]|"[^\\"]*")*){2}|(\[([^\[\]"]|"[^\\"]*")*){3}.*(\]([^\[\]"]|"[^\\"]*")*){3})\s*,\s*)*((\-|\+)?[0-9]+(\.[0-9]+)?|true|false|"[^\\"]*"|\{([^\{\}"]|"[^\\"]*")*\}|(\{([^\{\}"]|"[^\\"]*")*){2}.*(\}([^\{\}"]|"[^\\"]*")*){2}|(\{([^\{\}"]|"[^\\"]*")*){3}.*(\}([^\{\}"]|"[^\\"]*")*){3}|\[([^\[\]"]|"[^\\"]*")*\]|(\[([^\[\]"]|"[^\\"]*")*){2}.*(\]([^\[\]"]|"[^\\"]*")*){2}|(\[([^\[\]"]|"[^\\"]*")*){3}.*(\]([^\[\]"]|"[^\\"]*")*){3})\s*\]$

//validate object {...}
//^\\{\\s*((\"[^\"\\]+\")\\s*:\\s*((\\-|\\+)?[0-9]+(\\.[0-9]+)?|\"[^\"\\]+\"|\\{(([^\"\\{\\}]|\"[^\"\\]+\")*\\{){1}([^\"\\{\\}]|\"[^\"\\]+\")*(\\}([^\"\\{\\}]|\"[^\"\\]+\")*){1}\\}|\\{(([^\"\\{\\}]|\"[^\"\\]+\")*\\{){2}([^\"\\{\\}]|\"[^\"\\]+\")*(\\}([^\"\\{\\}]|\"[^\"\\]+\")*){2}\\})\\s*,\\s*)*((\"[^\"\\]+\")\\s*:\\s*((\\-|\\+)?[0-9]+(\\.[0-9]+)?|\"[^\"\\]+\"|\\{(([^\"\\{\\}]|\"[^\"\\]+\")*\\{){1}([^\"\\{\\}]|\"[^\"\\]+\")*(\\}([^\"\\{\\}]|\"[^\"\\]+\")*){1}\\}|\\{(([^\"\\{\\}]|\"[^\"\\]+\")*\\{){2}([^\"\\{\\}]|\"[^\"\\]+\")*(\\}([^\"\\{\\}]|\"[^\"\\]+\")*){2}\\}))\\s*\\}$




//{...} object element
// "(\"[^\"\\]+\")\\s*:\\s*((\\-|\\+)?[0-9]+(\\.[0-9]+)?|\"[^\"\\]+\"|\\{(([^\"\\{\\}]|\"[^\"\\]+\")*\\{){1}([^\"\\{\\}]|\"[^\"\\]+\")*(\\}([^\"\\{\\}]|\"[^\"\\]+\")*){1}\\}|\\{(([^\"\\{\\}]|\"[^\"\\]+\")*\\{){2}([^\"\\{\\}]|\"[^\"\\]+\")*(\\}([^\"\\{\\}]|\"[^\"\\]+\")*){2}\\})");

//[...] array element
// ((\-|\+)?[0-9]+(\.[0-9]+)?|true|false|"[^\\"]*"|\{([^\{\}"]|"[^\\"]*")*\}|(\{([^\{\}"]|"[^\\"]*")*){2}.*(\}([^\{\}"]|"[^\\"]*")*){2}|(\{([^\{\}"]|"[^\\"]*")*){3}.*(\}([^\{\}"]|"[^\\"]*")*){3}|\[([^\[\]"]|"[^\\"]*")*\]|(\[([^\[\]"]|"[^\\"]*")*){2}.*(\]([^\[\]"]|"[^\\"]*")*){2}|(\[([^\[\]"]|"[^\\"]*")*){3}.*(\]([^\[\]"]|"[^\\"]*")*){3})
