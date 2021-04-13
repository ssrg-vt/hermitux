This example exercises one of the debugging features of HermiTux: the user code
voluntarily dereferences a null pointer. Upon crashing tHe hypervisor should
give the problematic loc:

```
$ make test
hi!
GUEST PAGE FAULT @0x0 (RIP @0x400276)
0x0000000000400276
/home/pierre/Desktop/hermitux/apps/loader-pagefault/prog.c:9
Makefile:67: recipe for target 'test' failed
make: *** [test] Error 242
```
