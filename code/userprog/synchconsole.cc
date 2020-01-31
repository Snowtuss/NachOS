#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"
#include "console.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *SemPut;
static Semaphore *SemGetChar;
static Semaphore *SemGetString;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	console = new Console (readFile, writeFile, ReadAvail, WriteDone, 0);

	SemPut = new Semaphore("Put", 1);
	SemGetChar = new Semaphore("GetChar", 1);
	SemGetString = new Semaphore("GetString", 1);
}

SynchConsole::~SynchConsole()
{
	delete console;
	delete writeDone;
	delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch)
{
	SemPut->P();
	console->PutChar (ch);
	writeDone->P ();	// wait for write to finish
	SemPut->V();
}

char SynchConsole::SynchGetChar()
{
	SemGetChar->P();
	char ch;
	readAvail->P ();	// wait for character to arrive
	ch = console->GetChar ();
	SemGetChar->V();
	return ch;
}

void SynchConsole::SynchPutString(const char s[])
{
	SemPut->P();
	int i;
	for (i=0;i<MAX_STRING_SIZE && s[i]!='\0';i++){
		console->PutChar ((char)s[i]);
		writeDone->P ();	// wait for write to finish
	}
	SemPut->V();
}
/*
void SynchConsole::SynchGetString(char *s, int n)
{
	SemGetString->P();
	char c;
	int i;
	for (i=0;i<n;i++){
		c = synchconsole->SynchGetChar ();
		if(c==EOF)
			break;
		else
			s[i] = c;
	}
	s[i]='\0';
	SemGetString->V();
}*/


void SynchConsole::SynchGetString(char *s, int n)
{
	SemGetString->P();
	char c;
	int i;

	c = synchconsole->SynchGetChar ();
	if(c==EOF || c=='\n'){
		s[0]='\0';
		SemGetString->V();
		return;
	}
	else
		s[0] = c;
	for (i=1;i<n;i++){
		c = synchconsole->SynchGetChar ();
		if(c==EOF && s[i-1]=='\n')
			break;
		else{
			if(c==EOF)
				i--;
			else
				s[i] = c;
		}
	}
	s[i-1]='\0';
	SemGetString->V();
}


void SynchConsole::SynchPutInt(int n)
{
	char *s = new char(MAX_STRING_SIZE);
	snprintf(s,MAX_STRING_SIZE,"%d",n);
	synchconsole->SynchPutString(s);
}

int SynchConsole::SynchGetInt()
{
	char *s = (char*) malloc(sizeof(char)*MAX_STRING_SIZE); 
	int value;
	SynchGetString(s,MAX_STRING_SIZE);
  	sscanf(s,"%d",&value);
  	//machine->WriteMem(machine->ReadRegister(4),1,value);
	return value;
}

void SynchConsole::copyStringFromMachine( int from, char *to, unsigned size) {
	int i=0;
	int value;
	for(i=0;i<(int)size-1;i++){
		machine->ReadMem(from+i,1,&value);
		to[i] = (char) value;
	}
	to[size] = '\0';
}
