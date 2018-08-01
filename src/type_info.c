/*
 * type_info.c
 *
 *  Created on: Jul 24, 2018
 *      Author: Like.Z
 */
#include "type_info.h"

const TypeInf T_INF_INT={
		.name="int",
		.type=INT,
		.size={sizeof(int)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_UINT={
		.name="unsigned int",
		.type=UINT,
		.size={sizeof(unsigned int)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_ULONG={
		.name="unsigned long",
		.type=ULONG,
		.size={sizeof(unsigned long)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_ULLONG={
		.name="unsigned long long",
		.type=ULLONG,
		.size={sizeof(unsigned long long)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_LONG={
		.name="long",
		.type=LONG,
		.size={sizeof(long)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_LLONG={
		.name="long long",
		.type=LLONG,
		.size={sizeof(long long)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_FLOAT={
		.name="float",
		.type=FLOAT,
		.size={sizeof(float)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_DOUBLE={
		.name="double",
		.type=DOUBLE,
		.size={sizeof(double)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_LDOUBLE={
		.name="long double",
		.type=LDOUBLE,
		.size={sizeof(long double)},
		.subObjInfo=(void*)(0)
};

const TypeInf T_INF_BOOLEAN={
		.name="boolean",
		.type=BOOLEAN,
		.size={sizeof(char)},
		.subObjInfo=(void*)(0)
};

