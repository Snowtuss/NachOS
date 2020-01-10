#include "syscall.h"

void affiche(int i) {
	PutInt(i);
	UserThreadExit();
}


int main () {	
	int i = 2;
	void* f = affiche;
    int res = UserThreadCreate(f,&i);
    PutInt(6);

    return res;

}