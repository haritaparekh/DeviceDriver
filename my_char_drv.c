#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>

#define DEV_MEM_SIZE 512

MODULE_DESCRIPTION("Simple Char Device Driver");
MODULE_AUTHOR("BeanBag");
MODULE_LICENSE("GPL");

dev_t device_number;


struct pcd_device{
	struct cdev pcd_cdev;
	int arr[DEV_MEM_SIZE];
};
/* cdev struct */
st^ruct cdev pcd_cdev;

/*File Operations struct */


int pcd_open(struct inode *inode, struct file *filp) {

    count++;
    printk(KERN_INFO "The device number extracted from inode is %d:%d\n", imajor(inode), iminor(inode));
    printk(KERN_INFO "The count of times open called so far is %d and the address of count is %x\n", count,&count);
    return 0;
}

int pcd_release(struct inode *inode, struct file *filp) {
    
    printk(KERN_INFO "The  pcd device %d is released\n", iminor(inode));
    return 0;
}

loff_t pcd_lseek(struct file *filp, loff_t off, int whence) {
    
    loff_t temp;
    switch(whence){
        case(SEEK_SET):
              if(off  >= DEV_MEM_SIZE || off < 0){
                return -EINVAL;
              }
              filp->f_pos = off;
            break;
        case(SEEK_CUR):
             temp =  filp->f_pos + off;
            if(temp >= DEV_MEM_SIZE || temp < 0){
                return -EINVAL;
            }
            filp->f_pos = temp;
            break;
        case(SEEK_END):
            temp =  DEV_MEM_SIZE + off;
            if(temp >= DEV_MEM_SIZE || temp < 0){
                return -EINVAL;
            }
            filp->f_pos = temp;
            break;
        default:
            return -EINVAL;
    }
    printk(KERN_INFO " The file position is changed to %lld\n", filp->f_pos);
    return filp->f_pos;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *off) {
    

    if(*off >= DEV_MEM_SIZE) {
        return 0;
    }
    /* Adjust the count*/
    if(*off + count >= DEV_MEM_SIZE) {
       count = DEV_MEM_SIZE - *off;
    }

    /* copy to user*/
    if(copy_to_user(buff, &buffer[*off], count)) {
        return -EFAULT;
    }

    *off += count;
    printk(KERN_INFO "Total bytes read %zu\n", count);
    return count;
}

ssize_t pcd_write(struct file *filp, const  char __user *buff, size_t count, loff_t *off) {
    
  ssize_t retval = -ENOMEM;
   
  /*Adjust count*/
  if(*off + count >= DEV_MEM_SIZE){
        count = DEV_MEM_SIZE - *off;
  }

  if(count == 0){
    return retval;
  }
    printk(KERN_INFO "Data written to file offset %lld\n", *off);
  if(copy_from_user(&buffer[*off], buff, count)){
        return -EFAULT;
  }


  *off += count;

    printk(KERN_INFO "Data written is %s\n",buffer);
  return count;

}

struct file_operations pcd_fops = {

    .open    = pcd_open,
    .write   = pcd_write,
    .read    = pcd_read,
    .llseek  = pcd_lseek,
    .release = pcd_release,
    .owner   = THIS_MODULE
};

struct class *class_pcd;
struct device *device_pcd;
static int my_char_drv_init(void){

    printk(KERN_INFO "Initializing my_char_drv\n");

    /* 1. Allocate device number dynamically*/
    alloc_chrdev_region(&device_number,0,1,"my_char_drv");
    
    printk(KERN_INFO "Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number));

    /* 2. Initialize cdev struct*/
    cdev_init(&pcd_cdev, &pcd_fops);

    /* 3. Register device(cdev_struct) with VFS.  */
    pcd_cdev.owner = THIS_MODULE;
    cdev_add(&pcd_cdev,device_number, 1);

    /* 4. Create device class under /sys/class/ */
    class_pcd = class_create(THIS_MODULE, "pcd_class");

    /* 5. Populate device info in /sys/class/ */
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");

    return 0;
}


static void my_char_drv_exit(void){

    printk(KERN_INFO "Exitng my_char_driver\n");

    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region( device_number, 1);
    printk(KERN_INFO "The module is unloaded\n");

    return ;
}



module_init(my_char_drv_init);
module_exit(my_char_drv_exit);
