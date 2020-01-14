#include "syscall.h"

void print(int *i) {
	PutInt((int)*i);
	//PutString("USERTHREAD FUCNTION\n");
	UserThreadExit();
}

void print2(char *c) {
	PutString(c);
	UserThreadExit();
}


int main () {	
	//int i = 9;
	//void * b = (void *) &i;
	void* f = print2;
	int res;
    while(1)
    //res= UserThreadCreate(f,b);
	res= UserThreadCreate(f,"Test of String Thread\n");
    //PutString("Return value of Fork : ");
    PutInt(res);
    //PutInt(i);
    Halt();

}