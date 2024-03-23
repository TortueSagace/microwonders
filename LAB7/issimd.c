#include "issimd.h"

int issimd(char* filename, int width, int height){
    clock_t start, end;
    start = clock();

    int W=width, H=height; // Image size fixed (or from header)
    unsigned char *threshold; // color threshold
    unsigned char *src; // Pointers to arrays
    unsigned char *dst;

    // Allocate memory
    src = (unsigned char *) malloc (W*H*sizeof(unsigned char));
    dst = (unsigned char *) malloc (W*H*sizeof(unsigned char));
    threshold = (unsigned char *) malloc (16*sizeof(unsigned char));
    for(int i=0; i<16; i++) *(threshold+i) = 127;
    // Check if enough memory
    if (src == NULL || dst == NULL) {
        printf ("Out of memory!");
        exit (1);
    }
    // File pointer, open and read

    FILE *fp1 = fopen(filename,"r");
    if (fp1 != NULL) { // read only if file opened
    fread(src, sizeof(unsigned char), W*H, fp1);
    fclose(fp1);} // we close the file
    else {
        printf("Can’t open specified file!");
        free(src);
        exit(1);
    }

    int ii=H*W/16; // Set the counter


    __asm__(
    "mov %[in], %%ebx\n"
    "mov %[out], %%edx\n"
    "mov %[mask], %%eax\n"
    "mov %[l], %%ecx\n" // Removed semicolon

    "test %%ecx, %%ecx\n"
    "je end\n"
    "1:\n"
    "movdqu (%%ebx), %%xmm0\n"
    "movdqu (%%eax), %%xmm7\n"
    "pcmpgtb %%xmm7, %%xmm0\n"
    "movdqu %%xmm0, (%%edx)\n"
    "add $16, %%ebx\n"
    "add $16, %%edx\n"
    "dec %%ecx\n"
    "jnz 1b\n"
    "end:\n"
    : // No outputs
    : [mask] "m" (threshold), [in] "m" (src), [out] "m" (dst), [l] "r" (ii) // Use "r" for loop counter to allow register usage
    : "eax", "ebx", "edx", "ecx", "xmm0", "xmm7", "memory" // Added "edi" to clobbers
);

    char *prefix = (char *) malloc((strlen(filename)-4+1)*sizeof(char));
    for(int i=0; i<(strlen(filename)-4+1); i++){
            *(prefix+i)=*(filename+i);
            if(i==(strlen(filename)-4)) *(prefix+i)='\0';
    }

    char *suffix = "out_SIMD.raw";
    char *destFilename = (char *) malloc((strlen(prefix)+13)*sizeof(char));
    strcpy(destFilename,prefix);
    strcat(destFilename,suffix);

    FILE *fp2 = fopen(destFilename,"wb");
    if (fp2 != NULL) { // read only if file opened
        fwrite(dst, sizeof(unsigned char), W*H, fp2);
        fclose(fp2);} // we close the file
    else {
        printf("Can’t open specified file!");
        free(dst);
        exit(1);
    }

    free(src); // to avoid memory leaks
    free(dst);
    free(destFilename);
    free(suffix);
    end = clock();
    float time = (float)(end - start)/CLOCKS_PER_SEC;
    printf("Time spent: %f\n", time);

    return 0;
}

