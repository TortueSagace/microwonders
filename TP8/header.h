#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int notsimd(char* filename, int width, int height);
int issimd(char* filename, int width, int height, int kernelSize);

char* contourExtraction(char* src,int width, int height);
