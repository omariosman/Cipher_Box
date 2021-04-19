#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kfifo.h>
#include<linux/cdev.h>

MODULE_LICENSE("GPL");


/* Functions of the proc file system entry */

/* cipher proc */
static int cipher_output(struct seq_file *m, void *v){
	seq_printf(m, "%d\n","cipher_output");
	return 0;
}

/* function to open the file */
static int cipher_open(struct inode *inode, struct file *file){
	return single_open(file, cipher_output ,NULL);
}

/* function to write 0 in the file */
static ssize_t cipher_write(struct file *file, const char *ch, size_t count, loff_t *pos){
	return -EINVAL;
}

static const struct file_operations cipher_fops = {
	.owner = THIS_MODULE,
	.open = cipher_open,
	.read = seq_read,
	.write = cipher_write,
	.llseek = seq_lseek,
	.release = single_release,

};


/* cipher_key proc  */

/* Functions of the proc file system entry */

/* cipher proc */
static int cipher_key_output(struct seq_file *m, void *v){
	seq_printf(m, "%d\n", "cipher_key");
	return 0;
}

/* function to open the file */
static int cipher_key_open(struct inode *inode, struct file *file){
	return single_open(file, fork_proc_output ,NULL);
}

static const struct file_operations cipher_key_fops = {
	.owner = THIS_MODULE,
	.open = cipher_key_open,
	.read = seq_read,
	.write = cipher_key_write,
	.llseek = seq_lseek,
	.release = single_release,

};





/* RC4 */
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


static int my_open(struct inode *, struct file *);
static int my_close(struct inode *, struct file *);
static ssize_t my_read(struct file *, char *, size_t, loff_t *);
static ssize_t my_write(struct file *,  const char *, size_t, loff_t *);
static int write_call = 0;
static int num_writes = 0;
static int file_opens = 0;
static int major_number;
char key[256] = {0};
char encrypted_data[256];
static int encrypt_index = 0;


/* device struct for cipher */
static struct file_operations cipher = 
{
	.read = cipher_read,
	.write = cipher_write,
	.open = cipher_open,
	.release = cipher_close,
};

/* device struct for cipher_hey */
static struct file_operations cipher_key = 
{
	.read = cipher_key_read,
	.write = cipher_key_write,
	.open = cipher_key_open,
	.release = cipher_key_close,
};


static int ex9_init(void)
{
	
	major_number = register_chrdev_region(MKDEV(MY_MAJOR, 0), 2, "cipher");
	
	if (major != 0){
		return major;
	}

	cdev_init(&dev[0], &cipher_fops);
	cdev_add(&dev[0], MKDEV(MY_MAJOR, 0), 1);

	cdev_init(&dev[1], &key_fops);
	cdev_add(&dev[1], MKDEV(MY_MAJOR, 1), 1);

	cipher = proc_create("cipher", 0, NULL, &proc_cipher_fops);
	cipher_key = proc_create("cipher_key", 0, NULL, &proc_cipher_key_fops);


	

	return 0;
	
}


static ssize_t my_write(struct file *fp, const char *buffer, size_t length, loff_t *offset)
{
	/*
	int message_index = length - 1;
	char encrypt[256];
	int g = 0, error = 0;
	if (write_call == 0)
	{
		copy_from_user(key, buffer, length);
		write_call = 1;
		return 1;
	}
	
	error = copy_from_user(encrypt, buffer, length);
	if (error == 0)
	{
		printk(KERN_INFO "decdev: Successfully encrypted block %d of data", num_writes+1);
	}
	else
	{
		printk(KERN_INFO "decdev: Failed to encrypt block %d of data", num_writes+1);	
		return 0;
	}
	for ( ; encrypt_index < num_writes * 16 + length ; ++encrypt_index)
	{
		encrypted_data[encrypt_index] = (int)encrypt[g] ^ (int)key[g];
		key[g++] = encrypted_data[encrypt_index];
	}
	num_writes++;
	return message_index;
*/
}

static int my_open(struct inode *ip, struct file *fp)
{
	/*
	file_opens++;
	printk(KERN_INFO "encdev: device was opened\n");
	return 0;
	*/
}

static int my_close(struct inode *ip, struct file *fp)
{
	/*
	printk(KERN_INFO "encdev: device was closed\n");
	write_call = 0;
	num_writes = 0;
	encrypt_index = 0;
	return 0;
*/
}

static ssize_t my_read(struct file *fp, char *buffer, size_t length, loff_t *offset)
{
	/*
	
	int error = 0;
	encrypted_data[encrypt_index] = '\0';
	error = copy_to_user(buffer, encrypted_data, encrypt_index);
	if (error == 0)
	{
		printk(KERN_INFO "encdev: Read the encrypted text successfully\n");
		return 0;
	}
	else
	{
		printk(KERN_INFO "encdev: Failed to read the encrypted text\n");
		return -EFAULT;
	}
	*/
}


static void ex9_exit(void){
	
	remove_proc_entry("cipher", NULL);
	remove_proc_entry("cipher_key", NULL);

	cdev_del(&dev[0]);
	cdev_del(&dev[1]);
}



module_init(ex9_init);
module_exit(ex9_exit);
