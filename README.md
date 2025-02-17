# Project 1; System call

### Ideas for the report
- How we could do it in actual linux instead of UML

### Message passing
2 system calles giving acces to a message box
- Get
- Put
- As stack
- Removed after read

In adreess space (usser space)

### Files we need
#### Header file
```
#ifndef __DM510_MSGBOX_H__
#define __DM510_MSGBOX_H__

int sys_dm510_msgbox_put(char*, int);
int sys_dm510_msgbox_get(char*, int);

#endif
```

Change so it will be a system call

#### Implementation
```
Define type message:
  previous
  length
  message (pointer to first char)

Gobal pointer to the top message (set to NULL)

Put(buffer, length):
  Get space for the message unit
  message -> prevoius = top
  set length
  Get space for the message text
  Copy message text into message unit

  top = message (include an explaination of why we can boil dwon the original code)

  Retrun something

Get(buffer, length)
  If something on the stack:
    take the top message
    make previous one top

    if buffer too small:
      error

    copy the message to the buffer
    free memmory

  return something
```

#### 1 or more testing files

### Problems
For cuntion include linux/slab.h and linux/uaccess.h

#### Memmory allocation
Done with kmalloc and kfree 
Do they work kinda the same?
Max size is page size
Look at the flags (probaly just do kernel)

#### Adreess translation
Use function to figure out where an adress is in kernel space
Have marcus say the thing about the border guy(i don't get it)

#### Checking and errors
Length should be >= 0
Check if it's okay to store in the buffer
Maybe also preblem in put for bogus adressess
Make sure to check if the functions have problems 
We have to return an error code
negative value of code
Check the files

#### Cncurrency
interrupts can fuck with more proccesses running
Tempararlity didable interrupts
Only arround the critical stuff
Look into the flags
Maybe it's written weird for cuncurency reasons
Make sure the message is complete before being read

#### Possible errors to handle
Error num, Error descriptor

#### No message on stack when getting:
- 74, Not a data message
- 42, No message of desired type
- 61, No data available

#### kmalloc returned NULL:
- 12, out of memory
- 28, no space left on device

#### Message too long for buffer:
- 90, Message too long
- 105, No buffer space available

#### Bad parameter:
- 22, Invalid argument

#### Bad address:
- 14, bad address
