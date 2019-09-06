
MESA_18_0_LIB_PATH=/home/liujie/source/mesa/libs/18.0.5/lib
MESA_18_3_LIB_PATH=/home/liujie/source/mesa/libs/18.3-icd/lib/
MESA_18_3_NO_ICD_LIB_PATH=/home/liujie/source/mesa/libs/18.3-none-icd/lib
MESA_18_2_NO_ICD_LIB_PATH=/home/liujie/source/mesa/libs/18.2-none-icd/lib
#MESA_LIB_PATH=$(MESA_18_0_LIB_PATH)
MESA_LIB_PATH=$(MESA_18_3_LIB_PATH)
#MESA_LIB_PATH=$(MESA_18_3_NO_ICD_LIB_PATH)
MESA_LIB_PATH=$(MESA_18_2_NO_ICD_LIB_PATH)
MESA_LIB=$(MESA_LIB_PATH)/libOpenCL.so.1 $(MESA_LIB_PATH)/libdrm_amdgpu.so
#MESA_LIB_PATH=/usr/lib/x86_64-linux-gnu
#MESA_LIB=$(MESA_LIB_PATH)/libOpenCL.so.1

default:
	gcc -I/home/liujie/source/mesa/libs/18.2-none-icd/include devices.c -g -o device $(MESA_LIB)
	gcc hello.c -I-I/home/liujie/source/mesa/libs/18.2-none-icd/include -g -o hello $(MESA_LIB)
	gcc cl_test.c -I/home/liujie/source/mesa/libs/18.2-none-icd/include -g -o cl_test $(MESA_LIB) -lm
	export LD_LIBRARY_PATH=$(MESA_LIB_PATH)

