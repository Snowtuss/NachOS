#include "syscall.h"

int main(){
	//int returnFork0,returnFork1;
	//ForkExec("../build/userpages0");
	//ForkExec("../build/userthreadcreate");
	ForkExec("../build/userpages0");
	//ForkExec("../build/userthreadcreate");
	//returnFork1 = ForkExec("../build/userthreadcreate");
	//PutInt(returnFork0);
	//PutChar('\n');
	//PutInt(returnFork1);
	Exit(0);
}

