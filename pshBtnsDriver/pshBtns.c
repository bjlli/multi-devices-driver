#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Defines */

#define MAX_DEV_NUM 6

/* Global variables */

int first_time_onProbe = 0;
int device_num = 0;
int times_onRemove = 0;

struct device_info{
    int pBtn_gpio;
    int dev_num;
    const char *pBtn_label;
    char buffer[15];
    uint numOf_presses;
    int irq_num;
};

/* Functions declarations */

static int gpio_init_probe(struct platform_device *pdev);
static int gpio_exit_remove(struct platform_device *pdev);
static ssize_t show_pressNum( struct class *class, struct class_attribute *attr, char *buf );
static ssize_t store_pressNum( struct class *class, struct class_attribute *attr, const char *buf, size_t count );
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

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

/* Variables definitions */

static struct class *device_class = NULL;
struct class_attribute *class_attr = NULL;
struct device_info *pBtn_info = NULL;


/* Show and store functions definitions */

static ssize_t show_pressNum( struct class *class, struct class_attribute *attr, char *buf ){
    uint value = 0;
    int num = 0;

    num = (int)(*class).name[12];                                                                                  \    
    num = num - 48;                                                                                                  \               
    printk("Push button number: %d, READ!", num);
    value = (pBtn_info+num)->numOf_presses;

    return sprintf(buf, "%d", value); 
}

static ssize_t store_pressNum( struct class *class, struct class_attribute *attr, const char *buf, size_t count ){
    
    printk("NOT ABLE TO WRITE!\n"); 
    return count;
}

/* Interrupt callback */

static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    printk(KERN_ALERT "Interrupt was triggered!\n");
	printk("Irq is:%d", irq); 
    int irq_count = 0;
    for(irq_count=0; irq_count<=device_num;irq_count++){

        if((pBtn_info+irq_count)->irq_num == irq){
            break;
        }
    }

    printk("The push button %d was pressed", (pBtn_info+irq_count)->dev_num);
    printk("GPIO number %d", (pBtn_info+irq_count)->pBtn_gpio);
    (pBtn_info+irq_count)->numOf_presses = (pBtn_info+irq_count)->numOf_presses + 1;

    return (irq_handler_t) IRQ_HANDLED; 
}

/* Probe and remove functions definitions*/

static int gpio_init_probe(struct platform_device *pdev){

    printk("Probe!\n");
    int ret;
    struct device *dev = &pdev->dev;
    static struct lock_class_key __key;
    
    if(first_time_onProbe == 0){
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
        first_time_onProbe = 1;
        printk("First time on probe!");
    }else{
        printk("Not first time on probe!");
    }

    /* Overlay variables */
    ret = device_property_read_u32(dev,"dev_num",&device_num);
    (pBtn_info+device_num)->dev_num = device_num;
    ret = device_property_read_string(dev,"pBtn_label",&(pBtn_info+device_num)->pBtn_label);
    ret = device_property_read_u32(dev,"pBtn_gpio",&(pBtn_info+device_num)->pBtn_gpio);
    sprintf((pBtn_info+device_num)->buffer, "%s_%d", "push_button", device_num);
	printk("Device number is: %d", device_num);
    printk("GPIO pin is: %d", (pBtn_info+device_num)->pBtn_gpio);

    /* Class */
    (device_class+device_num)->name = (pBtn_info+device_num)->buffer;
    (device_class+device_num)->owner = THIS_MODULE;
    ret = __class_register((device_class+device_num),&__key);

    /* Class attributes */
    (*(class_attr + device_num)).show = show_pressNum;
    (*(class_attr + device_num)).store = store_pressNum;
    (*(class_attr + device_num)).attr.name = "pressNum";
    (*(class_attr + device_num)).attr.mode = 0777 ;
    ret = class_create_file((device_class+device_num), &(*(class_attr+device_num)));

    /* allocating push button GPIO*/
    if(gpio_request((pBtn_info+device_num)->pBtn_gpio, (pBtn_info+device_num)->pBtn_label)){
		printk("Error!\n");
		return -1;
	}
    /* push button GPIO defined as input */
    if(gpio_direction_input((pBtn_info+device_num)->pBtn_gpio)) {
		printk("Error!\n");
		gpio_free((pBtn_info+device_num)->pBtn_gpio);
		return -1;
	}
 
	/* Saving the IRQ number */
    (pBtn_info+device_num)->irq_num = gpio_to_irq((pBtn_info+device_num)->pBtn_gpio);
    printk("IRQ num:%d", (pBtn_info+device_num)->irq_num);
    /*  IRQ */
	if(request_irq((pBtn_info+device_num)->irq_num, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_FALLING, "my_gpio_irq", NULL) != 0){
		printk("Interrupt error!\n: %d\n", (pBtn_info+device_num)->irq_num);
		gpio_free((pBtn_info+device_num)->pBtn_gpio);
		return -1;
	}
	    if(gpio_set_debounce((pBtn_info+device_num)->pBtn_gpio,100)){
        printk("Debounce\n");
    }


    return 0;

}

static int gpio_exit_remove(struct platform_device *pdev){

    if(times_onRemove == 0){
	 printk("First time on remove");    
    	class_unregister(device_class);
    	class_destroy(device_class);
    	kfree(class_attr);
	kfree(pBtn_info);
        free_irq(pBtn_info->irq_num,NULL);
        gpio_free(pBtn_info->pBtn_gpio);
    }else{
    	printk("Not the first time on remove");
	class_unregister((device_class+times_onRemove));
	class_destroy((device_class+times_onRemove));
        free_irq((pBtn_info+times_onRemove)->irq_num,NULL);
        gpio_free((pBtn_info+times_onRemove)->pBtn_gpio);
    }
    times_onRemove = times_onRemove + 1;
    
    printk("Remove!\n");

    return 0;

}

/* Module */

MODULE_LICENSE("GPL");
module_platform_driver(pshBtns_driver);
