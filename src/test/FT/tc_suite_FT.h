/*
 * test.h
 *
 *  Created on: Aug 24, 2018
 *      Author: Like.Z(sxpc722@aliyun.com)
 */

#ifndef TC_SUITE_FT_H_
#define TC_SUITE_FT_H_
#include "../test.h"
extern const struct TC_IND ti_single_object;
extern const struct TC_IND ti_object_array;
extern const struct TC_IND* tc_ind_fc[2];

extern int init_suite_FT(void);

extern int clean_suite_FT(void);

#endif /* TC_SUITE_FT_H_ */
