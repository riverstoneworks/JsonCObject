# **JsonCObject API Reference Manual**
## I. Purpose
Map JSON and C Object to each other.
## II. Mapping Rule
### II.1 Type Map
   JSON Type|C Type|note
   :---:|:--:|:---:
   numeric | arithmetic type | 
   string | char array or single char |
   array | array | 
   boolean | _Bool | 1: ture, 0: false
   object | struct |
### II.2 TypeInf & ObjectInfo
   1. Mapping is dependent on [C type decription][1] with two structs. *TypeInf* describes type infomation and *ObjectInfo* describes object information. The relationship between them is as following:
      > ![](https://raw.github.com/riverstoneworks/CTypeInfo/master/doc/TypeInf_ObjectInfo.svg?sanitize=true)
   2. It is necessary to creates an *ObjectInfo* instance for the object to participate in conversion. For example:
      ```C
      struct A {
		int  age;
		char title[20];
      }a;

	  static const TypeInf T_INF_A=(TypeInf){
		  .name="struct A",
		  .type=OBJECT,
		  .size={24,2},
		  .subObjInfo=(ObjectInfo*)&(ObjectInfo){
			  {
				  .name="age",
				  .typeInf=TYPE_INF_INT,
				  .offset=0
			  },{
				  .name="title",
				  .typeInf={
					  .name="char20",
					  .type=STRING,
					  .size={20,20},
					  .subObjInfo=NULL
				  },
				  .offset=(void*)4
			  }
		  }
	  }

	  ObjectInfo a_inf={
		  .name="a",
		  .typeInfo=&T_INF_A,
		  .offset=(void*)&a
	  }
	  ```
   3. See project [CTypeInfo][1] for more information about C type decription.
### II.3 Object Mapping
   1. In JSON, an object is a wrapper around a set of data, just like struct in C. And they can map to each other.
   2. It use Regex Match Tracer to grep key-value in JSON.
   3. Mapping is dependent on the object description by *ObjectInfo*.
   4. The type & name of elements in JSON object and C object must match. For example:
	  C struct
      ```C
      struct A {
		int  age;
		char title[20];
      };
      ```
	  match JSON
	  ```json
      {"age":25,"title":"engineer"}
	  ```
### II.4 Array Mapping
Similar to Object Mapping.
### II.5 Numeric Mapping Accuracy
   1. It depends on C language data types used for mapping, because JSON can represent any number without limit in range and precision.
   2. And it was impossible to map floating-point type to integer type and vice versa.
### II.6 String Mapping
   1. It supports dealing with UTF-8 encoded text.
   2. It supports UTF-16 code string in JSON. For example:
      > "\u4E2D" in JSON can be converted to '中' in C String and and vice versa.
   3. Overflow will be truncated or it may cause failure due to insufficient text buffer capacity allocation.
## III. API
### III.0. Interface Summary
   Return Type|Tag|Parameter
   :---:|:--:|:---:
   int | json_cto_object_init | (void)
   int | json_cto_object | (char* , ObjectInfo*)
   int | json_cf_object | (char* ,size_t , ObjectInfo*)
   int | json_cto_object_destroy | (void)

### III.1. json_cto_object_init
   Necessarily initialize internal objects once before run json_cto_object.
   - Return :
	 >0 for success or -1 for failure.	
### III.2. json_cto_object : 
   Convert JSON to a C-Object, an instance of some structure.
   - Parameters:
      1. *char\* jsonStr*
         >It points to JSON string to be converted.
      2. *ObjectInfo\* objInfo*
         >It points to an instance of *ObjectInfo* which is a structure to describe the information about an object (an  instance of some structure).
   - Return:
     >0 for success or -1 for failure.
   - **Note**:
     >Before this, set environment variable LC_CTYPE as UTF-8, so that it can convert Unicode characters between utf-8 and utf-16 correctly. Just do it like this:
     ```C
	 if(!strstr(setlocale(LC_CTYPE, NULL),"UTF-8")) 
		if(!strstr(setlocale(LC_CTYPE, ""),"UTF-8"))
			setlocale(LC_CTYPE, "en_US.UTF-8"); 
     ```
### III.3. json_cf_object
   Convert object to JSON string
   - Parameters:
      1. *char\* jsonStr*
         >It points to buffer which is used to store the result. And it will cause failure when the buffer size is not enough.
      2. *size_t size*
         >The length of the buffer pointed by jsonStr.
      3. *ObjectInfo\* objInfo*
         >It points to an instance of ObjectInfo which is a structure to describe the information about an object (an instance of some structure).
   - Return:
	 >0 for success or -1 for failure.
### III.4. json_cto_object_destroy 
   Clear internal objects at the final.
   - Return:
     >0 for success or -1 for failure.
## IV. Example
   ```C
		#include "jsonCObject.h"
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

		/*
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

			int r = json_cto_object(string, &obj_info_a);
			if (!r&&
				(a.a == 5741)&&
				(fabs(a.b - -1.026f) < 0.000001)&&
				(a.flag = 1)&&
				(0 == strcmp(a.x->y.z, "䐠book"))&&
				(a.d[0] == 235)&&
				(fabs(a.d[1] - 6.850000f) < 0.000001)&&
				(fabs(a.d[2] - -52.360000f) < 0.000001)&&
				(a.d[3] == 352)&&
				(a.llongs[0] == 23525406)&&
				(a.llongs[1] == -68562342)&&
				(a.llongs[2] == 52754128)&&
				(a.llongs[3] == -98702540)
				){
				puts(“success!”);
			}else
				puts(“failed!”);
		}

		//TC object to json
		void tc_json_cf_obj(void){
			AA a = { .a = 5741,
					.b = -1.026f,
					.flag = 1,
					.x = NULL,
					.d = {235,6.85,-52.36,352,0,0,0,0,0,0},
					.llongs ={23525406,-68562342,52754128,-98702540,0,0,0,0,0,0}
			};
			ObjectInfo obj_info_a = {
				.name = "a", 
				.offset = &a,
				.typeInf = &T_INF_AA };

			char json_str[512],
			*result="{\"a\":5741,\"b\":-1.026,\"flag\":true,\"x\":null,\"d\":[235,6.85,-52.36,352,0,0,0,0,0,0],\"llongs\":[23525406,-68562342,52754128,-98702540,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]}";
			int r=json_cf_object(json_str, 512, &obj_info_a);
			if(!r&&(0==strcmp(json_str,result))){
				puts(“success!”);
			}else
				puts(“failed!”);
		}

		int main(void){
			// Set locale character code
			if(!strstr(setlocale(LC_CTYPE, NULL),"UTF-8"))
				if(!strstr(setlocale(LC_CTYPE, ""),"UTF-8"))
					setlocale(LC_CTYPE, "en_US.UTF-8");
			// Initialize
			json_cto_object_init();
			//convert JSON to object
			tc_json_cto_obj(void) ;
			//convert object to JSON
			tc_json_cf_obj(void);
			// Clear up
			json_cto_object_destroy();
		}
   ```

## V. NOTE
   - JsonCObject on github: https://github.com/riverstoneworks/JsonCObject

[1]:https://github.com/riverstoneworks/CTypeInfo
