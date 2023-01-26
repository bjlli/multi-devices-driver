#!/bin/bash
echo "/dts-v1/;" > overlay-$1.dts
echo "/plugin/;" >> overlay-$1.dts
echo "/{" >> overlay-$1.dts
echo "	compatible = \"allwinner,sun8i-h3\";" >> overlay-$1.dts
echo "	fragment@0 {" >> overlay-$1.dts
echo "		target-path = \"/\";" >> overlay-$1.dts
echo "		__overlay__{" >> overlay-$1.dts
echo "			my_device_$1{" >> overlay-$1.dts
echo "				dev_num = <$1>;" >> overlay-$1.dts
echo "				compatible = \"emc-logic, pshBtns\";" >> overlay-$1.dts
echo "				status = \"okay\";" >> overlay-$1.dts
echo "				pBtn_label = \"rpi-gpio-$2\";" >> overlay-$1.dts
echo "				pBtn_gpio = <$2>;" >> overlay-$1.dts
echo "			};" >> overlay-$1.dts
echo "		};" >> overlay-$1.dts
echo "	};" >> overlay-$1.dts
echo "};" >> overlay-$1.dts
armbian-add-overlay overlay-$1.dts
