#include "copyright.h"
#include "thread.h"
#include "switch.h"
#include "synch.h"
#include "system.h"
#include "thread.h"


typedef struct{
    int f;
    int arg;
}ForkArgs;

void StartUserThread(int data) {

	ForkArgs *fargs = (ForkArgs *)data;
	
	//space = new AddrSpace (fargs->f);
    //currentThread->space = space;

    currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	//Need to write f the function in some register, maybe PC ?
	//machine->WriteRegister(4,fargs->arg);
	machine->WriteRegister(4,fargs->arg);
    machine->Run ();
}

int UserThreadCreate(int f, int arg) {

	 Thread *userthread = new Thread("User thread");
	 if (userthread == NULL){
	 	delete userthread;
	 	return -1;
	 }
	 	

	 ForkArgs *fargs = new ForkArgs;
	 fargs->f = f;
	 fargs->arg = arg;

	 userthread->Fork(StartUserThread,(int)fargs);
	 return 0;

}

int do_UserThreadExit(Thread *th) {
	//if(th->name() != "main"){
		th->Finish();
		return 0;
	//}
	//else
	//	return -1;
}

int UserThreadExit() {
	return do_UserThreadExit(currentThread);
}



//do_UserThreadExit

//void UserThread::UserThreadExit(char *n)