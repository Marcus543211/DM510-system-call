#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char *test = "Hello world!";
    char recive[50];

    syscall(__NR_dm510_msgbox_put, test, strlen(test)+1);

    syscall(__NR_dm510_msgbox_get, recive, 50);

    print("%s\n", recive);

    return 0;
}
