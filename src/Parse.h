/*
 * Parse.h
 *
 *  Created on: Jun 27, 2018
 *      Author: Like.Z
 */

#ifndef PARSE_H_
#define PARSE_H_
#include "type_info.h"


extern int json_cto_struct_init();
extern int json_cto_struct(char* , ObjectInfo*);
extern int json_convert_from_struct(char* , ObjectInfo*);
extern int json_cto_struct_destroy();

extern int verify_test(int i,char* s);
#endif /* PARSE_H_ */
