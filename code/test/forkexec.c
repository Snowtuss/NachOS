#include "syscall.h"

int main(){
	int returnFork0,returnFork1;
	returnFork0 = ForkExec("../test/userpages0");
	returnFork1 = ForkExec("../test/userpages1");
	PutInt(returnFork0);
	PutChar('\n');
	PutInt(returnFork1);
	return 0;
}

