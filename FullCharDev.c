#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kfifo.h>
#include <linux/cdev.h>
#include <linux/device.h>
MODULE_LICENSE("GPL");



char cipher_buffer[4096]="Hi hello";
char cipher_key_buffer [128] = "cikey";
char cipher_key_proc_buffer [128]= "pokey";
char cipher_encrypted_buffer [4096];


void rc4(unsigned char * p, unsigned char * k, unsigned char * c,int l)
{
        unsigned char s [256];
        unsigned char t [256];
        unsigned char temp;
        unsigned char kk;
        int i,j,x;
        for ( i  = 0 ; i  < 256 ; i ++ )
        {
                s[i] = i;
                t[i]= k[i % 4];
        }
        j = 0 ;
        for ( i  = 0 ; i  < 256 ; i ++ )
        {
                j = (j+s[i]+t[i])%256;
                temp = s[i];
                s[i] = s[j];
                s[j] = temp;
        }

        i = j = -1;
        for ( x = 0 ; x < l ; x++ )
        {
                i = (i+1) % 256;
                j = (j+s[i]) % 256;
                temp = s[i];
                s[i] = s[j];
                s[j] = temp;
                kk = (s[i]+s[j]) % 256;
                c[x] = p[x] ^ s[kk];
        }
}



static int cipherprocshow(struct seq_file *m, void *v) {
//  rc4(cipher_encrypted_buffer,cipher_key_proc_buffer, cipher_buffer,4096);
  seq_printf(m, "%s\n", cipher_buffer);
  return 0;
}
/*
static ssize_t cipherprocwrite(struct file * f, const char* ch, size_t sa, loff_t* offset){

  seq_printf( "You can only read from this file, you pervert\n");

}
*/
static int cipherprocopen(struct inode *inode, struct  file *file) {
  return single_open(file, cipherprocshow, NULL);
}

static const struct file_operations cipherprocfops = {
  .owner = THIS_MODULE,
  .open = cipherprocopen,
  .read = seq_read,
 // .write=cipherprocwrite,
  .llseek = seq_lseek,
  .release = single_release,
};



static int cipher_keyprocshow(struct seq_file *m, void *v) {
  seq_printf(m, "You cannot see the encryption key, buddy\n");
  return 0;
}

static ssize_t cipher_keyprocwrite(struct file * f, const char* ch, size_t sa, loff_t* offset){
copy_from_user(cipher_key_proc_buffer,ch,sa);
  rc4(cipher_encrypted_buffer,cipher_key_proc_buffer, cipher_buffer,4096);
return sa; 
}

static int cipher_keyprocopen(struct inode *inode, struct  file *file) {
  return single_open(file, cipher_keyprocshow, NULL);
}

static const struct file_operations cipher_keyprocfops = {
  .owner = THIS_MODULE,
  .open = cipher_keyprocopen,
  .read = seq_read,
  .write=cipher_keyprocwrite,
  .llseek = seq_lseek,
  .release = single_release,
};


static int cipherdevshow(struct seq_file *m, void *v) {
  
//  rc4(cipher_buffer,cipher_key_buffer, cipher_encrypted_buffer,4096);
  seq_printf(m, "%s\n", cipher_encrypted_buffer);
  return 0;
}

static ssize_t cipherdevwrite(struct file * f, const char* ch, size_t sa, loff_t* offset){
copy_from_user(cipher_buffer,ch,sa);

rc4(cipher_buffer,cipher_key_buffer, cipher_encrypted_buffer,4096);
return sa; 
  

}

static int cipherdevopen(struct inode *inode, struct  file *file) {
  return single_open(file, cipherdevshow, NULL);
}

static const struct file_operations cipherdevfops = {
  .owner = THIS_MODULE,
  .open = cipherdevopen,
  .read = seq_read,
  .write=cipherdevwrite,
  .llseek = seq_lseek,
  .release = single_release,
};



static int cipher_keydevshow(struct seq_file *m, void *v) {
  seq_printf(m, "You cannot see the encryption key, buddy\n");
  return 0;
}

static ssize_t cipher_keydevwrite(struct file * f, const char* ch, size_t sa, loff_t* offset){
copy_from_user(cipher_key_buffer,ch,sa);
return sa; 
}

static int cipher_keydevopen(struct inode *inode, struct  file *file) {
  return single_open(file, cipher_keydevshow, NULL);
}

static const struct file_operations cipher_keydevfops = {
  .owner = THIS_MODULE,
  .open = cipher_keydevopen,
  .read = seq_read,
  .write=cipher_keydevwrite,
  .llseek = seq_lseek,
  .release = single_release,
};


static int devflag1;
static int devflag2;
struct cdev chardev[2];



int init_module(void){
		
	proc_create("cipher", 0644, NULL, &cipherprocfops);
	proc_create("cipher_key", 0644, NULL, &cipher_keyprocfops);



	
	devflag1 = register_chrdev_region(MKDEV(300,0),2, "cipher");
	if (devflag1 >= 0)
		printk(KERN_ALERT "cipher: Device Registered\n");
	else 
		printk(KERN_ALERT "cipher: Device Registration failed\n");
	
/*	devflag2 = register_chrdev_region(MKDEV(300,1),2, "cipher_key");
	if (devflag2 >= 0)
		printk(KERN_ALERT "cipher_key: Device Registered\n");
	else 
		printk(KERN_ALERT "cipher_key: Device Registration failed\n");
*/
	cdev_init(&chardev[0], &cipherdevfops);
	cdev_add (&chardev[0], MKDEV (300,0),1);

	cdev_init(&chardev[1], &cipher_keydevfops);
	cdev_add (&chardev[1], MKDEV(300,1),1);
	return 0; 
}



void cleanup_module(void){

remove_proc_entry("cipher", NULL);
remove_proc_entry("cipher_key", NULL);
//	unregister_chrdev(300, "cipher");
	cdev_del(&chardev[0]);

	printk(KERN_ALERT "cipher: Unregistered the device\n");
//	unregister_chrdev(300, "cipher_key");

	cdev_del(&chardev[1]);
	
	printk(KERN_ALERT "cipher_key: Unregistered the device\n");
}



