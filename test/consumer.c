#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char receive[50];
    
    while (1) {
        int res = syscall(__NR_dm510_msgbox_get, receive, 50);
        
        // Successfully recived a message.
        if (res >= 0) {
            printf("%s\n", receive);
            // Did we get an error or just no message?
        } else if (res != -ENOMSG) {
            printf("Failed to 'get' message. Error: %d", res);
            return -1;
        }
        
        // Don't use all the CPU.
        usleep(1000);
    }
    
    return 0;
}
