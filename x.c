#include <stdint.h>
#include <stdio.h>

int main(void)
{
	int num = 3645272;
	const char *key = (uintptr_t) num;

	printf("%d\n", (int) key);
	return 0;
}
