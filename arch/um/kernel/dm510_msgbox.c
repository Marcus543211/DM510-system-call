#include "dm510_msgbox.h"
#include <stdlib.h>
#include <string.h>
#include "linux/slab.h"
#include "linux/uaccess.h"

typedef struct _msg_t msg_t;

struct _msg_t{
  msg_t* previous;
  int length;
  char* message;
};

static msg_t *top = NULL;

int dm510_msgbox_put( char *buffer, int length ) {
  if (length < 0) { return -EINVAL; }
  if (!access_ok(buffer, length)) { return -EFAULT; }
  msg_t* msg = kmalloc(sizeof(msg_t));
  if (msg == NULL) { return -ENOMEM; } 
  msg->previous = NULL;
  msg->length = length;
  msg->message = kmalloc(length);
  if (msg->message == NULL) { kfree(msg); return -EFAULT; }
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

int dm510_msgbox_get( char* buffer, int length ) {
  if (length < 0) { return -EINVAL; }
  if (!access_ok(buffer, length)) { return -EFAULT; }
  if (top != NULL) {
    msg_t* msg = top;
    int mlength = msg->length;
    top = msg->previous;

    if (length < mlength) {
      return -EMSGSIZE;
    }

    /* copy message */
    copy_to_user(buffer, msg->message, mlength);

    /* free memory */
    kfree(msg->message);
    kfree(msg);

    return mlength;
  }
  return -ENOMSG;


}
