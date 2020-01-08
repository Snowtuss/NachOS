#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console(readFile,writeFile,ReadAvail,WriteDone,0);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	console->PutChar(ch);
	writeDone->P();
}

char SynchConsole::SynchGetChar()
{
	readAvail->P ();
	return console->GetChar();
}

void SynchConsole::SynchPutString(const char s[])
{
	int i=0;
	while(s[i]!='\0') {
		SynchPutChar(s[i]);
		i++;
	}
	
}

void SynchConsole::SynchGetString(char *s,int n)
{
	int i;
	for(i=0;i<n;i++)
		s[i] = SynchGetChar();
}




void SynchConsole::copyStringFromMachine( int from, char *to, unsigned size) {
	int i=0;
	int *value = (int *) malloc(size);
	for(i=0;i<(int)size-1;i++){
		machine->ReadMem(from+i,sizeof(int),value);
		to[i] = (char) value[0];
	}
	to[size] = '\0';
}