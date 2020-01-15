#include "syscall.h"

void print(int c, int n){
	int i;
	for (i = 0; i < n; i++) {
		PutInt(c+i);
	}
	PutChar('\n');
}

int main(){
	//PutChar('h');
	//PutInt(1);
	print(1,4);
	//Halt();
	return 0;
}