# multi-devices-driver
Linux Kernel driver module that can be used by multiple devices.

# To generate a new overlay for Orange Pi (armbian 22.05):
./overlayGen GPIO_PIN
you can check Orange Pi GPIO pins <a href=“https://allwincnc.github.io/wiring.html“>here</a>

# To insert/remove module into kernel
Insert in amrbian 22.05:
make
sudo insmod pshBtns.ko
Remove on amrbian 22.05:
sudo rmmod pshBtns
