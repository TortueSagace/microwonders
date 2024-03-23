#include "draft.h"

int draft(void) {

    int length = 32;
    unsigned char * buffer = (unsigned char*)malloc((length+1)*sizeof(char));
    //for (int i=0;i<length;i++) buffer[i] = 15;
    buffer[16] = 17;
    buffer[15] = 14;
    buffer[14] = 16;
    buffer[13] = 15;
    unsigned char* threshold;
    threshold = (unsigned char *) malloc (16*sizeof(unsigned char));
    for(int i=0; i<length; i++) *(threshold+i) = 15;
    // -- Print initial data:
    printf("%x, %d\n",buffer, length);
    for (int i=0;i<length;i++) printf("%d ",buffer[i]);
    printf("\n");

    __asm__(
    "mov %[in], %%ebx\n"
    "mov %[mask], %%eax\n"
    "mov $2, %%ecx\n"

    "test %%ecx, %%ecx\n"
    "je end\n"
    "1:\n"
    "movdqu (%%ebx), %%xmm0\n"
    "movdqu (%%eax), %%xmm7\n"
    "pcmpgtb %%xmm7, %%xmm0\n"
    "movdqu %%xmm0, (%%ebx)\n"
    "add $16, %%ebx\n"
    "dec %%ecx\n"
    "jnz 1b\n"
    "end: \n"

    : // No outputs
    : [mask] "m" (threshold), [in] "m" (buffer)// Use "r" for loop counter to allow register usage
    : "eax", "ebx", "edx", "ecx", "xmm0", "xmm7", "memory" // Added "edi" to clobbers
);

    // -- Print result:
    printf("%x, %d\n",buffer, length);
    for (int i=0;i<length;i++) printf("%d ",buffer[i]);
    printf("\n");
    return 0;
    }
