#include "linux/kernel.h"
#include "linux/unistd.h"

asmlinkage
int sys_hello_kernel( int flag ) {
	printk("Your kernel greets you %d times! \n", flag);
	return 0;
}
