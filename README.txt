In this package, you will see the following files:

00_Configuration:        Contain CPU, GPU, Memory configuration files
00_Report                After 03 and 04, the reports will be generated here
                         Note that the reports will be covered every running 04.
01_COMPILE_ON_NATIVE.sh* Could run on native after 01
03_COMPILE_ON_M2S.sh*    Compile source code by using M2S's library
04_RUN_ON_M2S.sh*        Start to simulate on M2S
09_CLEAN_REPORT.sh*      Clean all report files

parameter.h
mm.cl         Write your code here
mm.c          Write your code here
              This .c file contains the function which is used to verify your GPU functionality
              function: "Matrix_Mult_CPU" do matrix multiplication by CPU
              function: "Verify" could check GPU's result with CPU
              You could use this function when writing OpenCL code.
			  
If there is any question, please contact with the TA.
Thanks.

TA: 
Zi-Hao, Liao
ceremony1116@gmail.com
