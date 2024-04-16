#include "header.h"

int issimd(char* filename, int width, int height){
    clock_t start, end;
    start = clock();

    int W=width, H=height; // Image size fixed (or from header)
    signed char *src; // Pointers to arrays
    signed char *dst;

    int ii= (int) ((H-(3-1)*W))/16; // Set the counter

    printf("%d \n", ii);

    // Allocate memory
    src = (signed char *) malloc (W*H*sizeof(signed char));
    dst = (signed char *) malloc (W*H*sizeof(signed char));
    //W = ( int ) malloc (sizeof(int));
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
        printf("Can�t open specified file!");
        free(src);
        exit(1);
    }

__asm__(
    "mov %[in], %%ebx\n"        // data in ptr of the line
    "mov %[l], %%ecx\n"         // counter
    "mov %[out], %%edx\n"       // data out pointer
    "mov %[width], %%eax\n"

    "test %%ecx, %%ecx\n"
    "je end\n"
    "1:\n"
    "movdqu (%%ebx), %%xmm0\n"
    "movdqu %%xmm0, (%%edx)\n"
    "add $16, %%ebx\n"
    "add $16, %%edx\n"
    "sub $1, %%ecx\n"
    "jnz 1b\n"
    "end:\n"

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
        fwrite(dst, sizeof(signed char), W*H, fp2);
        fclose(fp2);} // we close the file
    else {
        printf("Can�t open specified file!");
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

