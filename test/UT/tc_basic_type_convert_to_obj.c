/*
 * tc_numeric_conver.c
 *
 *  Created on: Aug 26, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */
#include <CUnit/Basic.h>

#include "JsonCObject/jsonCObject.h"
#include "tc_suite_UT.h"

static void tc_numeric_convert(void){
	char i8;
	unsigned char ui8;
	int i32;
	unsigned int ui32;
	short i16;
	unsigned short ui16;
	unsigned long long ui64;
	long long i64;
	char *_i8[]={"-128","127"},
	*_ui8[]={"0","255"},
	*_i16[]={"-32768","32767"},
	*_ui16[]={"0","65535"},
	*_i32[]={"-2147483648","2147483647"},
	*_ui32[]={"0","4294967295"},
	*_ui64[]={"0","18446744073709551615"},
	*_i64[]={"-9223372036854775807","9223372036854775806"};

	int r;
	CU_ASSERT(0==(r=json_cto_object(_i8[0], &(ObjectInfo){.typeInf=&T_INF_INT8,.offset=&i8})));
	if(!r)	CU_ASSERT(i8==-128);
	CU_ASSERT(0==(r=json_cto_object(_i8[1], &(ObjectInfo){.typeInf=&T_INF_INT8,.offset=&i8})));
	if(!r)	CU_ASSERT(i8==127);
	CU_ASSERT(0==(r=json_cto_object(_ui8[0], &(ObjectInfo){.typeInf=&T_INF_UINT8,.offset=&ui8})));
	if(!r)	CU_ASSERT(ui8==0);
	CU_ASSERT(0==(r=json_cto_object(_ui8[1], &(ObjectInfo){.typeInf=&T_INF_UINT8,.offset=&ui8})));
	if(!r)	CU_ASSERT(ui8==255);
	CU_ASSERT(0==(r=json_cto_object(_i16[0], &(ObjectInfo){.typeInf=&T_INF_INT16,.offset=&i16})));
	if(!r)	CU_ASSERT(i16==-32768);
	CU_ASSERT(0==(r=json_cto_object(_i16[1], &(ObjectInfo){.typeInf=&T_INF_INT16,.offset=&i16})));
	if(!r)	CU_ASSERT(i16==32767);
	CU_ASSERT(0==(r=json_cto_object(_ui16[0], &(ObjectInfo){.typeInf=&T_INF_UINT16,.offset=&ui16})));
	if(!r)	CU_ASSERT(ui16==0);
	CU_ASSERT(0==(r=json_cto_object(_ui16[1], &(ObjectInfo){.typeInf=&T_INF_UINT16,.offset=&ui16})));
	if(!r)	CU_ASSERT(ui16==65535);
	CU_ASSERT(0==(r=json_cto_object(_i32[0], &(ObjectInfo){.typeInf=&T_INF_INT32,.offset=&i32})));
	if(!r)	CU_ASSERT(i32==-2147483648);
	CU_ASSERT(0==(r=json_cto_object(_i32[1], &(ObjectInfo){.typeInf=&T_INF_INT32,.offset=&i32})));
	if(!r)	CU_ASSERT(i32==2147483647);
	CU_ASSERT(0==(r=json_cto_object(_ui32[0], &(ObjectInfo){.typeInf=&T_INF_UINT32,.offset=&ui32})));
	if(!r)	CU_ASSERT(ui32==0);
	CU_ASSERT(0==(r=json_cto_object(_ui32[1], &(ObjectInfo){.typeInf=&T_INF_UINT32,.offset=&ui32})));
	if(!r)	CU_ASSERT(ui32==4294967295);
	CU_ASSERT(0==(r=json_cto_object(_ui64[0], &(ObjectInfo){.typeInf=&T_INF_UINT64,.offset=&ui64})));
	if(!r)	CU_ASSERT(ui64==0);
	CU_ASSERT(0==(r=json_cto_object(_ui64[1], &(ObjectInfo){.typeInf=&T_INF_UINT64,.offset=&ui64})));
	if(!r)	CU_ASSERT(ui64==(unsigned long long)(~0UL));
	CU_ASSERT(0==(r=json_cto_object(_i64[0], &(ObjectInfo){.typeInf=&T_INF_INT64,.offset=&i64})));
	if(!r)	CU_ASSERT(i64==(long long)-(~0ULL>>1));
	CU_ASSERT(0==(r=json_cto_object(_i64[1], &(ObjectInfo){.typeInf=&T_INF_INT64,.offset=&i64})));
	if(!r)	CU_ASSERT(i64==(long long)(~0ULL>>1)-1);
}

static void tc_achar_convert(void){
	char cc[]={'"',0,'"','\0'},c;
	int r;
	for(int i=1;i<128;++i){
		cc[1]=i;
		r=json_cto_object(cc, &(ObjectInfo){.typeInf=&T_INF_ACHAR,.offset=&c});
		CU_ASSERT(0==r||(cc[1]=='\\'||cc[1]=='"'||cc[1]=='\r'||cc[1]=='\t'||cc[1]=='\f'||cc[1]=='\n'||cc[1]=='\b'));
		if(!r){	CU_ASSERT(c==i);}
	}
}

static void tc_bool_convert(void){
	char r,f;
	CU_ASSERT(0==(r=json_cto_object("true", &(ObjectInfo){.typeInf=&T_INF_BOOLEAN,.offset=&f})));
	if(!r){	CU_ASSERT(f==1);}
	CU_ASSERT(0==(r=json_cto_object("false", &(ObjectInfo){.typeInf=&T_INF_BOOLEAN,.offset=&f})));
	if(!r){	CU_ASSERT(f==0);}
}

static void tc_string_convert(void){
	char r,
		str[512]={0};
	CU_ASSERT(0==(r=json_cto_object("\"asdfghjklqwertyuiopzxcvbnm123456789!@#$%^&*()_+|[];',./-=QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?\\\\\"", &(ObjectInfo){.typeInf=&T_STRING(512),.offset=&str})));
	if(!r){	CU_ASSERT(strcmp(str,"asdfghjklqwertyuiopzxcvbnm123456789!@#$%^&*()_+|[];',./-=QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?\\\""));}
}

const struct TC_IND ti_basic_type_convert_to_obj={
		.tc_num=4,
		.caseMap={
			{"TC numeric convert",tc_numeric_convert},
			{"TC ASCII char convert",tc_achar_convert},
			{"TC boolean convert",tc_bool_convert},
			{"TC string convert",tc_string_convert}
		}
};
