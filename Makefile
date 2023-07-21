CFLAGS += -g
CFLAGS += -O0

.PHONY: all

all: test
	./test

test: ext2.o test.o
clean:
	rm *.o test
# initrd.img:
# 	dd if=/dev/zero of=initrd.img bs=300k count=1
# 	mke2fs -F -m0 initrd.img
