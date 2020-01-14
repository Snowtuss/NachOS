#include "copyright.h"
#include "thread.h"
#include "switch.h"
#include "synch.h"
#include "system.h"
#include "userthread.h"
#include "bitmap.h"
#include "addrspace.h"


typedef struct{
    int f;
    int arg;
}ForkArgs;

void StartUserThread(int data) {

	ForkArgs *fargs = (ForkArgs *)data;
	
	//space = new AddrSpace (fargs->f);
    //currentThread->space = space;
	machine->nbThreads+=1;
    currentThread->space->InitRegisters();

	currentThread->space->RestoreState();
	//Need to write f the function in some register, but which one ?
	//machine->WriteRegister(4,fargs->arg);
	machine->WriteRegister(4,fargs->arg);
	machine->WriteRegister(PCReg,fargs->f);
	//fargs->f+=4;
	machine->WriteRegister(NextPCReg,fargs->f+4);
	//write the stack
	machine->WriteRegister(StackReg,currentThread->space->StackAddr());
	//machine->WriteRegister(RetAddrReg, currentThread->space->userexitaddr);
	
	//currentThread->Yield();
	
    machine->Run();
    printf("\nUser Thread is doing alright !\n");
    
}

int UserThreadCreate(int f, int arg) {
	
	 Thread *userthread = new Thread("User thread");
	 if (userthread == NULL){
	 	delete userthread;
	 	return -1;
	 }
	 //currentThread->space->userexitaddr = machine->ReadRegister(6);

	 ForkArgs *fargs = new ForkArgs;
	 fargs->f = f;
	 fargs->arg = arg;
	 
	 userthread->Fork(StartUserThread,(int)fargs);
	 //currentThread->Yield();
	 return 0;

}

void do_UserThreadExit() {
	//if(th->name() != "main"){
		//currentThread->Finish();
		//printf("User Thread Finished successfuly");
	//	return 0;
	//}
	//else
	//	return -1;
	//if(currentThread->GetIdThread() == 0)
	//	return -1
	//else{

	//fin du thread
	printf("\nExiting User thread.\n");
	currentThread->space->FreeMapStack();
	currentThread->space->LockHalt();
	currentThread->Finish ();
	machine->nbThreads-=1;


	
}





//do_UserThreadExit

//void UserThread::UserThreadExit(char *n)