#!/bin/bash

declare -i i=0

while [[ $i -le 5 ]]
do
    if [[ -f "overlay-$i.dts" ]]
    then
        echo "File overlay-$i.dts exists"
    else
        echo "File  overlay-$i.dts does not exist"

        echo "/dts-v1/;" > overlay-$i.dts
        echo "/plugin/;" >> overlay-$i.dts
        echo "/{" >> overlay-$i.dts
        echo "	compatible = \"allwinner,sun8i-h3\";" >> overlay-$i.dts
        echo "	fragment@0 {" >> overlay-$i.dts
        echo "		target-path = \"/\";" >> overlay-$i.dts
        echo "		__overlay__{" >> overlay-$i.dts
        echo "			my_device_$i{" >> overlay-$i.dts
        echo "				dev_num = <$i>;" >> overlay-$i.dts
        echo "				compatible = \"emc-logic, pshBtns\";" >> overlay-$i.dts
        echo "				status = \"okay\";" >> overlay-$i.dts
        echo "				pBtn_label = \"rpi-gpio-$1\";" >> overlay-$i.dts
        echo "				pBtn_gpio = <$1>;" >> overlay-$i.dts
        echo "			};" >> overlay-$i.dts
        echo "		};" >> overlay-$i.dts
        echo "	};" >> overlay-$i.dts
        echo "};" >> overlay-$i.dts

        armbian-add-overlay
        sudo reboot -h now
        
        break
    fi
    ((i++))
done

