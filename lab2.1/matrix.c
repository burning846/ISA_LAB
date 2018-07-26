#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int* A(int *a, int *b, int m, int n, int p)
{
	int *c = malloc(m * p * sizeof(int));
	memset(c,0,sizeof(c));
	for(int i = 0; i < m; i++)
	{
		//printf("0\n");
		for(int j = 0; j < p; j++)
		{
			//printf("2\n");
			c[i * p + j] = 0;
			//printf("3\n");
			for(int k = 0; k < n; k++)
			{
				//printf("2\n");
				c[i*p+j] += a[i*n+k] * b[k*p+j];
			}
		}
	}
	//printf("1\n");
	return c;
}

int main()
{
	int m = 1000;
	int n = 1000;
	int p = 1000;
	int *a = malloc(m * n * sizeof(int));
	int *b = malloc(n * p * sizeof(int));
	memset(a,0,sizeof(a));
	memset(b,0,sizeof(b));
	int *c = A(a,b,m,n,p);
	return 0;
}