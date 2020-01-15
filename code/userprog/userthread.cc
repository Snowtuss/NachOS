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
	
	ASSERT(fargs!=NULL);
	ASSERT(fargs->f!=0);
	ASSERT(fargs->arg!=0);
	//fargs->f+=4;
	machine->WriteRegister(4,(int)fargs->arg);
	machine->WriteRegister(NextPCReg,fargs->f+4);
	machine->WriteRegister(PCReg,(int)fargs->f);
	//write the stack
	machine->WriteRegister(StackReg,(int)currentThread->space->StackAddr());
	//printf("\n*************machine->nbThreads at StartUserThread = %d for Thread id %d*************\n",machine->nbThreads,currentThread->GetIdThread());
	//machine->WriteRegister(RetAddrReg, currentThread->space->userexitaddr);
	
	//currentThread->Yield();
	
    machine->Run();
    //printf("\nUser Thread is doing alright !\n");
    
}

int do_UserThreadCreate(int f, int arg) {
	
	 Thread *userthread = new Thread("User thread");
	 if (userthread == NULL){
	 	delete userthread;
	 	return -1;
	 }
	 //currentThread->space->userexitaddr = machine->ReadRegister(6);
	 //if(currentThread->GetIdThread() == 0)
         //currentThread->space->LockHalt();

	 ForkArgs *fargs = new ForkArgs;
	 fargs->f = f;
	 fargs->arg = arg;
	 //printf("machine->nbThreads at creation = %d\n",machine->nbThreads);
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
	//printf("\n-----------Exiting User thread of thread %d.-----------\n", currentThread->GetIdThread());
	currentThread->space->FreeMapStack();
	
	//if(machine->nbThreads==0)
	currentThread->space->UnlockHalt();
	machine->nbThreads-=1;
	//printf("machine->nbThreads at EXIT = %d",machine->nbThreads);
	currentThread->Finish ();
	
	


	
}





//do_UserThreadExit

//void UserThread::UserThreadExit(char *n)