#include "draft.h"

int draft(void) {

    int length = 16;
    char * inbuffer = (char*)malloc(length*sizeof(char));
    for (int i=0;i<16;i++) inbuffer[i] = 42;
    char * outbuffer = (char*)malloc(length*sizeof(char));
    // -- Print initial data:
    printf("%p, %d\n",outbuffer, length);
    for (int i=0;i<16;i++) printf("%d ",outbuffer[i]);
    printf("\n");

    __asm__(
    "mov %[in], %%esi\n"
    "mov %[out], %%eax\n"
    "mov %[l], %%ecx;\n"
    "movdqu (%%esi), %%xmm7\n"
    "movdqu %%xmm7, (%%eax)\n"
    "add $16, %%esi\n"
    "sub $16, %%ecx\n"
    ://outputs
    :[in]"m" (inbuffer), [out]"m" (outbuffer), [l]"m" (length) //inputs
    : "eax", "esi", "ecx", "xmm7" //clobbers
    );

    // -- Print result:
    printf("%p, %d\n",outbuffer, length);
    for (int i=0;i<16;i++) printf("%d ",outbuffer[i]);
    printf("\n");
    return 0;
    }
