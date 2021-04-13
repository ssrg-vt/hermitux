Minifs simple test. To make it work as a dynamically compiled application
(`make test-dyn`) the binary needs to be present on the minifs filesystem so
you need to edit `.minfs` and add this line:

```
prog-dyn;prog-dyn
```
