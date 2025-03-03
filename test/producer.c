#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../arch/x86/include/generated/uapi/asm/unistd_64.h"

int main(int argc, char **argv) {
    char buffer[50];
    
    for (int i = 0; ; i++) {
        // Generate a message to send.
        int len = sprintf(buffer, "Message %d", i);
        
        printf("Sending: %s\n", buffer);
        
        int err = syscall(__NR_dm510_msgbox_put, buffer, len+1);
        if (err) {
            printf("Failed to 'put' message. Error: %d", err);
            return -1;
        }
        
        // Don't just spam messages.
        sleep(1);
    }
    
    return 0;
}
