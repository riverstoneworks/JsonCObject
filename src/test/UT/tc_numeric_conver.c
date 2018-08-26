/*
 * tc_numeric_conver.c
 *
 *  Created on: Aug 26, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */
#include <CUnit/Basic.h>
#include "../../jsonCObject.h"
#include "tc_suite_UT.h"

void tc_numeric_conver(void){
	char c;
	unsigned char uc;
	int I;
	unsigned int ui;
	short si;
	unsigned short us;
	long int li;
	unsigned long ul;
	long long ll;
	unsigned long long ull;
	char *_c[]={"-128","127"},
	*_uc[]={"0","255"},
	*_si[]={"-32768","32767"},
	*_us[]={"0","65535"},
	*_i[]={"-2147483648","2147483647"},
	*_ui[]={"0","4294967295"},
	*_l[]={"-9223372036854775807","9223372036854775806"},
	*_ul[]={"0","18446744073709551615"},
	*_ll[]={"-9223372036854775807","9223372036854775806"},
	*_ull[]={"0","18446744073709551615"};


	int r;
	CU_ASSERT(0==(r=json_cto_object(_c[0], &(ObjectInfo){.typeInf=&T_INF_CHAR,.offset=&c})));
	if(!r)	CU_ASSERT(c==-128);
	CU_ASSERT(0==(r=json_cto_object(_c[1], &(ObjectInfo){.typeInf=&T_INF_CHAR,.offset=&c})));
	if(!r)	CU_ASSERT(c==127);
	CU_ASSERT(0==(r=json_cto_object(_uc[0], &(ObjectInfo){.typeInf=&T_INF_UCHAR,.offset=&uc})));
	if(!r)	CU_ASSERT(uc==0);
	CU_ASSERT(0==(r=json_cto_object(_uc[1], &(ObjectInfo){.typeInf=&T_INF_UCHAR,.offset=&uc})));
	if(!r)	CU_ASSERT(uc==255);
	CU_ASSERT(0==(r=json_cto_object(_si[0], &(ObjectInfo){.typeInf=&T_INF_SHORT,.offset=&si})));
	if(!r)	CU_ASSERT(si==-32768);
	CU_ASSERT(0==(r=json_cto_object(_si[1], &(ObjectInfo){.typeInf=&T_INF_SHORT,.offset=&si})));
	if(!r)	CU_ASSERT(si==32767);
	CU_ASSERT(0==(r=json_cto_object(_us[0], &(ObjectInfo){.typeInf=&T_INF_USHORT,.offset=&us})));
	if(!r)	CU_ASSERT(us==0);
	CU_ASSERT(0==(r=json_cto_object(_us[1], &(ObjectInfo){.typeInf=&T_INF_USHORT,.offset=&us})));
	if(!r)	CU_ASSERT(us==65535);
	CU_ASSERT(0==(r=json_cto_object(_i[0], &(ObjectInfo){.typeInf=&T_INF_INT,.offset=&I})));
	if(!r)	CU_ASSERT(I==-2147483648);
	CU_ASSERT(0==(r=json_cto_object(_i[1], &(ObjectInfo){.typeInf=&T_INF_INT,.offset=&I})));
	if(!r)	CU_ASSERT(I==2147483647);
	CU_ASSERT(0==(r=json_cto_object(_ui[0], &(ObjectInfo){.typeInf=&T_INF_UINT,.offset=&ui})));
	if(!r)	CU_ASSERT(ui==0);
	CU_ASSERT(0==(r=json_cto_object(_ui[1], &(ObjectInfo){.typeInf=&T_INF_UINT,.offset=&ui})));
	if(!r)	CU_ASSERT(ui==4294967295);
	CU_ASSERT(0==(r=json_cto_object(_l[0], &(ObjectInfo){.typeInf=&T_INF_LONG,.offset=&li})));
	if(!r)	CU_ASSERT(li==(long)-(~0UL>>1));
	CU_ASSERT(0==(r=json_cto_object(_l[1], &(ObjectInfo){.typeInf=&T_INF_LONG,.offset=&li})));
	if(!r)	CU_ASSERT(li==(long)(~0UL>>1)-1);
	CU_ASSERT(0==(r=json_cto_object(_ul[0], &(ObjectInfo){.typeInf=&T_INF_ULONG,.offset=&ul})));
	if(!r)	CU_ASSERT(ul==0);
	CU_ASSERT(0==(r=json_cto_object(_ul[1], &(ObjectInfo){.typeInf=&T_INF_ULONG,.offset=&ul})));
	if(!r)	CU_ASSERT(ul==(unsigned long)(~0UL));
	CU_ASSERT(0==(r=json_cto_object(_ll[0], &(ObjectInfo){.typeInf=&T_INF_LLONG,.offset=&ll})));
	if(!r)	CU_ASSERT(ll==(long long)-(~0ULL>>1));
	CU_ASSERT(0==(r=json_cto_object(_ll[1], &(ObjectInfo){.typeInf=&T_INF_LLONG,.offset=&ll})));
	if(!r)	CU_ASSERT(ll==(long long)(~0ULL>>1)-1);
	CU_ASSERT(0==(r=json_cto_object(_ull[0], &(ObjectInfo){.typeInf=&T_INF_ULLONG,.offset=&ull})));
	if(!r)	CU_ASSERT(ull==0);
	CU_ASSERT(0==(r=json_cto_object(_ull[1], &(ObjectInfo){.typeInf=&T_INF_ULLONG,.offset=&ull})));
	if(!r)	CU_ASSERT(ull==(unsigned long long)(~0ULL));
}

const struct TC_IND ti_numeric_convert={
		.tc_num=1,
		.caseMap={
			{"TC numeric convert",tc_numeric_conver}
		}
};
