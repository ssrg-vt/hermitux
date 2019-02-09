#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/auxv.h>

int main(int argc, char *argv[])
{

	char *str = (char *)getauxval(AT_PLATFORM);
	printf("AT_PLATFORM: %s\n", str);

	str = (char *)getauxval(AT_NULL);
	printf("AT_NULL: %s\n", str);

	str = (char *)getauxval(AT_IGNORE);
	printf("AT_IGNORE: %s\n", str);

	str = (char *)getauxval(AT_EXECFD);
	printf("AT_EXECFD: %s\n", str);

	void *addr = (void *)getauxval(AT_PHDR);
	printf("AT_PHDR: %p\n", addr);

	int integer = (int)getauxval(AT_PHENT);
	printf("AT_PHENT: %d\n", integer);

	integer = (int)getauxval(AT_PHNUM);
	printf("AT_PHNUM: %d\n", integer);

	integer = (int)getauxval(AT_PAGESZ);
	printf("AT_PAGESZ: %d\n", integer);

	integer = (int)getauxval(AT_BASE);
	printf("AT_BASE: %d\n", integer);

	integer = (int)getauxval(AT_FLAGS);
	printf("AT_FLAGS: %d\n", integer);

	addr = (void *)getauxval(AT_ENTRY);
	printf("AT_ENTRY: %p\n", addr);

	integer = (int)getauxval(AT_NOTELF);
	printf("AT_NOTELF: %d\n", integer);

	integer = (int)getauxval(AT_UID);
	printf("AT_UID: %d\n", integer);

	integer = (int)getauxval(AT_EUID);
	printf("AT_EUID: %d\n", integer);

	integer = (int)getauxval(AT_GID);
	printf("AT_GID: %d\n", integer);

	integer = (int)getauxval(AT_EGID);
	printf("AT_EGID: %d\n", integer);

	integer = (int)getauxval(AT_CLKTCK);
	printf("AT_CLKTCK: %d\n", integer);

	addr = (void *)getauxval(AT_HWCAP);
	printf("AT_HWCAP: %p\n", addr);

	return 0;
}
