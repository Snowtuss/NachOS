// addrspace.cc 
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option 
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include <strings.h>  
#include "synch.h"


//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the 
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------
//Semaphore* lockEndMain;
static Semaphore *lockHalt;
static Semaphore *lockStack;
static Semaphore *lockThread[(UserStackSize/(PagePerThread*PageSize))];

BitMap *bitmapStack;


void ReadAtVirtual( OpenFile *executable, int virtualaddr,int numBytes, int position,
                            TranslationEntry *pageTable,unsigned numPages) {

    char temp[numBytes];
    int size;
    size = executable->ReadAt(temp, numBytes, position);
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
    int i;
    for(i=0;i<size;i++) {
        machine->WriteMem(virtualaddr+i, 1, temp[i]);
    }
}

static void
SwapHeader (NoffHeader * noffH)
{
    noffH->noffMagic = WordToHost (noffH->noffMagic);
    noffH->code.size = WordToHost (noffH->code.size);
    noffH->code.virtualAddr = WordToHost (noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost (noffH->code.inFileAddr);
    noffH->initData.size = WordToHost (noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost (noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost (noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost (noffH->uninitData.size);
    noffH->uninitData.virtualAddr =
  WordToHost (noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost (noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical 
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace (OpenFile * executable)
{
    NoffHeader noffH;
    unsigned int i, size;
    bitmapStack = new BitMap((int)(UserStackSize/(PagePerThread*PageSize)));
    
    bitmapStack->Mark(0);

    executable->ReadAt ((char *) &noffH, sizeof (noffH), 0);


    if ((noffH.noffMagic != NOFFMAGIC) &&
  (WordToHost (noffH.noffMagic) == NOFFMAGIC))
  SwapHeader (&noffH);
    ASSERT (noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize; // we need to increase the size
    // to leave room for the stack
    numPages = divRoundUp (size, PageSize);
    size = numPages * PageSize;

    ASSERT (numPages <= NumPhysPages);  // check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory

    DEBUG ('a', "Initializing address space, num pages %d, size %d\n",
     numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++)
      {
    pageTable[i].virtualPage = i; // for now, virtual page # = phys page #
    pageTable[i].physicalPage = fp->GetEmptyFrame(FALSE);
    //pageTable[i].physicalPage = i+1;
    pageTable[i].valid = TRUE;
    pageTable[i].use = FALSE;
    pageTable[i].dirty = FALSE;
    pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
    // a separate page, we could set its 
    // pages to be read-only
      }

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    //bzero (machine->mainMemory, size);

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0)
      {
    DEBUG ('a', "Initializing code segment, at 0x%x, size %d\n",
     noffH.code.virtualAddr, noffH.code.size);
    //executable->ReadAt (&(machine->mainMemory[noffH.code.virtualAddr]),noffH.code.size, noffH.code.inFileAddr);
     ReadAtVirtual(executable,noffH.code.virtualAddr,noffH.code.size,noffH.code.inFileAddr,pageTable,numPages);
      }
    if (noffH.initData.size > 0)
      {
    DEBUG ('a', "Initializing data segment, at 0x%x, size %d\n",
     noffH.initData.virtualAddr, noffH.initData.size);
   // executable->ReadAt (&(machine->mainMemory[noffH.initData.virtualAddr]),noffH.initData.size, noffH.initData.inFileAddr);
      ReadAtVirtual(executable,noffH.initData.virtualAddr,noffH.initData.size,noffH.initData.inFileAddr,pageTable,numPages);
      }
      
      lockHalt = new Semaphore("lock main's halt",0);
      for(i=0;i<(UserStackSize/(PagePerThread*PageSize));i++)
        lockThread[i] = new Semaphore("lock a user thread",0);
      //currentThread->space->LockHalt();
      lockStack  = new Semaphore("lock stack",1);
      nbThreadAccess = new Semaphore("read available", 1);
      
       

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
  // LB: Missing [] for delete
  // delete pageTable;
  delete [] pageTable;
  //delete fp;
  // End of modification
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters ()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
  machine->WriteRegister (i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister (PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister (NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister (StackReg, numPages * PageSize - 16);
    DEBUG ('a', "Initializing stack register to %d\n",
     numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//
//      For now, nothing!
//----------------------------------------------------------------------

void
AddrSpace::SaveState ()
{
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void
AddrSpace::RestoreState ()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}


//Fonction qui alloue et renvoie le premier byte libre dans la bitmap et le définit comme id de thread.

int AddrSpace::TidAllocator() {
  lockStack->P();
  
  if(bitmapStack->NumClear()<=0)
    return -1;
  int find = bitmapStack->Find();
  if(!bitmapStack->Test(find))
    return -1;
  if(find == 0 ) {
    bitmapStack->Mark(find);
    find = bitmapStack->Find();
  }
  else
    bitmapStack->Mark(find);
  lockStack->V();
  return find;
}

//Fonction qui renvoie l'addresse de la pile du thread
int AddrSpace::StackAddr() {

    return (numPages*PageSize - 16 - currentThread->GetIdThread()*PagePerThread*PageSize);
    
}

//Fonction qui nous permets de lock notre verrou de main
void AddrSpace::LockHalt(){
  lockHalt->P();
}

//Fonction qui nous permets d'unlock notre verrou de main
void AddrSpace::UnlockHalt(){
  lockHalt->V();
}

//Fonction qui nous permets de lock notre verrou de thread qu'on va utiliser dans les sections critiques
void AddrSpace::LockThread(int idThread){
  lockThread[idThread]->P();
}

//Fonction qui nous permets d'unlock notre verrou de main
void AddrSpace::UnlockThread(int idThread){
  lockThread[idThread]->V();
}

//Fonction qui désaloue un byte dans notre bitmap
void AddrSpace::FreeMapStack(){
  bitmapStack->Clear(currentThread->GetIdThread());
  nbThreadAccess->P();
  //On décrémente le nombre de threads vu qu'un vient de se finir
  currentThread->space->UpdateNbThreads(-1);
  nbThreadAccess->V();
}

//Fonction qui incrémente le nombre de threads de n
void AddrSpace::UpdateNbThreads(int n){
  nbThreads+=n;
}

//Fonction qui renvoie le nombre de threads
int AddrSpace::GetNbThreads(){
  return nbThreads;
}
