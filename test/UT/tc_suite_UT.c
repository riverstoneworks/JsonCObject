/*
 * tc_suite_FT.c
 *
 *  Created on: Aug 25, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */
#include <locale.h>
#include <string.h>
#include "tc_suite_UT.h"
#include "JsonCObject/jsonCObject.h"

const struct TC_IND* tc_ind_uc[]={
		&ti_basic_type_convert_to_obj,
		&ti_basic_type_convert_to_str
};

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite_UT(void){
	if(!strstr(setlocale(LC_CTYPE, NULL),"UTF-8"))
		if(!strstr(setlocale(LC_CTYPE, ""),"UTF-8"))
			setlocale(LC_CTYPE, "en_US.UTF-8");

    return json_cto_object_init();
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_UT(void){
	return json_cto_object_destroy();
}
