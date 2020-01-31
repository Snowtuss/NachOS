#include "syscall.h"
#define MAX_STRING_SIZE 100

int main(){
	char str[MAX_STRING_SIZE];
	GetString(str,MAX_STRING_SIZE);
	//str[1]='h';
	PutString(str);
	PutChar('\n');
	//PutChar('a');
	Exit(0);
}