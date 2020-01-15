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
	int i = 9;

	void * b = (void *) &i;
	void* f = print;
	void* f2 = print2;
	int res;
	int res2;
    //while(1)
    res= UserThreadCreate(f,b);

    PutInt(res);
    UserThreadJoin(1);
	res2= UserThreadCreate(f2,"Test of String Thread\n");
	//UserThreadJoin(2);
	UserThreadJoin(1);
	res2= UserThreadCreate(f2,"Test of String Thread2\n");
	UserThreadJoin(1);
	//UserThreadJoin(3);
	res2= UserThreadCreate(f2,"Test of String Thread3\n");
	UserThreadJoin(1);
	//UserThreadJoin(4);
    //PutString("Return value of Fork : ");
    
    PutInt(res2);
    //PutInt(i);
    Halt();

}