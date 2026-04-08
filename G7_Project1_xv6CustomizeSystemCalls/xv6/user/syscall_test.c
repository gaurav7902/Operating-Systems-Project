#include "kernel/types.h"
#include "user/user.h"

int main(){
	printf("Testing syscall logger\n");
	open("README",0);
	write(1,"hello\n",6);
	exit(0);
}
