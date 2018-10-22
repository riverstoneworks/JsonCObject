/*
 * test.h
 *
 *  Created on: Aug 24, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */

#ifndef TC_SUITE_UT_H_
#define TC_SUITE_UT_H_
#include "../test.h"
extern const struct TC_IND ti_basic_type_convert_to_obj;
extern const struct TC_IND ti_basic_type_convert_to_str;
extern const struct TC_IND* tc_ind_uc[2];
//
extern int init_suite_UT(void);

extern int clean_suite_UT(void);

#endif /* TC_SUITE_UT_H_ */
