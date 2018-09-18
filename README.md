# **JsonCObject API Reference Manual**

## I. API

### I.0. Interface Summary
   Return Type|Tag|Parameter
   :---:|:--:|:---:
   int | json_cto_object_init | (void)
   int | json_cto_object | (char* , ObjectInfo*)
   int | json_cf_object | (char* ,size_t , ObjectInfo*)
   int | json_cto_object_destroy | (void)
	 
### I.1. json_cto_object_init
   Necessarily initialize internal objects once before run json_cto_object.
   - Return :
	 >0 for success or -1 for failure.
	
### I.2. json_cto_object : 
   Convert JSON to a C-Object, an instance of some structure.
   - Parameters:
      1. ***char\* jsonStr***
         >It points to JSON string to be converted.
      2. ***ObjectInfo\* objInfo*** : 
         >It points to an instance of ***ObjectInfo*** which is a structure to describe the information about an object (an  instance of some structure).
   - Return:
     >type:***int*** , 0 for success or -1 for failure.
   - **Note**:
     >Before this, set locate "LC\_CTYPE;UTF-8" for Unicode char transform between utf-8 and utf-16. (e.g. UTF-16 char "\u040F" in JSON string)
     ```C
				 if(!strstr(setlocale(LC\_CTYPE, NULL),"UTF-8")) 
					if(!strstr(setlocale(LC\_CTYPE, ""),"UTF-8"))
						setlocale(LC\_CTYPE, "en\_US.UTF-8"); 
     ```
### I.3. json_cf_object
   Convert object to JSON string
   - Parameters:
      1. char* jsonStr
         >It points to buffer which is used to store the result. And it will cause failure when the buffer size is not enough.
      2. size_t size
         >The length of the buffer pointed by jsonStr.
      3. ObjectInfo\* objInfo
         >It points to an instance of ObjectInfo which is a structure to describe the information about an object (an  instance of some structure).
   - Return:
	 >0 for success or -1 for failure.

### I.4. json\_cto\_object\_destroy 
   Clear internal objects at the final of conversion.
   - Return:
	 >0 for success or -1 for failure.

## II. Example
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

## III. NOTE
   - See project Type-Info for more detail about ObjectInfo


