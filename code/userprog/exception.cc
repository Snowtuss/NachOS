// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchconsole.h"
#include "synch.h"
#include "userthread.h"

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
//static SynchConsole *sync;
static Semaphore *readAvail = new Semaphore("read available", 0);

static Semaphore *writeDone = new Semaphore("write done", 0);
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------
/*
void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);

    if ((which == SyscallException) && (type == SC_Halt))
      {
    DEBUG ('a', "Shutdown, initiated by user program.\n");
    interrupt->Halt ();
      }
    else
      {
    printf ("Unexpected user mode exception %d %d\n", which, type);
    ASSERT (FALSE);
      }

    // LB: Do not forget to increment the pc before returning!
    UpdatePC ();
    // End of addition
}
*/

void
ExceptionHandler(ExceptionType which)
{

  int type = machine->ReadRegister(2);
  readAvail = new Semaphore ("read avail", 0);
  //sync = new SynchConsole(NULL,NULL);
  /*if ((which == SyscallException) && (type == SC_Halt)) {
      DEBUG(’a’, "Shutdown, initiated by user program.\n");
      interrupt->Halt();
  } else {
      printf("Unexpected user mode exception %d %d\n", which, type);
      ASSERT(FALSE);
  }*/
  if (which == SyscallException) {
    switch (type) {
    case SC_Halt: {
      
      currentThread->space->nbThreadAccess->P();
      
      while(currentThread->space->GetNbThreads()>0) //printf("hey");
      {
        currentThread->space->nbThreadAccess->V();
         currentThread->space->LockHalt();
         currentThread->space->nbThreadAccess->P();

      }
           
          //printf("\nhey------------------\n");
        interrupt->Halt();
      break;
    }
    case SC_Exit: {

      currentThread->space->nbThreadAccess->P();
    
      while(currentThread->space->GetNbThreads()>0) //printf("hey");
      {
        currentThread->space->nbThreadAccess->V();
        currentThread->space->LockHalt();
        currentThread->space->nbThreadAccess->P();
      }
      
      UpdateNbProcess(-1);
      delete currentThread->space;
        if(GetNbProcess()>=0){
          currentThread->Finish();
        }
        else{
          interrupt->Halt ();
        }
        break;
    }
    case SC_PutChar: {
      readAvail->V();
      //printf("%c\n",(char)machine->ReadRegister(4));
      synchconsole->SynchPutChar(machine->ReadRegister(4));
      readAvail->P();
      break;
    }
    case SC_PutString: {
      //char str[MAX_STRING_SIZE];
      //printf("i got a SC_PutString call\n");
      //readAvail->V();
      char *str = (char *) malloc(MAX_STRING_SIZE*sizeof(char));
      synchconsole->copyStringFromMachine(machine->ReadRegister(4),str,MAX_STRING_SIZE);
      synchconsole->SynchPutString(str);
      free(str);
      //readAvail->P();
      //printf("finished the SC_PutString call\n");
      break;
    }
    case SC_GetChar: {
      writeDone->V();
      machine->WriteRegister(2,(int)synchconsole->SynchGetChar());
      writeDone->P();
      break;
    }
    /*case SC_GetString: {
      printf("i got a SC_GetString call\n");
      char *str = (char *) malloc(MAX_STRING_SIZE*sizeof(char));
      synchconsole->SynchGetString(str, MAX_STRING_SIZE);
      int i=0;

      while(str[i] != '\0' && i<MAX_STRING_SIZE){
       // writeDone->V();
        machine->WriteRegister(2,(int)str[i]);
        //writeDone->P();
        i++;
      }
      
      break;
    }
    */
    case SC_GetString: {
      char *str = (char *) malloc(MAX_STRING_SIZE*sizeof(char));
      synchconsole->SynchGetString(str, MAX_STRING_SIZE);
      //copyStringToMachine(str,machine->ReadRegister(4),machine->ReadRegister(5));
      //synchconsole->copyStringFromMachine(str,machine->ReadRegister(4),machine->ReadRegister(5));
      int i=0;
      //int value;
      while(str[i] != '\0' && i<MAX_STRING_SIZE-1){
       // writeDone->V();
        //value = str[i];
        //machine->WriteRegister(2,(int)str[i]);
        machine->WriteMem(machine->ReadRegister(4)+i,1,str[i]);
        //writeDone->P();
        i++;
      }
      //value='\0';
      //machine->WriteMem(machine->ReadRegister(4)+i,1,value);
      free(str);
      break;
    }
    case SC_PutInt: {
      //readAvail->V();
      //printf("%c\n",(char)machine->ReadRegister(4));
      synchconsole->SynchPutInt(machine->ReadRegister(4));
      //readAvail->P();
      break;
    }
    case SC_GetInt: {
      //writeDone->V();
     // machine->WriteRegister(4,(int)synchconsole->SynchGetInt());
      //writeDone->P();
       writeDone->V();
      machine->WriteRegister(2,(int)synchconsole->SynchGetInt());
      writeDone->P();
      break;
    }
    case SC_UserThreadCreate: {
      int returnValue = do_UserThreadCreate(machine->ReadRegister(4),machine->ReadRegister(5));
      machine->WriteRegister(2,returnValue);
      break;
    }
    case SC_UserThreadExit: {
      do_UserThreadExit();
      break;
    }
    case SC_UserThreadJoin: {
      do_UserThreadJoin(machine->ReadRegister(4));
      break;
    }
    case SC_ForkExec: {
      char *str = (char *) malloc(MAX_STRING_SIZE*sizeof(char));
      synchconsole->copyStringFromMachine(machine->ReadRegister(4),str,MAX_STRING_SIZE);
      printf("OUR FILE IS  : %s\n",str);
      machine->WriteRegister(2,do_ForkExec(str));
      break;
    }
    default: {
      printf("Unexpected user mode exception %d %d\n", which, type);
      ASSERT(FALSE);
      break;
    }
    }
    UpdatePC();
  }
}