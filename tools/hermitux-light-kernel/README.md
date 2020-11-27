#Hermitux light

To compile, edit the Makefile and fill in the following variables:
  - `HERMITUX_BASE`: the hermitux install folder
  - `HERMIT_TOOLCHAIN_INSTALL`: the HermitCore prefix (generally `/opt/hermit`)

Note that the kernel must be compiled with LWIP for hermitux-light to be able 
to be built as LWIP requires newlib, and hermitux-light explicitely disables
newlib. In order to do so, when invoking cmake to build the kernel you need
to add this flag: `-DNO_NET=1`.

Type `make` to compile hermitux-light. To launch an application with that 
kernel, edit the template Makefile and set the `KERNEL` variable to point to
`hermitux-light.gz`.
