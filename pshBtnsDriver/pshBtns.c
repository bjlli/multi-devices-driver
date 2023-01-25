#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

/* Defines */

#define MAX_DEV_NUM 6

/* Global variables */

uint times_onProbe = 0;
uint times_onRemove = 0;

struct device_info{
    struct gpio_desc *pBtn;
    int pBtn_gpio;
    int dev_num;
    const char *pBtn_label;
    char buffer[15];
    uint number_presses;
    int irq_sensornum;
};

/* Probe/remove functions declarations */

static int gpio_init_probe(struct platform_device *pdev);
static int gpio_exit_remove(struct platform_device *pdev);

/* Show/store functions declarations */

static ssize_t show_pressNum( struct class *class, struct class_attribute *attr, char *buf );
static ssize_t store_pressNum( struct class *class, struct class_attribute *attr, const char *buf, size_t count );

/* Strcuts declarations*/

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

/* Show and store functions definitions */

static ssize_t show_pressNum( struct class *class, struct class_attribute *attr, char *buf ){
    int value = 0;
    printk("READ!");
    return sprintf(buf, "%d", value); 
}

static ssize_t store_pressNum( struct class *class, struct class_attribute *attr, const char *buf, size_t count ){
    
    printk("NOT ABLE TO WRITE!\n"); 
    return count;
}

/* Variables definitions */

static struct class *device_class = NULL;
struct class_attribute *class_attr = NULL;
struct device_info *pBtn_info = NULL;

/* Probe and remove functions definitions*/

static int gpio_init_probe(struct platform_device *pdev){

    printk("PROBE!\n");
    int ret;
    struct device *dev = &pdev->dev;
    static struct lock_class_key __key;
    
    if(times_onProbe == 0){
        /* class allocation */
        device_class = (struct class *)kmalloc(MAX_DEV_NUM*sizeof(struct class),GFP_ATOMIC);
        if(!device_class){
		    printk("Class allocation error");
	    }
        /* class attribute allocation */
	    class_attr = (struct class_attribute *)kmalloc(3*MAX_DEV_NUM*sizeof(struct class_attribute), GFP_ATOMIC);
	    if(!class_attr){
		    printk("Class attribute allocation error");
	    }
        /* device_info allocation */
        pBtn_info = (struct device_info *)kmalloc(MAX_DEV_NUM*sizeof(struct device_info), GFP_ATOMIC);
        if(!pBtn_info){
		    printk("device_info allocation error");
	    }
        printk("First time on probe!");
    }else{
        printk("Not first time on probe!");
    }

    sprintf((pBtn_info+times_onProbe)->buffer, "%s_%d", "push_button", times_onProbe);

    (device_class+times_onProbe)->name = (pBtn_info+times_onProbe)->buffer;
    (device_class+times_onProbe)->owner = THIS_MODULE;
    ret = __class_register((device_class+times_onProbe),&__key);

    (*(class_attr + times_onProbe)).show = show_pressNum;
    (*(class_attr + times_onProbe)).store = store_pressNum;
    (*(class_attr + times_onProbe)).attr.name = "pressNum";
    (*(class_attr + times_onProbe)).attr.mode = 0777 ;
    ret = class_create_file((device_class+times_onProbe), &(*(class_attr+times_onProbe)));

    times_onProbe = times_onProbe + 1;

    return 0;

}

static int gpio_exit_remove(struct platform_device *pdev){

    if(times_onRemove == 0){
	    printk("First time on remove");    
    	class_unregister(device_class);
    	class_destroy(device_class);
    	kfree(class_attr);
	    kfree(pBtn_info);
    }else{
    	printk("Not the first time on remove");
	    class_unregister((device_class+times_onRemove));
	    class_destroy((device_class+times_onRemove));
    }
    times_onRemove = times_onRemove + 1;
    
    printk("REMOVE!\n");

    return 0;

}

/* Module */

MODULE_LICENSE("GPL");
module_platform_driver(pshBtns_driver);
