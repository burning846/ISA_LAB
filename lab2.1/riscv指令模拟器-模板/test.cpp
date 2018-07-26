#include <stdio.h>

int main()
{
	int a = 0;
	int b = 1;
	int c[10] = {1,2,3,4,5,6,7,8,9,10};
	for(int i = 0; i < 10; ++i)
		a += c[i];
	b = a + b;
	return 0;
}