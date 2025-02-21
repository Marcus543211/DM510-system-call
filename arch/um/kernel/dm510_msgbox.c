#include "linux/slab.h"
#include "linux/uaccess.h"

typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

static msg_t *top = NULL;

int sys_dm510_msgbox_put( char *buffer, int length ) {
  if (length < 0) { return -EINVAL; }
  if (!access_ok(buffer, length)) { return -EFAULT; }
  msg_t* msg = kmalloc(sizeof(msg_t), GFP_KERNEL);
  if (msg == NULL) { return -ENOMEM; } /* If we get this error we assume that it's because of no mem */
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length, GFP_KERNEL);
  if (msg->message == NULL) { kfree(msg); return -ENOMEM; } /* If we get this error we assume that it's because of no mem. We also free the mem we allocated in the first kmalloc */
  copy_from_user(msg->message, buffer, length);

  if (top == NULL) {
    top = msg;
  } else {
    /* not empty stack */
    msg->previous = top;
    top = msg;
  }
  return 0;
}

int sys_dm510_msgbox_get( char* buffer, int length ) {
  if (length < 0) { return -EINVAL; }
  if (!access_ok(buffer, length)) { return -EFAULT; }
  if (top == NULL) { return -ENOMSG; }
  msg_t* msg = top;
  top = msg->previous;
  int mlength = msg->length;
  if (length < mlength) { return -EMSGSIZE; }

  /* copy message */
  copy_to_user(buffer, msg->message, mlength);

  /* free memory */
  kfree(msg->message);
  kfree(msg);

  return mlength;
}
