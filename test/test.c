#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char *test = "Hello world!";
    char receive[50];

    syscall(__NR_dm510_msgbox_put, test, strlen(test)+1);

    syscall(__NR_dm510_msgbox_get, receive, 50);

    printf("%s\n", receive);

    return 0;
}
