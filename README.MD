# A small example practice on ext2 file system

Reading inode content and directory entry record from a ext2 image file.

## Compile

Simply run `make`.

## Create ext2 file system image file

The target execute file need to read a file name `initrd.img`. You can change the file name to any. You can create a file with `mke2fs`:

``` bash
dd if=/dev/zero of=initrd.img bs=300k count=1
mke2fs -F -m0 initrd.img
```

Then mount the file, do what ever you want in that mount point. Unmount, execute the target in debugger, then watch the results.

## Watch the results

Execute target won't output anythings. In order to see the result, you need to use debugger with breakpoints to watch the changes of the variable `d` and `content`.

## References

This code based on [OSdev's Ext2 article](https://wiki.osdev.org/Ext2).