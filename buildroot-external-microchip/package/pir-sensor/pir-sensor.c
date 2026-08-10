#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define GPIO_PIR 85

unsigned int irq_number;
dev_t pir_dev ;
static struct class *sensor_class;
static struct device *pir_sensor; 
static struct cdev pir_cdev;
int motion_detected = 0;


static int __init pir_sensor_init(void);
static void __exit pir_sensor_exit(void);
static int pir_open(struct inode *inode, struct file *file);
static int pir_release(struct inode *inode, struct file *file);
static ssize_t pir_read(struct file *filp, char __user *buf, size_t len,loff_t * off);


//IRQ Handler for GPIO PC21
static irqreturn_t gpio22_irq_handler(int irq, void *arg){
	printk(KERN_INFO "Interrupt Occurred : PC21 \n");
//    printk(KERN_INFO "Interrupt GPIO22 Value = %d\n" , gpio_get_value(GPIO_PIR));
	motion_detected = 1;
	return IRQ_HANDLED;
}


static struct file_operations fops = 
{
	.owner = THIS_MODULE,
	.read = pir_read,
	.open = pir_open,
	.release = pir_release,
};

static int pir_open(struct inode *inode, struct file *file){
	return 0;
}

static int pir_release(struct inode *inode, struct file *file){
	return 0;
}

static ssize_t pir_read(struct file *filp, char __user *buf,size_t len,loff_t * off){

	char msg[] = "Motion detected\n";
	
	if(!motion_detected){
		return 0;
	}
	
	if(copy_to_user(buf,msg,strlen(msg)))
		return -EFAULT;
	
	motion_detected = 0;
		
	return strlen(msg);
}



/*INIT FUNCTION*/
static int __init pir_sensor_init(void) {
  //GPIO 17 Configuration
  if(gpio_request(GPIO_PIR,"GPIO_22") < 0){
  	printk(KERN_ERR "ERROR: GPIO 22 REQUEST\n");
  }
  
  gpio_direction_input(GPIO_PIR);

  //IRQ Request for PC21
  irq_number = gpio_to_irq(GPIO_PIR);
  printk(KERN_INFO "IRQ number assigned for PC21 is %d\n",irq_number);

  //IRQ Handler registration for GPIO17 on the detetction of Falling edge
  if(request_irq(irq_number, gpio22_irq_handler, IRQF_TRIGGER_RISING,"pir_sensor",NULL)){
	  	printk(KERN_INFO "pir_sensor:Cannot register IRQ\n");
	  	return -1;
  }

 //Dynamic Allocation of Major and Minor number for PIR sensor 
  if(alloc_chrdev_region(&pir_dev,0,1,"PIR_SENSOR") < 0){
	  	printk(KERN_INFO "Cannot Allocate major and minor number for PIR sensor\n");
	  	return -1;
  }
  printk(KERN_INFO "Major number assigned for PIR SENSOR is %d\n",MAJOR(pir_dev));
  printk(KERN_INFO "Minor number assigned for PIR SENSOR is %d\n",MINOR(pir_dev));
  
  //Creating cdev structure
  cdev_init(&pir_cdev,&fops);
  
  if(cdev_add(&pir_cdev,pir_dev,1)<0){
	  printk(KERN_INFO "Cannot add device to the system\n");
	  unregister_chrdev_region(pir_dev,1);
	  return -1 ;  
  }
  
  //Class creation for Motion Sensor
  sensor_class = class_create("Motion_sensor");
  if(IS_ERR(sensor_class)){
	  	printk(KERN_ERR "Failed to create struct class for Motion_sensor\n");
	  	unregister_chrdev_region(pir_dev,1);
	  	return -1;
  }

 printk(KERN_INFO "Class for Motion_sensor created");
  
  //Device File creation for PIR sensor
  pir_sensor = device_create(sensor_class,NULL,pir_dev,0,"pir");
  if(IS_ERR(sensor_class)){
	  	printk("Cannot create device for PIR sensor\n");
	  	class_destroy(sensor_class);
	  	unregister_chrdev_region(pir_dev,1);
	  	return -1;
  }
  printk(KERN_INFO "Device File pir created\n"); 
  printk(KERN_INFO "PIR MOTION SENSOR DRIVER LOADED SUCCESSFULLY\n"); 

  return 0;
  
}

/*EXIT FUNCTION*/
static void __exit pir_sensor_exit(void) {
	
	free_irq(irq_number, NULL);
	gpio_free(GPIO_PIR);
	
	//Destroy class
	class_destroy(sensor_class);

	cdev_del(&pir_cdev);
	
	//Unregister PIR sensor
	unregister_chrdev_region(pir_dev,1);  
	
	printk(KERN_INFO "PIR MOTION SENSOR DRIVER REMOVED");

}

module_init(pir_sensor_init);
module_exit(pir_sensor_exit);


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PIR MOTION SENSOR DRIVER");
