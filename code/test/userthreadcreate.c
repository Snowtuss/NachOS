#include "syscall.h"

void print(int i) {
	PutInt(i);
	UserThreadExit();
}


int main () {	
	int i = 2;
	void* f = print;
    int res = UserThreadCreate(f,&i);
    //PutString("Return value of Fork : ");
    PutInt(res);

    return res;

}