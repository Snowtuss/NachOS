#include "syscall.h"

int main(){
	PutChar((char)GetChar());
	PutChar('\n');
	Halt();
}