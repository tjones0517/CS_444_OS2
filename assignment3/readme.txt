INSTRUCTIONS FOR MOUNTING DEVICE DRIVER:

The device driver, tjmc_a3, needs to be loaded as module into the VM.

In order to get it into the VM, you must scp it in from the host.

To scp the .ko (module) into the host, you must run the following qemu command:

qemu-system-386 -gdb tcp::5531 -S -nographic -kernel linux-yocto-3.19/arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext4 -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttys0 debug" -redir tcp:3122:22

Note that the -redir flag was added with a new tcp port 3122 to redirect to 22 to allow the scp of the file. Once in the vm, from the host type the following scp command.

$ scp -P 3122 tjmc_a3.ko root@localhost:/home/root

This will place the .ko file within the vm in the root directory, check to make sure it is there.

Once the .ko is in the vm do the following to get it working.

1) insmod ~/tjmc_a3.ko (This step inserts the file as a module)
2) mkfs.ext2 /dev/sbd0 (This will give an unpartitioned table error, this is okay and expected)
3) mount -t ext2 /dev/sbd0 /TJMountPoint/ (This mounts the file system onto the drive)

Once these steps are done, simply echoing text to the mount point should be sufficient for testing.

Example:

# echo 'testtext' > /TJMountPoint/testfile (This will send text to the mounted file system, and should then be grabbed by our driver tjmc_a3.ko, and sent to /dev/sbd0 since that is where we created the file system)

In order to test encryption, I recommend we use printk statements that will display the unencrypted text, and the encrypted text to ensure they are different. These printk statements should just be added into tjmc_a3.ko in the proper functions. It is difficult to get any sensible output from /dev/sbd0 since it is stored as unsigned variables rather than the text that was echoed to it. Printk statements should do the trick and be sufficient evidence

