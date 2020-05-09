#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <asm/mach/arch.h>
#include <linux/phy.h> 
#include <linux/gpio.h>
//#include <mach/gpio.h>
//#include <plat/gpio-cfg.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/gpio.h>

#define DEVICE_NAME "myirexp"
//open
int myirexp_misc_dev_open(struct inode *inode, struct file *filp)
{
	printk("misc dev open!\n");
	return 0 ;
}
//close
int myirexp_misc_dev_close(struct inode *inode, struct file *filp)
{
	printk("misc dev close!\n");
	return 0 ;
}

static void __init myirexp_board_init(void)
{
    static struct device_node *np = NULL, *nplte = NULL;
    int gpio,gpio1;
    enum of_gpio_flags flags;
    
	//for dm9061 
     np = of_find_compatible_node(NULL,NULL,"myir,dm9061");
     if(NULL != np)
     {
        gpio = of_get_named_gpio_flags(np, "phy1-gpios", 0, &flags);
         if(!gpio_is_valid(gpio)){
              printk("the phy1 gpio failed\n");
                     
        }else{    
             if(gpio_request(gpio, "phy1-gpio"))
             {
                  printk("can not request phy1 gpio\n");
                 gpio_free(gpio);
             }else{
                  gpio_direction_output(gpio, 0);
                 msleep(100);
                 gpio_direction_output(gpio, 1);
                 }
 
           }
    }

	//for 4g lte
 	nplte = of_find_compatible_node(NULL,NULL,"myir,lte_pwrseq");
     	if(NULL != nplte)
     	{
        	gpio1 = of_get_named_gpio_flags(nplte, "lte-gpios", 0, &flags);
         	if(!gpio_is_valid(gpio1)){
               		printk("the lte gpio failed\n");     
        	}else{    
             		if(gpio_request(gpio1, "lte-gpio"))
             		{
                  		printk("can not request lte gpio\n");
                 		gpio_free(gpio1);
             		}else{
                 		gpio_direction_output(gpio1, 0);
                 		msleep(100);
                 		gpio_direction_output(gpio1, 1);
                 }
 
           }
    }
    
    
}

struct file_operations myirexp_file_ops = {
	.owner = THIS_MODULE ,
	.open = myirexp_misc_dev_open,
	.release = myirexp_misc_dev_close,
};
struct miscdevice myirexp_misc_dev = {
	.minor = MISC_DYNAMIC_MINOR ,
	.name = DEVICE_NAME ,
	.fops = &myirexp_file_ops,	
};


static int __init myirexp_misc_dev_init(void) 
{
	int ret_error ;
	int ret = misc_register(&myirexp_misc_dev);
	if(ret != 0){
	   ret_error = ret ;
	   printk("misc register fair!\n");
	   goto fair ;
	}
	printk("myirexp init\n");
    	myirexp_board_init();

	return ret ;
fair:
	return ret_error ;
}

static void __exit myirexp_misc_dev_exit(void) 
{
	misc_deregister(&myirexp_misc_dev);
}

module_init(myirexp_misc_dev_init);
module_exit(myirexp_misc_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("licy add misc driver");
