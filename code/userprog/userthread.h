#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "copyright.h"
#include "utility.h"
#include "machine.h"
#include "addrspace.h"

int UserThreadCreate(int f, int *arg);
void StartUserThread(int data);
int UserThreadExit();
int do_UserThreadExit(Thread *th);


#endif

