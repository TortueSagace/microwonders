#include "header.h"

int issimd(char* filename, int width, int height){
    clock_t start, end;
    start = clock();

    int W=width, H=height; // Image size fixed (or from header)
    signed char *src; // Pointers to arrays
    signed char *dst;

    // Allocate memory
    src = (signed char *) malloc (W*H*sizeof(signed char));
    dst = (signed char *) malloc (W*H*sizeof(signed char));
    W = ( int ) malloc (sizeof(int));
    // Check if enough memory
    if (src == NULL || dst == NULL) {
        printf ("Out of memory!");
        exit (1);
    }
    // File pointer, open and read

    FILE *fp1 = fopen(filename,"r");
    if (fp1 != NULL) { // read only if file opened
    fread(src, sizeof(signed char), W*H, fp1);
    fclose(fp1);} // we close the file
    else {
        printf("Can’t open specified file!");
        free(src);
        exit(1);
    }

    int ii=(H*(W-2)-2)/16; // Set the counter

__asm__(
    "mov %[in], %%ebx\n"        // datain ptr of the line
    "mov %[l], %%ecx\n"         // counter
    "mov %[out], %%edx\n"       // dataout pointer
    "mov %[width], %%eax\n"
    "l1:\n"
    "movdqu (%%ebx), %%xmm0\n"               // l1st line max
    "movdqu 512+%[in], %%xmm1\n"      // 2nd line max
    "movdqu 1024+%[in], %%xmm2\n"  // 3rd line max
    "pmaxub %%xmm1, %%xmm0\n"               // compare max
    "pmaxub %%xmm2, %%xmm0\n"

    "movdqu (%%ebx), %%xmm3\n"               // l1st line min
    "movdqu 512+%[in], %%xmm4\n"      // 2nd line min
    "movdqu 1024+%[in], %%xmm5\n"  // 3rd line min
    "pminub %%xmm4, %%xmm3\n"               // compare min
    "pminub %%xmm5, %%xmm3\n"

    "movdqu %%xmm0, %%xmm6\n" // copy max
    "movdqu %%xmm0, %%xmm7\n"
    "psrldq $1, %%xmm6\n" // shift max
    "psrldq $2, %%xmm7\n"
    "pmaxub %%xmm7, %%xmm6\n" // colon max
    "pmaxub %%xmm0, %%xmm6\n"

    "movdqu %%xmm3, %%xmm1\n" // copy min
    "movdqu %%xmm3, %%xmm2\n"
    "psrldq $1, %%xmm1\n" // shift min
    "psrldq $2, %%xmm2\n"
    "pminub %%xmm2, %%xmm1\n" // colon min
    "pminub %%xmm3, %%xmm1\n"

    "psubsb %%xmm1, %%xmm6\n"
    "movdqu %%xmm6, (%%edx)\n"
    "add $16, %%ebx\n"
    "add $16, %%edx\n"
    "sub $1, %%ecx\n"
    "jnz l1\n"
    : // No outputs
    : [in] "m" (src), [out] "m" (dst), [width] "m" (W), [l] "r" (ii) // Use "r" for loop counter to allow register usage
    : "eax", "ebx", "edx", "ecx", "memory", "xmm0", "xmm1", "xmm2", "xmm6", "xmm7", "xmm3", "xmm4", "xmm5" // Added "memory" to clobbers
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
    end = clock();
    float time = (float)1e3*(end - start)/CLOCKS_PER_SEC; //time in milliseconds
    printf("Time spent: %f\n", time);

    return 0;
}
