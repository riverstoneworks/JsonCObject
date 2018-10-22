/*
 * tc_suite_FT.c
 *
 *  Created on: Aug 25, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */
#include <locale.h>
#include "tc_suite_FT.h"


const struct TC_IND* tc_ind_fc[]={
		&ti_single_object,
		&ti_object_array
};
/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite_FT(void){
	if(!strstr(setlocale(LC_CTYPE, NULL),"UTF-8"))
		if(!strstr(setlocale(LC_CTYPE, ""),"UTF-8"))
			setlocale(LC_CTYPE, "en_US.UTF-8");

    return json_cto_object_init();
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite_FT(void){
	return json_cto_object_destroy();
}
