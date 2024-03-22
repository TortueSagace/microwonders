#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "issimd.h"
#include "draft.h"
#include "notsimd.h"

int main(void){
    notsimd("Quentin_512x512.raw\0", 512, 512);
    issimd("Quentin_512x512.raw\0", 512, 512);
    return 0;
}
