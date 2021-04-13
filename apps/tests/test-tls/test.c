#include <stdio.h>

__thread int x = 12;
__thread int y;

int main(int argc, char *argv[])
{
	printf("X (tdata) = %d\n", x);
	printf("Y (tbss) = %d\n", y);
	return 0;
}
