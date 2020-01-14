#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *putSem;

static Semaphore *writeChar;
static Semaphore *readChar;
static Semaphore *writeString;
static Semaphore *readString;

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile)
{
	readAvail = new Semaphore("read avail", 0);
	writeDone = new Semaphore("write done", 0);
	putSem = new Semaphore("write done", 0);
	putSem->V();



	writeChar = new Semaphore("write char",1);
	readChar = new Semaphore("read char",1);

	writeString = new Semaphore("write string",1);
	readString = new Semaphore("read string",1);

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
	writeChar->P();

	console->PutChar(ch);
	writeDone->P();

	writeChar->V();
}

int SynchConsole::SynchGetChar()
{
	readChar->P();

	readAvail->P ();
	return console->GetChar();

	readChar->V();
}

void SynchConsole::SynchPutString(const char s[])
{
	writeString->P();
	int i=0;
	while(s[i]!='\0') {
		SynchPutChar(s[i]);
		i++;
	}
	writeString->V();
	//copyStringFromMachine(machine->ReadRegister(4),(char*)s,MAX_STRING_SIZE);
}

void SynchConsole::SynchGetString(char *s,int n)
{
	//SynchPutString("entering GetString\n");
	readString->P();
	char c;
	int i=0;
	for(i=0;i<n-1;i++){
		
		c = SynchGetChar();
		//readAvail->V ();
		if(c == '\n' || c == EOF)
			break;
		//SynchPutString("entering for loop\n");
		s[i]=c;
	}
	//SynchPutString("GOing out from GetString\n");

	s[i]='\0';
	readString->V();
	//writeDone->P();
	//readAvail->P ();
	//s[0]=0;
}

void SynchConsole::SynchPutInt(int n)
{
	char *s = (char*) malloc(sizeof(char)*MAX_STRING_SIZE); 
  	snprintf( s, MAX_STRING_SIZE, "%d",n);
  	SynchPutString(s);
	//writeDone->P();
}

int SynchConsole::SynchGetInt()
{
	putSem->P ();
	char *s = (char*) malloc(sizeof(char)*MAX_STRING_SIZE); 
	int value;
	SynchGetString(s,MAX_STRING_SIZE);
  	sscanf(s,"%d",&value);
  	//machine->WriteMem(machine->ReadRegister(4),1,value);
	return value;
	//machine->WriteMem(machine->ReadRegister(4),1,&value);
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