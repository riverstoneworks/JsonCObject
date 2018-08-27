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

static const struct TYPE_INF T_INF_AA = T(AA, 6, O(a,T_INF_INT32,0), O(b,T_INF_DOUBLE,8),
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
		O(llongs,T_ARRAY(long long,20,T_INF_INT64),192));


/* Simple test of json_cto_object().
 * TC json to object
 */
void tc_json_cto_obj(void) {
	AA a;memset(&a,0,sizeof(a));
    ObjectInfo obj_info_a={
    				.name="a",
					.offset=&a,
					.typeInf=&T_INF_AA
    };
    char string[512] = "{\"a\":5741,\"b\": -1.026, \"flag\" : true, \"x\" : {\"y\" : {\"z\" : \"\\u4420book\"} } , \"d\" : [235, 6.85,-52.36 ,352], \"llongs\" :[ [[ 23525406, -68562342],[52754128 ,-98702540]]]}";
	int r;
	CU_ASSERT(0 == (r = json_cto_object(string, &obj_info_a)));
	if (!r) {
		CU_ASSERT(a.a == 5741);
		CU_ASSERT(fabs(a.b - -1.026f) < 0.000001);
		CU_ASSERT(a.flag = 1);
		CU_ASSERT(0 == strcmp(a.x->y.z, "䐠book"));
		CU_ASSERT(a.d[0] == 235);
		CU_ASSERT(fabs(a.d[1] - 6.850000f) < 0.000001);
		CU_ASSERT(fabs(a.d[2] - -52.360000f) < 0.000001);
		CU_ASSERT(a.d[3] == 352);
		CU_ASSERT(a.llongs[0] == 23525406);
		CU_ASSERT(a.llongs[1] == -68562342);
		CU_ASSERT(a.llongs[2] == 52754128);
		CU_ASSERT(a.llongs[3] == -98702540);
	}
}

/* Simple test of json_cf_object().
 * TC object to json
 */
void tc_json_cf_obj(void){
	AA a = { .a = 5741,
			.b = -1.026f,
			.flag = 1,
			.x = NULL,
			.d = { 235, 6.85,-52.36, 352,0,0,0,0,0,0 },
			.llongs ={ 23525406, -68562342, 52754128, -98702540 ,0,0,0,0,0,0}
	};
	ObjectInfo obj_info_a = { .name = "a", .offset = &a, .typeInf = &T_INF_AA };

	char json_str[512],
	*result="{\"a\":5741,\"b\":-1.026,\"flag\":true,\"x\":null,\"d\":[235,6.85,-52.36,352,0,0,0,0,0,0],\"llongs\":[23525406,-68562342,52754128,-98702540,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}";
	int r;
	CU_ASSERT(0==(r=json_cf_object(json_str, 512, &obj_info_a)));
	if(!r){
		CU_ASSERT(0==strcmp(json_str,result));
	}
}

const struct TC_IND ti_single_object={
		.tc_num=2,
		.caseMap={
			{"TC Json_to_Object",tc_json_cto_obj},
			{"TC Object_to_Json",tc_json_cf_obj}
		}
};
