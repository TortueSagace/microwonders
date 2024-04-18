
#include "header.h"

int main() {
    notsimd("Quentin_512x512.raw\0", 512, 512);
    issimd("Quentin_512x512.raw\0", 512, 512, 3); // 3 for the kernel size
    return 0;
}
