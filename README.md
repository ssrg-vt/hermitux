# Hermitux test environment

## Prerequisites
  - HermitCore prerequisites (https://github.com/RWTH-OS/HermitCore)
  - HermitCore toolchain installed in /opt/hermit (the one coming from the
  debian repositories mentionned in HermitCore GitHub repositories works fine)

## Steps

1. Install everything with the bootstrap script:

```bash
./bootstrap.sh
```

2. Test and example application, for example NPB IS:
```bash
cd apps/loader-npb/loader-npb-is
make test
```
