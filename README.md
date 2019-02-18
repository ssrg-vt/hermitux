# Hermitux test environment

## Prerequisites
  - Recommended system: Debian 9 (GlibC support is not assured on newer 
    distributions)
  - `build-essential` debian package, plus [HermitCore prerequisites](https://github.com/RWTH-OS/HermitCore#requirements):

```
sudo apt update
sudo apt install build-essential cmake nasm apt-transport-https wget
```

  - [HermitCore toolchain](https://github.com/RWTH-OS/HermitCore#hermitcore-cross-toolchain) installed in /opt/hermit (the one coming from the
  debian repositories mentionned in HermitCore GitHub repositories works fine, you might need to install the `apt-transport-https` debian package before downloading the toolchain packages):
```
echo "deb [trusted=yes] https://dl.bintray.com/hermitcore/ubuntu bionic main" | sudo tee -a /etc/apt/sources.list
sudo apt update
sudo apt install binutils-hermit newlib-hermit pte-hermit gcc-hermit libomp-hermit libhermit
```
    - You may also need to install a recent version of libmpfr to use the hermit toolchain on debian 9:https://www.mpfr.org/mpfr-current/
  - For fortran test application, you will need the `gfortran` debian package
  - Clang/LLVM to test this compiler, we recommend the following version to also test the obfuscation options: https://github.com/obfuscator-llvm/obfuscator

TODO here: put prerequisites for syscall rewriting and identification (cmake
with curl support)

## Steps

1. Install everything with the bootstrap script:

```bash
./bootstrap.sh
```

2. Test an example application, for example NPB IS:
```bash
cd apps/loader-npb/loader-npb-is
make test
```

## Template Makefile
TODO describe here

## `hermit-run`
TODO describe here

## Networking

In order to enable network for the unikernel you need to create a _tap_
interface. Moreover, to get access to the unikernel from outside the host, you
need to bridge that interface with the physical interface of the host.

To that aim, use the following commands:

```bash
# Create the bridge br0
sudo ip link add br0 type bridge

# Add the physical interface to the bridge (change the physical interface
# name enp0s31f6 to the correct one on your machine)
sudo ip link set enp0s31f6 master br0

# At that point you will loose internet connection on the host, to get it back:
sudo dhclient br0

# Create the tap interface
sudo ip tuntap add tap100 mode tap

# Set the ip addr for the tap interface on your LAN (it will be the ip address
# of the unikernel, here I use 192.168.1.4
sudo ip addr add 192.168.1.4 broadcast 192.168.1.255 dev tap100

# Enable proxy ARP for the tap interface
sudo bash -c 'echo 1 > /proc/sys/net/ipv4/conf/tap100/proxy_arp'

# Enable the tap interface
sudo ip link set dev tap100 up

# Add it to the bridge
sudo ip link set tap100 master br0

# Next you can launch the unikernel, you need to correctly set the
# network-related environment variables:
HERMIT_NETIF=tap100 HERMIT_IP=192.168.1.4 HERMIT_GATEWAY=192.168.1.1

```

## Features

- Debugging: TODO describe here
- Profiling: TODO describe here
- secure container: TODO describe here
- Checkpoint/restart: TODO describe here
- ASLR: see apps/loader-pie

## Syscall binary rewriting
You need to switch the hermitux branch to `dc-syscalls-pierre` and the
hermitux-kernel branch to `dc-syscalls-pierre` too. Then look at the file
`hermitux/syscall_rewriter/readme`
