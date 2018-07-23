/*
 * Parse.h
 *
 *  Created on: Jun 27, 2018
 *      Author: Like.Z
 */

#ifndef PARSE_H_
#define PARSE_H_
#include "type_info.h"


int json_convert_init();
int json_convert_to_obj(char* , ObjectInfo*);
int json_convert_from_obj(char* , ObjectInfo*);
int json_convert_destroy();

int verify_test(int i,char* s);
#endif /* PARSE_H_ */
