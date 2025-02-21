#include "linux/slab.h"
#include "linux/uaccess.h"

typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

/* The stack is initialized as empty */
static msg_t *top = NULL;

int sys_dm510_msgbox_put( char *buffer, int length ) {
  /* Gives an invalid argument error if the message length is invalid */
  if (length < 0) { return -EINVAL; }
  /* Gives a bad address error if the buffer isn't safe to write to */
  if (!access_ok(buffer, length)) { return -EFAULT; }

  msg_t* msg = kmalloc(sizeof(msg_t), GFP_KERNEL); /* Memory is allocated in kernel space instead of user space */
  if (msg == NULL) { return -ENOMEM; } /* If we get this error we assume that it's because we are out of memory */
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length, GFP_KERNEL);

  /* Free the memory we allocated in the first kmalloc if we fail to allocate memory for the message and give an out of memory error */
  if (msg->message == NULL) { kfree(msg); return -ENOMEM; } /* If we get this error we assume that it's because of no mem. We also free the mem we allocated in the first kmalloc */
  /* Copies the message from the user space to the kernel space and gives a bad address error if it fails */
  if (copy_from_user(msg->message, buffer, length)) { return -EFAULT; }

  /* Disables interrupts */
  unsigned long flags;
  local_irq_save(flags);

  if (top == NULL) {
    top = msg;
  } else {
    /* not empty stack */
    msg->previous = top;
    top = msg;
  }

  /* Enables interrupts */
  local_irq_restore(flags);

  return 0;
}

int sys_dm510_msgbox_get( char* buffer, int length ) {
  /* Gives an invalid argument error if the message length is invalid */
  if (length < 0) { return -EINVAL; }
  /* Gives a bad address error if the buffer isn't safe to write to */
  if (!access_ok(buffer, length)) { return -EFAULT; }
  /* Gives a no message error if the stack is empty */
  if (top == NULL) { return -ENOMSG; }

  /* Disables interrupts */
  unsigned long flags;
  local_irq_save(flags);

  msg_t* msg = top;
  top = msg->previous;

  /* Enables interrupts */
  local_irq_restore(flags);

  int mlength = msg->length;
  /* Gives a message size error if the message length is greater than the buffer length */
  if (length < mlength) { return -EMSGSIZE; } /* We could also have an approach where we copy as much as possible */

  /* Copies the message from the kernel space to the user space and gives a bad address error if it fails */
  if (copy_to_user(buffer, msg->message, mlength)) { return -EFAULT; }

  /* free memory */
  kfree(msg->message);
  kfree(msg);

  return mlength;
}
