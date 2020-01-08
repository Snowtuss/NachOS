#include "syscall.h"

void print(char *c){
	PutString(c);
}

int main(){
	print("Hello \127 World\n");
	Halt();
}