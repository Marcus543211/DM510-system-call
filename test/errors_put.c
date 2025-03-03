#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char *test = "Hello world!";
    char receive[50];
    
    // Error: Negative length
    int err1 = syscall(__NR_dm510_msgbox_put, test, -1);
    if (err1 != -EINVAL) { printf("Failure on negative length: %d\n", err1); }
    
    // Error: Null
    int err2 = syscall(__NR_dm510_msgbox_put, NULL, 10);
    if (err2 != -EFAULT) { printf("Failure on null: %d\n", err2); }
    
    // Error: Invalid address
    // Addresses 1 to 4095 are always invalid
    // https://stackoverflow.com/questions/8997044/
    int err3 = syscall(__NR_dm510_msgbox_put, 1000, 10);
    if (err3 != -EFAULT) { printf("Failure on invalid address: %d\n", err3); }
    
    // No message should be available
    int res = syscall(__NR_dm510_msgbox_get, receive, 50);
    if (res != -ENOMSG) { printf("Some invalid message was written\n"); }
    
    if (err1 && err2 && err3 && res == -ENOMSG) {
        printf("System handled errors successfully\n");
    }
    
    return 0;
}
