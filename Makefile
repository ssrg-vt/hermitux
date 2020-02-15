KERNEL=hermitux-kernel/prefix/x86_64-hermit/extra/tests/hermitux
MUSL=musl/prefix/lib/libc.a

ifeq (, $(shell which /opt/hermit/bin/x86_64-hermit-gcc))
$(error "Hermit toolchain not found, please follow these instructions: \
https://github.com/hermitcore/libhermit#hermitcore-cross-toolchain")
endif


all: $(KERNEL) $(MUSL) $(LOMP)

submodules: hermitux-kernel/ musl/ libiomp/
	git submodule init
	git submodule update

.PHONY: $(KERNEL)
$(KERNEL): hermitux-kernel/build/
	make -C hermitux-kernel/build install

hermitux-kernel/build/: hermitux-kernel/
	cd hermitux-kernel && git submodule init && git submodule update && \
		mkdir -p build/ && mkdir -p prefix

hermitux-kernel/: submodules

.PHONY: $(MUSL)
$(MUSL): musl/prefix/
	make -C musl install

musl/prefix: submodules
	cd musl && mkdir -p prefix && ./configure --prefix=$(PWD)/musl/prefix

.PHONY: $(LOMP)
$(LOMP): libiomp/build/
	make -C libiomp/build 

libiomp/build: submodules
	cd libiomp && mkdir -p build && cd build && \
		cmake -DLIBOMP_ENABLE_SHARED=OFF ..

clean:
	make -C hermitux-kernel/build clean
	make -C musl clean
	make -C libiomp/build clean
