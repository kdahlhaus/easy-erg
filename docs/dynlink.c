#include <windows.h>
#include <stdio.h>	

int main () {
	
	/*Typedef the hello function*/
	typedef int (*pfunc)();
	
	/*Windows handle*/
	HANDLE hdll;
	
	/*A pointer to a function*/
	pfunc hello;
	
	/*LoadLibrary*/
	hdll = LoadLibrary("RPPM3CSAFE.dll");
	
	/*GetProcAddress*/
	hello = (pfunc)GetProcAddress(hdll, 6);
	
	/*Call the function*/
	int i = hello();
    printf("%d", i);
	return 0;
}

