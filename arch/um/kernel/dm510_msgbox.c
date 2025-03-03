#include "linux/slab.h"
#include "linux/uaccess.h"

typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

// The stack is initialized as empty.
static msg_t *top = NULL;

int sys_dm510_msgbox_put( char *buffer, int length ) {
  // Gives an invalid argument error if the message length is invalid.
  if (length < 0) { return -EINVAL; }
  // Gives a bad address error if the buffer isn't safe to write to.
  if (!access_ok(buffer, length)) { return -EFAULT; }

  // Memory is allocated in kernel space instead of user space.
  msg_t* msg = kmalloc(sizeof(msg_t), GFP_KERNEL); 
  // If we get this error we assume that it's because we are out of memory.
  if (msg == NULL) { return -ENOMEM; } 
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length, GFP_KERNEL);

  // Free the memory we already allocated with kmalloc if we fail to allocate
  // memory for the message. Return with an out of memory error.
  if (msg->message == NULL) { kfree(msg); return -ENOMEM; }
  // Copy the message from user to kernel space and give a bad address error on failure.
  if (copy_from_user(msg->message, buffer, length)) { return -EFAULT; }

  // Disable interrupts.
  unsigned long flags;
  local_irq_save(flags);

  // Check if stack is empty.
  if (top == NULL) {
    top = msg;
  } else {
    msg->previous = top;
    top = msg;
  }

  // Enable interrupts.
  local_irq_restore(flags);

  return 0;
}

int sys_dm510_msgbox_get( char* buffer, int length ) {
  // Give an invalid argument error if the message length is invalid.
  if (length < 0) { return -EINVAL; }
  // Give a bad address error if the buffer isn't safe to write to.
  if (!access_ok(buffer, length)) { return -EFAULT; }

  // Disable interrupts.
  unsigned long flags;
  local_irq_save(flags);

  // Give a no message error if the stack is empty.
  if (top == NULL) {
    // Reenable interrupts before exiting.
    local_irq_restore(flags);
    return -ENOMSG;
  }
  // Take the top message from the stack.
  msg_t* msg = top;
  top = msg->previous;

  // Enable interrupts.
  local_irq_restore(flags);

  int mlength = msg->length;
  // Give a message size error if the message length is greater than the buffer length.
  // Alternatively, we could copy as much as possible.
  if (length < mlength) { return -EMSGSIZE; }

  // Copy the message from kernel to user space and give a bad address error on failure.
  if (copy_to_user(buffer, msg->message, mlength)) { return -EFAULT; }

  // Free memory.
  kfree(msg->message);
  kfree(msg);

  return mlength;
}
