#include <stdio.h>
#include <string.h>

#define N 100

void getNext(char *T, int lenT, int *next)
{
	int i,j;
    	next[1] = 0;
    	j = 0;
    	for(i = 2; i <= lenT; i++)
    	{
        	while(j > 0 && T[j+1] != T[i])
        	j = next[j];
        	if(T[j+1] == T[i])
            	j++;
        	next[i] = j;
        }
}
int KMP(char *S, int lenS, int pos, char *T, int lenT, int *next)
{
    int i,j;
    j = 0;
    for(i = pos; i < lenS; i++)
    {
        while(j > 0 && T[j+1] != S[i])
            j = next[j];
        if(T[j+1] == S[i])
            j++;
        if(j == lenT)
        {
            return i-j+1;
            //用于多次匹配：j = next[j];
            //printf("Pattern occurs at %d.", i-j+1);
            //j = next[j];
        }
    }
    return -1;
}

int main()
{
    int x;
    char *A = "abababaababacbaaaababacb";
    char *B = "ababacb";
    char BB[N];
    int next[N];
    int i;
   
    memset(BB, 0, sizeof(BB));
    BB[0] = '0';
    for (i=0; i<strlen(B); i++)
    {
        BB[i+1] = B[i];
    }
   
    printf("%s %s\n", A, BB);
   
    getNext(BB, strlen(B), next);
    i = KMP(A, strlen(A), 0, BB, strlen(B), next);
    printf("%d\n", i);
   
    getchar();
    return 0;
}









void Getnext(char *t, int *next)
{
	int i, j, len;
	i = 0;
	j = -1;
	next[0] = -1;
	while(t[i] != '\0')
	{
		if(j == -1 || t[i] == t[j])
		{
			i++;
			j++;
			next[i] = j;
		}
		else
		{
			j = next[j];
		}
	}
}
