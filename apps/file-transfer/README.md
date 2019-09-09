Usage:

1. Start the client:
```
make test
```

2. Create a file for transfer, put it in a separate directory:
```
mkdir folder
dd if=/dev/zero of=folder/file bs=1M count=1
```

3. Send the file:
```
./send_file folder/file 10.0.5.2
```
