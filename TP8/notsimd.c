#include "header.h"

int notsimd(char* filename, int width, int height){
    clock_t start, end;
    start = clock();

    int W=width, H=height; // Image size fixed (or from header)
    signed char threshold = 0; // color threshold
    signed char *src; // Pointers to arrays
    signed char *dst;

    // Allocate memory
    src = (signed char *) malloc (W*H*sizeof(signed char));
    dst = (signed char *) malloc (W*H*sizeof(signed char));
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

    // Calling the contour extraction function
    dst = contourExtraction(src,W,H);


    char *prefix = (char *) malloc((strlen(filename)-4+1)*sizeof(char));
    for(int i=0; i<(strlen(filename)-4+1); i++){
            *(prefix+i)=*(filename+i);
            if(i==(strlen(filename)-4)) *(prefix+i)='\0';
    }

    char *suffix = "out_C.raw";
    char *destFilename = (char *) malloc((strlen(prefix)+10)*sizeof(char));
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
    float time = (float)1e3*(end - start)/CLOCKS_PER_SEC; // time in milliseconds
    printf("Time spent: %f\n", time);

    return 0;
}

char* contourExtraction(char* src,int width, int height){
    unsigned char *dst;
    unsigned char min;
    unsigned char max;
    dst = (unsigned char*) malloc (width*height*sizeof(unsigned char));
    for(int i=0; i<width*height; i++){
        if(i<width || i>width*(height-1) || i%width == 0 || i%width == width - 1){ //Is the pixel on the border of the image ?
            *(dst+i)=0;
        } else { // If not: we can implement the min/max filtering
            unsigned char list[] = {*(src+i-width-1),*(src+i-width),*(src+i-width+1),*(src+i-1),*(src+i),*(src+i+1),*(src+i+width-1),*(src+i+width),*(src+i+width+1)};
            min = list[0];
            max = list[0];
            for (int j = 0; j < 9; j++){
                if (list[j]<min) min = list[j];
                if (list[j]>max) max = list[j];
            }
            *(dst+i)=max-min;
        }
    }
    return dst;
}
