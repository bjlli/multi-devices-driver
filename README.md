# multi-devices-driver
Linux Kernel driver module that can be used by multiple identical devices. This code shows an example for driver module that runs in a Orange Pi (armbian 22.05) and that is used by multiple circuits with push buttons. To run this module in any embedded dispositive you should modify the device-tree overlay files. 

# Driver directories and files:
The driver directories are located at /sys/class/ path and their names have the pattern: push_button_(DEV_NUM), being DEV_NUM the push button number defined on its overlay (starting always on 0). Inside all these directories you can find a file called pressNum: you won't be able to write on it, but you'll be able to read the the number of times that the current push button was pressed.

# To generate a new overlay for Orange Pi (armbian 22.05):

./overlayGen GPIO_PIN

You can check Orange Pi GPIO pins [here](https://allwincnc.github.io/wiring.html).

# To insert/remove module into kernel

Insert armbian 22.05 (you should generate all overlay files before that):

make

sudo insmod pshBtns.ko

Remove armbian 22.05:

sudo rmmod pshBtns

