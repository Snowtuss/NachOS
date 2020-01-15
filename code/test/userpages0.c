#include "syscall.h"

void print(char *c) {
	PutString(c);
	UserThreadExit();
}


int main () {	

	void* f = print;
	int res;
	int res2;
    res = UserThreadCreate(f,"<h><e><l><l><o>\n");
    UserThreadJoin(1);
	res2 = UserThreadCreate(f,"<f><r><i><e><n><d>\n");
	UserThreadJoin(1);
	PutInt(res);
	PutInt(res2);
	Halt();

}