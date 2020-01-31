#include "copyright.h"
#include "thread.h"
#include "switch.h"
#include "synch.h"
#include "system.h"
#include "userthread.h"
#include "bitmap.h"
#include "addrspace.h"

//Structure pour transporter nos data

typedef struct{
    int f; // instructions (fonction)
    int arg; // Argument de la fonction
}ForkArgs;


void StartUserThread(int data) {

	ForkArgs *fargs = (ForkArgs *)data;
	//On initialise les registres
    currentThread->space->InitRegisters();
    //On restaure l'état
	currentThread->space->RestoreState();

	//On vérifie bien que notre structure,l'instruction et l'argument ont des valeurs non nulles
	ASSERT(fargs!=NULL);
	ASSERT(fargs->f!=0);
	ASSERT(fargs->arg!=0);

	//On écrit dans le registre 4 notre argument
	machine->WriteRegister(4,(int)fargs->arg);

	//On met dans le registre suivant de PC l'instruction incrémenter 4 fois
	machine->WriteRegister(NextPCReg,fargs->f+4);

	//On met dans le registre de PC l'instruction
	machine->WriteRegister(PCReg,(int)fargs->f);
	
	//On mets dans le registre de la pile l'addresse de la pile calculé par la fonction StackAddr()
	machine->WriteRegister(StackReg,(int)currentThread->space->StackAddr());

	//On lance la machine
    machine->Run();

    
}

//Fonction qui crée un thread utilisateur et le fork avec l'instruction f et l'argument arg et retourne le id du thread utilisateur créer
int do_UserThreadCreate(int f, int arg) {
	
	//On crée un nouveau thread
	 Thread *userthread = new Thread("User thread");

	 // On vérifie bien s'il est null ou pas avant de le manipuler 
	 if (userthread == NULL){
	 	delete userthread;
	 	return -1;
	 }

	 //Vérrou de thread vu qu'on est dans une section critique
	 currentThread->space->nbThreadAccess->P();
	 //On incrémente le nombre de threads
	 currentThread->space->UpdateNbThreads(1);
	 currentThread->space->nbThreadAccess->V();

	 ForkArgs *fargs = new ForkArgs;
	 fargs->f = f;
	 fargs->arg = arg;

	 //On récupure l'id du thread
	 int tid = userthread->space->TidAllocator();
	  if (tid == -1){
	 	delete userthread;
	 	return -1;
	 }


	 userthread->SetIdThread(tid);

	 //On fork notre thread
	 userthread->Fork(StartUserThread,(int)fargs);
	 //On donne le relais 
	 currentThread->Yield();

	 return tid;

}

//Fonction qui permet de bien finir le thread
void do_UserThreadExit() {

	//On lache le vérrou du HALT vu que le thread vient de finir son travail
	currentThread->space->UnlockHalt();
	
	//On lache le vérrou du THREAD (utile pour le join)
	currentThread->space->UnlockThread(currentThread->GetIdThread());


	currentThread->space->FreeMapStack();

	//On termine le thread
	currentThread->Finish ();

}

//Fonction qui permet d'attendre la finition d'un thread avant de continuer le programme
void do_UserThreadJoin(int idThread){
	//On prends le vérrou du Thread auquelle l'id est passé en paramétre 
	currentThread->space->LockThread(idThread);
}


void StartForkExec(int data){
	
	OpenFile * executable = (OpenFile *) data;
	AddrSpace *space = new AddrSpace(executable);
	
	currentThread->space = space;
	
	currentThread->space->InitRegisters ();
    currentThread->space->RestoreState ();
    
    machine->Run ();		
}

//Fonction qui crée un processus avec le nom de fichier passé en paramétre 
int do_ForkExec(char *s){
	
	if(s == NULL)
		return -1;
	
	OpenFile * executable = fileSystem->Open(s);

	Thread *ForkedThread = new Thread("ForkExec Thread");
	
	if(ForkedThread == NULL || executable == NULL ){
		return -1;
	}
	
	ForkedThread->Fork(StartForkExec,(int)executable);
	UpdateNbProcess(1);

	return 0;
}
