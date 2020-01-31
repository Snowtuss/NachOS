#include "syscall.h"

void print(int *i) {
	PutInt((int)*i);

	UserThreadExit();
}

void print2(char *c) {
	PutString(c);
	UserThreadExit();
}


int main () {	
	int i = 9;

	void * b = (void *) &i;
	void* f = print;
	void* f2 = print2;
	int res;
	int res2;
    
    res= UserThreadCreate(f,b);

    UserThreadJoin(res);
    PutChar('\n');

	res2= UserThreadCreate(f2,"Test of String Thread\n");
	UserThreadJoin(res2);

	res2= UserThreadCreate(f2,"Test of String Thread2\n");

	res2= UserThreadCreate(f2,"Test of String Thread3\n");

    
    PutInt(res2);

    Exit(0);

}