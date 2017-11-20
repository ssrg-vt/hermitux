# Capstan Example

This is a "hello world" C++ application that shows how to use Capstan to
package and run native applications on OSv.

## Prerequisites

You need to have [Capstan](https://github.com/cloudius-systems/capstan)
installed on your computer.

## Usage

To build and launch the example application under QEMU, type:

```
$ capstan run
```

This makes Capstan automatically pull a base image, invoke ``make`` to
build the software, build an image, and finally launch the application
under QEMU.

The ``run`` command will never overwrite the created image.  If you make
changes to the application, you need rebuild the image with:

```
$ capstan build
```

You can also launch the example application under VirtualBox with:

```
$ capstan run -p vbox
```
