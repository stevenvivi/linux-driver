#include <stdio.h>
#include <stdlib.h>

int handler()
{
	printf("from: %s\n",__func__);
	return 0;
}
struct per{
	int a;
	int b;
	int (*handle)();
};

int main(int argc, char *argv[])
{
	struct per a = {
		.a = 7,
		//.handle = handler,
	};
	(*a.handle)();
	printf("a.a = %d, a.b = %d\n", a.a, a.b);
	return 0;
}
