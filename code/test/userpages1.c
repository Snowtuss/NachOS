#include "syscall.h"


void print(char *c) {
	int i;
	for(i=0;i<4;i++)
	PutString(c);
	UserThreadExit();
}

int main () {	


	void* f2 = print;

	int res2;
	res2= UserThreadCreate(f2,"cccccc");

	//UserThreadJoin(res2);
    //print("bbb");
    PutInt(res2);
    
    Exit(0);

}