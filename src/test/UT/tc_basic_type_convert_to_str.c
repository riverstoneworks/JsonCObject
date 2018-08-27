/*
 * tc_numeric_conver.c
 *
 *  Created on: Aug 26, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */
#include <CUnit/Basic.h>
#include "../../jsonCObject.h"
#include "tc_suite_UT.h"

static void tc_numeric_convert(void){
	char i8=-128;
	unsigned char ui8;
	int i32;
	unsigned int ui32;
	short i16;
	unsigned short ui16;
	unsigned long long ui64;
	long long i64;
	char _i8[30]={0},
	_ui8[30]={0},
	_i16[30]={0},
	_ui16[30]={0},
	_i32[30]={0},
	_ui32[30]={0},
	_ui64[30]={0},
	_i64[30]={0};

	int r;
	CU_ASSERT(0==(r=json_cf_object(_i8,30, &(ObjectInfo){.typeInf=&T_INF_INT8,.offset=&i8})));
	if(!r)	CU_ASSERT(strcmp(_i8,"-128")==0);
	i8=127;
	CU_ASSERT(0==(r=json_cf_object(_i8,30, &(ObjectInfo){.typeInf=&T_INF_INT8,.offset=&i8})));
	if(!r)	CU_ASSERT(strcmp(_i8,"127")==0);
	(ui8=0);
	CU_ASSERT(0==(r=json_cf_object(_ui8,30, &(ObjectInfo){.typeInf=&T_INF_UINT8,.offset=&ui8})));
	if(!r)	CU_ASSERT(strcmp(_ui8,"0")==0);
	(ui8=255);
	CU_ASSERT(0==(r=json_cf_object(_ui8,30, &(ObjectInfo){.typeInf=&T_INF_UINT8,.offset=&ui8})));
	if(!r)	CU_ASSERT(strcmp(_ui8,"255")==0);
	(i16=-32768);
	CU_ASSERT(0==(r=json_cf_object(_i16,30, &(ObjectInfo){.typeInf=&T_INF_INT16,.offset=&(i16)})));
	if(!r)	CU_ASSERT(strcmp(_ui8,"-32768")==0);
	i16=32767;
	CU_ASSERT(0==(r=json_cf_object(_i16,30, &(ObjectInfo){.typeInf=&T_INF_INT16,.offset=&(i16)})));
	if(!r)	CU_ASSERT(strcmp(_ui8,"32767")==0);
	ui16=0;
	CU_ASSERT(0==(r=json_cf_object(_ui16,30, &(ObjectInfo){.typeInf=&T_INF_UINT16,.offset=&(ui16)})));
	if(!r)	CU_ASSERT(strcmp(_i16,"0")==0);
	ui16=65535;
	CU_ASSERT(0==(r=json_cf_object(_ui16,30, &(ObjectInfo){.typeInf=&T_INF_UINT16,.offset=&(ui16)})));
	if(!r)	CU_ASSERT(strcmp(_i16,"65535")==0);
	i32=-2147483648;
	CU_ASSERT(0==(r=json_cf_object(_i32,30, &(ObjectInfo){.typeInf=&T_INF_INT32,.offset=&(i32)})));
	if(!r)	CU_ASSERT(strcmp(_i32,"-2147483648")==0);
	i32=2147483647;
	CU_ASSERT(0==(r=json_cf_object(_i32,30, &(ObjectInfo){.typeInf=&T_INF_INT32,.offset=&(i32)})));
	if(!r)	CU_ASSERT(strcmp(_i32,"2147483647")==0);
	ui32=0;
	CU_ASSERT(0==(r=json_cf_object(_ui32,30, &(ObjectInfo){.typeInf=&T_INF_UINT32,.offset=&(ui32)})));
	if(!r)	CU_ASSERT(strcmp(_ui32,"0")==0);
	ui32=4294967295;
	CU_ASSERT(0==(r=json_cf_object(_ui32,30, &(ObjectInfo){.typeInf=&T_INF_UINT32,.offset=&(ui32)})));
	if(!r)	CU_ASSERT(strcmp(_ui32,"4294967295")==0);
	ui64=0;
	CU_ASSERT(0==(r=json_cf_object(_ui64,30, &(ObjectInfo){.typeInf=&T_INF_UINT64,.offset=&(ui64)})));
	if(!r)	CU_ASSERT(strcmp(_ui64,"0")==0);
	ui64=(unsigned long long)(~0UL);
	i64=(long long)-(~0ULL>>1);
	CU_ASSERT(0==(r=json_cf_object(_ui64,30, &(ObjectInfo){.typeInf=&T_INF_UINT64,.offset=&(ui64)})));
	if(!r)	CU_ASSERT(strcmp(_ui64,"18446744073709551615")==0);
	CU_ASSERT(0==(r=json_cf_object(_i64,30, &(ObjectInfo){.typeInf=&T_INF_INT64,.offset=&(i64)})));
	if(!r)	CU_ASSERT(strcmp(_i64,"-9223372036854775807")==0);
	i64=(long long)(~0ULL>>1)-1;
	CU_ASSERT(0==(r=json_cf_object(_i64,30, &(ObjectInfo){.typeInf=&T_INF_INT64,.offset=&i64})));
	if(!r)	CU_ASSERT(strcmp(_i64,"9223372036854775806")==0);
}

static void tc_achar_convert(void){

	int r;
	char c=0,str[5];
	for(int i=1;i<128;++i){
		c=i;
		r=json_cf_object(str,5, &(ObjectInfo){.typeInf=&T_INF_ACHAR,.offset=&c});
		CU_ASSERT(0==r);
		if(!r){	CU_ASSERT(str[1]==i||(str[2]=='\\'||str[2]=='"'||str[2]=='r'||str[2]=='t'||str[2]=='f'||str[2]=='n'||str[2]=='b'));}
	}
}

static void tc_bool_convert(void){
	char r,str[6],f=0;
	CU_ASSERT(0==(r=json_cf_object(str,6, &(ObjectInfo){.typeInf=&T_INF_BOOLEAN,.offset=&f})));
	if(!r){	CU_ASSERT(strcmp(str,"false")==0);}
	f=1;
	CU_ASSERT(0==(r=json_cf_object(str,6, &(ObjectInfo){.typeInf=&T_INF_BOOLEAN,.offset=&f})));
	if(!r){	CU_ASSERT(strcmp(str,"true")==0);}
}

static void tc_string_convert(void){
	char r,
		str[512]={0},s[512]="asdfghjklqwertyuiopzxcvbnm123456789!@#$%^&*()_+|[];',./-=QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?\\\"";
	CU_ASSERT(0==(r=json_cf_object(str,512, &(ObjectInfo){.typeInf=&T_STRING(512),.offset=&s})));
	if(!r){	CU_ASSERT(strcmp(str,"\"asdfghjklqwertyuiopzxcvbnm123456789!@#$%^&*()_+|[];',./-=QWERTYUIOP{}ASDFGHJKL:ZXCVBNM<>?\\\\\""));}
}

const struct TC_IND ti_to_str_numeric_convert={
		.tc_num=4,
		.caseMap={
			{"TC numeric convert",tc_numeric_convert},
			{"TC ASCII char convert",tc_achar_convert},
			{"TC boolean convert",tc_bool_convert},
			{"TC string convert",tc_string_convert}
		}
};
