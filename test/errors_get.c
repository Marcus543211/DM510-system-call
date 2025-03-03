#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char *test = "Hello world!";
    char receive[50];

    // Write a message to test read.
    int err = syscall(__NR_dm510_msgbox_put, test, strlen(test)+1);
    if (err) { printf("Failed to write message.\n"); return -1; }

    // Error: Negative buffer length
    int res1 = syscall(__NR_dm510_msgbox_get, receive, -1);
    if (res1 != -EINVAL) { printf("Failed on negative length: %d\n", res1); }

    // Error: Null
    int res2 = syscall(__NR_dm510_msgbox_get, NULL, 10);
    if (res2 != -EFAULT) { printf("Failed on null: %d\n", res2); }

    // Error: Invalid address
    // Addresses 1 to 4095 are always invalid
    // https://stackoverflow.com/questions/8997044/
    int res3 = syscall(__NR_dm510_msgbox_get, 1000, 10);
    if (res3 != -EFAULT) { printf("Failed on invalid address: %d\n", res3); }

    // Error: Buffer too small (lying)
    int res4 = syscall(__NR_dm510_msgbox_get, receive, 1);
    if (res4 != -EMSGSIZE) { printf("Faild on too small buffer: %d\n", res4); }

    // No message should be available
    int res = syscall(__NR_dm510_msgbox_get, receive, 50);
    if (res != -ENOMSG) { printf("Message was not removed by small buffer.\n."); }

    if (res1 < 0 && res2 < 0 && res3 < 0 && res4 < 0 && res == -ENOMSG) {
        printf("System handled errors successfully.\n");
    }

    return 0;
}
