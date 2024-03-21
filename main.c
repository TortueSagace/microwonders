#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
    clock_t start, end;
    start = clock();

    int W=512, H=256; // Image size fixed (or from header)
    unsigned char threshold = 128; // color threshold
    unsigned char *src; // Pointers to arrays
    unsigned char *dst;

    // Allocate memory
    src = (unsigned char *) malloc (W*H*sizeof(unsigned char));
    dst = (unsigned char *) malloc (W*H*sizeof(unsigned char));
    // Check if enough memory
    if (src == NULL || dst == NULL) {
        printf ("Out of memory!");
        exit (1);
    }
    // File pointer, open and read

    FILE *fp1 = fopen("parrots_512x256.raw","r");
    if (fp1 != NULL) { // read only if file opened
    fread(src, sizeof(unsigned char), W*H, fp1);
    fclose(fp1);} // we close the file
    else {
        printf("Can’t open specified file!");
        free(src);
        exit(1);
    }

    // ... we can now do something with src and dst arrays
    for(int i=0; i<W*H; i++){
        if(*(src+i)<threshold){
            *(dst+i)=0;
        }
        else *(dst+i)=255;
    }

    FILE *fp2 = fopen("parrots_512x256threshold.raw","wb");
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
    end = clock();
    float time = (float)(end - start)/CLOCKS_PER_SEC;
    printf("Time spent: %f\n", time);

    return 0;
}
