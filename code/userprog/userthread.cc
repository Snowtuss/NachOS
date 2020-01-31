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

static Semaphore *await = new Semaphore("Wait",1);
bool firstThread = true;
void StartUserThread(int data) {

	ForkArgs *fargs = (ForkArgs *)data;
	
	//space = new AddrSpace (fargs->f);
    //currentThread->space = space;
    /*if(firstThread) {
    	firstThread = false;
    	 currentThread->space->InitRegisters();
    }*/
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
	printf("\n-----------USER THREAD  NUMBER %d IS RUNNING.-----------\n",currentThread->GetIdThread());
	//currentThread->Yield();
	//await->V();
    machine->Run();
    //printf("\nUser Thread is doing alright !\n");
    
}

int do_UserThreadCreate(int f, int arg) {
	
	 Thread *userthread = new Thread("User thread");
	 //await->P();
	 if (userthread == NULL){
	 	delete userthread;
	 	return -1;
	 }
	 currentThread->space->nbThreadAccess->P();
	 currentThread->space->UpdateNbThreads(1);
	 currentThread->space->nbThreadAccess->V();
	 //currentThread->space->userexitaddr = machine->ReadRegister(6);
	 //if(currentThread->GetIdThread() == 0)
         //currentThread->space->LockHalt();

	 ForkArgs *fargs = new ForkArgs;
	 fargs->f = f;
	 fargs->arg = arg;
	 int tid = userthread->space->TidAllocator();
	  if (tid == -1){
	 	delete userthread;
	 	return -1;
	 }
	 userthread->SetIdThread(tid);
	 //printf("machine->nbThreads at creation = %d\n",machine->nbThreads);
	 userthread->Fork(StartUserThread,(int)fargs);
	 //await->V();
	 //currentThread->Yield();
	 return tid;

}

void do_UserThreadExit() {
	
	
	

	//currentThread->space->UpdateNbThreads(-1);
	//currentThread->space->FreeStack(currentThread->GetIdThread());
	//if(machine->nbThreads==0)
	currentThread->space->UnlockHalt();
	
	currentThread->space->UnlockThread(currentThread->GetIdThread());
	//printf("machine->nbThreads at EXIT = %d",machine->nbThreads);
	printf("\n-----------USER THREAD NUMBER %d EXIT.-----------\n",currentThread->GetIdThread());
	currentThread->space->FreeMapStack();
	currentThread->Finish ();
	
	
	


	
}


void do_UserThreadJoin(int idThread){
	currentThread->space->LockThread(idThread);
}


void StartForkExec(int data){
	//char *str;
	//str = (char*)data;
	//OpenFile * executable = fileSystem->Open(str);
	OpenFile * executable = (OpenFile *) data;
	AddrSpace *space = new AddrSpace(executable);
	//machine->nbThreads+=1;
	//ForkExecArgs *args = (ForkExecArgs *)data;
	currentThread->space = space;
	//currentThread->SetIdThread(99);
	currentThread->space->InitRegisters ();
    currentThread->space->RestoreState ();
    //currentThread->Yield();
    machine->Run ();		
}

int do_ForkExec(char *s){
	printf("ENTERING FORKEXEC FUNCTION\n");
	//char str[MAX_STRING_SIZE];
	//synchconsole->copyStringFromMachine(addresse,str,MAX_STRING_SIZE);
	if(s == NULL)
		return -1;
	OpenFile * executable = fileSystem->Open(s);
	//printf("%s",s);

	//AddrSpace *space = new AddrSpace(executable);
	Thread *ForkedThread = new Thread("ForkExec Thread");
	
	if(ForkedThread == NULL || executable == NULL ){
		return -1;
	}
	
	//ForkExecArgs *args = new ForkExecArgs;
	//args->space = space;
	//ForkedThread->SetIdThread(99);
	ForkedThread->Fork(StartForkExec,(int)executable);
	UpdateNbProcess(1);
	//currentThread->space->nbThreads+=1;
	//ForkedThread->SetIdThread(99);
	printf("\n-----------FORK EXEC , PROC NUMBER  %d.-----------\n",GetNbProcess());
	//currentThread->Yield();
	//printf("FORK EXEC machine->nbThreads = %d\n",currentThread->space->nbThreads);
	return 0;
}




//do_UserThreadExit

//void UserThread::UserThreadExit(char *n)