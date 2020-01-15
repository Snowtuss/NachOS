#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "copyright.h"
#include "utility.h"
#include "machine.h"
#include "addrspace.h"
#include "bitmap.h"

int do_UserThreadCreate(int f, int arg);
void StartUserThread(int data);
void do_UserThreadExit();
int stackAddr();


#endif

