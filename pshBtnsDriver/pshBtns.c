#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

/* Driver Functions */

static int gpio_init_probe(struct platform_device *pdev);
static int gpio_exit_remove(struct platform_device *pdev);

/* Global Structs */

static struct of_device_id pshBtns_ids[] = {
    {.compatible = "emc-logic, pshBtns"},
    {/* end node */}
};

static struct platform_driver pshBtns_driver = {
    .probe = gpio_init_probe,
    .remove = gpio_exit_remove,
    .driver = {
        .name = "pshBtns_driver",
        .owner = THIS_MODULE,
        .of_match_table = pshBtns_ids,
    }
};

/* Probe and remove functions */

static int gpio_init_probe(struct platform_device *pdev){

    printk("PROBE!\n");

    return 0;

}

static int gpio_exit_remove(struct platform_device *pdev){
    
    printk("REMOVE!\n");

    return 0;

}

MODULE_LICENSE("GPL");
module_platform_driver(pshBtns_driver);
