extern int errno;

int *__errno_location(void) __attribute__((weak));
int *__errno_location() { return &errno; }
int *__getreent(void);

int *__getreent(void) {
	return __errno_location();
}

