#include <stdio.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
	#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

extern void SWV_Init(void);

int main(void) {
	SWV_Init();

	printf("Hello World\r\n");

	for (;;);
}
