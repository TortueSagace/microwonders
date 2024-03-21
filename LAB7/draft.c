#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int draft() {

    long length = 16;
    char * inbuffer = (char*)malloc(length*sizeof(char));
    for (int i=0;i<16;i++) inbuffer[i] = 42;
    char * outbuffer = (char*)malloc(length*sizeof(char));
    // -- Print initial data:
    printf("%p, %d\n",outbuffer, length);
    for (int i=0;i<16;i++) printf("%d ",outbuffer[i]);
    printf("\n");

    __asm__(
    "mov %[in], %%xmm0\n"
    "mov %[out], %%xmm1\n"
    "mov %[l], %%xmm2;\n"
    "movdqu (%%xmm0), %%xmm7\n"
    "movdqu %%xmm7, (%%xmm1)\n"
    "add $16, %%xmm0\n"
    "sub $16, %%xmm2\n"
    ://outputs
    :[in]"m" (inbuffer), [out]"m" (outbuffer), [l]"m" (length) //inputs
    : "xmm1", "xmm0", "xmm2", "xmm7" //clobbers
    );

    // -- Print result:
    printf("%p, %d\n",outbuffer, length);
    for (int i=0;i<16;i++) printf("%d ",outbuffer[i]);
    printf("\n");
    return 0;
    }
