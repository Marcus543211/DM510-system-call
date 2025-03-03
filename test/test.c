#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char *test = "Hello world!";
    int length = strlen(test) + 1;
    char receive[50];
    
    // Send a message.
    int err = syscall(__NR_dm510_msgbox_put, test, length);
    if (err) {
        printf("Failed to 'put' message. Error: %d\n", err);
        return -1;
    }
    
    // Retrive it.
    int res = syscall(__NR_dm510_msgbox_get, receive, 50);
    if (res < 0) {
        printf("Failed to 'get' message. Error: %d\n", res);
        return -2;
    }
    
    // Check if the lengths match up.
    if (res != length) {
        printf("Failed to read entire message. Read %d of %d\n", res, length);
        return -3;
    }
    
    printf("Successfully read: %s\n", receive);
    
    return 0;
}
