#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "header.h"

int main(void){
    notsimd("Quentin_512x512.raw\0", 512, 512);
    issimd("Quentin_512x512.raw\0", 512, 512);
    //int res = draft();
    return 0;
}
