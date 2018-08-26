/*
 * tc_jsonCObject.c
 *
 *  Created on: Aug 24, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */
#include <CUnit/Basic.h>
#include "../../jsonCObject.h"
#include "tc_suite_FT.h"

struct X {
	struct Y {
		char z[20];
	} y;
};

typedef struct A{
	int a;
	double b;
	char flag;
	struct X* x;
	long double d[10];
	long long llongs[20];
}AA;

static const struct TYPE_INF T_INF_AA = T(AA, 6, O(a,T_INF_INT,0), O(b,T_INF_DOUBLE,8),
		O(flag,T_INF_BOOLEAN,16),
		O(x,T_PTR(struct X*,
					O(x,T(struct X,1,
						O(y,T(struct Y,1,
								O(z,T_STRING(20),0)
							),0)
					),0)
				),
		24),
		O(d,T_ARRAY(long double,10,T_INF_LDOUBLE),32),
		O(llongs,T_ARRAY(long long,20,T_INF_LLONG),192));


/* Simple test of json_cto_object().
 * TC json to object
 */
void tc_json_cto_obj_array(void) {
    AA a[2];
    memset(&a,0,sizeof(a));
	ObjectInfo a_array_2 = {
			.name = "a",
			.typeInf = &(struct TYPE_INF ) {
					.size = { sizeof(a) , 2 },
					.type = ARRAY,
					.subObjInfo = &(ObjectInfo ) {
								.typeInf = &T_INF_AA,
								.offset = NULL
					}
			},
			.offset = &a
	};
    char string[] = "[{\"a\":5741,\"b\": -102.6, \"flag\": true, \"x\" : null , \"d\" : [235, 6.85,-52.36 ,352], \"llongs\" : [23525406, -68562342,52754128 ,-28702540]},{\"a\":6741,\"b\": -102.6, \"flag\" : false, \"x\" : {\"y\" : {\"z\" : \"topup\"} } , \"d\" : [235, 6.85,-52.36 ,352], \"llongs\" : [23525406, -68562342,52754128 ,-78702540]}]";
	int r;
	CU_ASSERT(0 == (r = json_cto_object(string, &a_array_2)));
	if (!r) {
		CU_ASSERT(a[0].a == 5741);
		CU_ASSERT(fabs(a[0].b - -102.6) < 0.000001);
		CU_ASSERT(a[0].flag = 1);
		CU_ASSERT(0 == strcmp(a[1].x->y.z, "topup"));
		CU_ASSERT(a[0].x==NULL);
		CU_ASSERT(a[0].d[0] == 235);
		CU_ASSERT(fabs(a[0].d[1] - 6.850000f) < 0.000001);
		CU_ASSERT(fabs(a[0].d[2] - -52.360000f) < 0.000001);
		CU_ASSERT(a[0].d[3] == 352);
		CU_ASSERT(a[0].llongs[0] == 23525406);
		CU_ASSERT(a[0].llongs[1] == -68562342);
		CU_ASSERT(a[0].llongs[2] == 52754128);
		CU_ASSERT(a[0].llongs[3] == -28702540);
	}
}

/* Simple test of json_cf_object().
 * TC object to json
 */
void tc_json_cf_obj_array(void){

	AA a[2] = {
			{ .a = 5741,
			.b = -102.6f,
			.flag = 1,
			.x = &(struct X){
				.y={
						.z="book书"
				}
			},
			.d = { 235,6.85,-52.36,352,0,0,0,0,0,0 },
			.llongs ={ 23525406,-68562342,52754128,-98702540,0,0,0,0,0,0}
			},{
				.a = 6741,
				.b = -102.6f,
				.flag = 0,
				.x = NULL,
				.d = { 235,6.85,-52.36,352,0,0,0,0,0,0 },
				.llongs = { 23525406,-68562342,52754128,-78702540,0,0,0,0,0,0 }
			}
	};
	ObjectInfo a_array_2={
			.name="a",
			.typeInf =&(struct TYPE_INF){
				.size={sizeof(a),2},
				.type=ARRAY,
				.subObjInfo=&(ObjectInfo){
					.typeInf=&T_INF_AA,
					.offset =NULL
				}
			},
			.offset=&a
	};

	char json_str[2048];
	const char* result="[{\"a\":5741,\"b\":-102.6,\"flag\":true,\"x\":{\"y\":{\"z\":\"book书\"}},\"d\":[235,6.85,-52.36,352,0,0,0,0,0,0],\"llongs\":[23525406,-68562342,52754128,-98702540,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]},"
			"{\"a\":6741,\"b\":-102.6,\"flag\":false,\"x\":null,\"d\":[235,6.85,-52.36,352,0,0,0,0,0,0],\"llongs\":[23525406,-68562342,52754128,-78702540,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}]";
	int r;
	CU_ASSERT(0==(r=json_cf_object(json_str, 2048, &a_array_2)));
	if(!r){
		CU_ASSERT(0==strcmp(json_str,result));
	}
}

const struct TC_IND ti_object_array={
		.tc_num=2,
		.caseMap={
			{"TC Json_to_Object_array",tc_json_cto_obj_array},
			{"TC Object_to_Json_array",tc_json_cf_obj_array}
		}
};
