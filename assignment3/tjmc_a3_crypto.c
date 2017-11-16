/*
 * A sample, extra-simple block driver. Updated for kernel 2.6.31.
 *
 * (C) 2003 Eklektix, Inc.
 * (C) 2010 Pat Patterson <pat at superpat dot com>
 * Redistributable under the terms of the GNU GPL.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>

MODULE_LICENSE("Dual BSD/GPL");
static char *Version = "1.4";

static int major_num = 0;
module_param(major_num, int, 0);
static int logical_block_size = 512;
module_param(logical_block_size, int, 0);
static int nsectors = 1024; /* How big the drive is */
module_param(nsectors, int, 0);

/*
 * We can tweak our hardware sector size, but the kernel talks to us
 * in terms of small sectors, always.
 */
#define KERNEL_SECTOR_SIZE 512

/*
 * Our request queue.
 */
static struct request_queue *Queue;

//struct for crypto 
static struct crypto_cipher *cipher_handle;
/*
 * The internal representation of our device.
 */
static struct sbd_device {
	unsigned long size;
	spinlock_t lock;
	u8 *data;
	struct gendisk *gd;
} Device;

struct tcrypt_result{
   struct completion completion;
   int err;
};

struct skcipher_def{
   struct scatterlist sg;
   struct crypto_skcipher *tfm;
   struct skcipher_request *req;
   struct tcrypt_result result;
};

static void test_skcipher_cb(struct crypto_async_request *req, int error){
   struct tcrypt_result *result = req->data;

   if(error == -EINPROGRESS)
      return;
   result->err = error;
   complete(&result->completion);
   pr_info("Encryption finished successfully\n");
}

static unsigned int test_skcipher_encdec(struct skcipher_def *sk, int enc){
   int rc = 0;

   if(enc)
      rc = crypto_skcipher_encrypt(sk->req);
   else
      rc = crypto_skcipher_decrypt(sk->req);
   switch(rc){
      case 0:
	 break;
      case -EINPROGRESS:
      case -EBUSY:
	 rc = wait_for_completion_interruptible(
	       &sk->result.completion);
	 if(!rc && !sk->result.err){
	    reinit_completion(&sk->result.completion);
	    break;
   	 }
      default:
	 pr_info("skcipher encrypt returned with %d result %d\n", rc, sk->result.err);
	 break;
   }
   init_completion(&sk->result.completion);

   return rc;

}

/*
 * Handle an I/O request.
 */
static void sbd_transfer(struct sbd_device *dev, sector_t sector,
		unsigned long nsect, char *buffer, int write) {
	unsigned long offset = sector * logical_block_size;
	unsigned long nbytes = nsect * logical_block_size;
	unsigned char key[32] = "key_try";
	int i;

	//setting the key for the cipher
	//get_random_bytes(&key, 32);
	if(crypto_cipher_setkey(cipher_handle, key, 32)){
	   pr_info("error when trying to set key\n");
	   printk("error setting key\n");
	   return;

	}

	if ((offset + nbytes) > dev->size) {
		printk (KERN_NOTICE "sbd: Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
	}
	if (write){
	//	memcpy(dev->data + offset, buffer, nbytes);
		for(i = 0; i < nbytes ; i+= crypto_cipher_blocksize(cipher_handle) ){
		crypto_cipher_encrypt_one(cipher_handle,dev->data+offset+i, buffer+i);//encrypting one block at a time


		}
	}
	else{
		//memcpy(buffer, dev->data + offset, nbytes);
		for(i = 0 ; i < nbytes ; i += crypto_cipher_blocksize(cipher_handle)){
	
		   crypto_cipher_decrypt_one(cipher_handle, buffer+i, dev->data + offset +i);
		}


	}
	
}	

static void sbd_request(struct request_queue *q) {
	struct request *req;

	req = blk_fetch_request(q);
	while (req != NULL) {
		// blk_fs_request() was removed in 2.6.36 - many thanks to
		// Christian Paro for the heads up and fix...
		//if (!blk_fs_request(req)) {
		if (req == NULL || (req->cmd_type != REQ_TYPE_FS)) {
			printk (KERN_NOTICE "Skip non-CMD request\n");
			__blk_end_request_all(req, -EIO);
			continue;
		}
		sbd_transfer(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req),
				bio_data(req->bio), rq_data_dir(req));
		if ( ! __blk_end_request_cur(req, 0) ) {
			req = blk_fetch_request(q);
		}
	}
}

/*
 * The HDIO_GETGEO ioctl is handled in blkdev_ioctl(), which
 * calls this. We need to implement getgeo, since we can't
 * use tools such as fdisk to partition the drive otherwise.
 */
int sbd_getgeo(struct block_device * block_device, struct hd_geometry * geo) {
	long size;

	/* We have no real geometry, of course, so make something up. */
	size = Device.size * (logical_block_size / KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 0;
	return 0;
}

/*
 * The device operations structure.
 */
static struct block_device_operations sbd_ops = {
		.owner  = THIS_MODULE,
		.getgeo = sbd_getgeo
};

static int __init sbd_init(void) {
	/*
	 * Set up our internal device.
	 */
	Device.size = nsectors * logical_block_size;
	spin_lock_init(&Device.lock);
	Device.data = vmalloc(Device.size);
	////////////////
	cipher_handle = crypto_alloc_cipher("aes", 0, 0);
	if(IS_ERR(cipher_handle)){
	   pr_err("error allocating cipher handle\n");
	   return PTR_ERR(cipher_handle);
	   //do something?
	}

	////////////////
	if (Device.data == NULL)
		return -ENOMEM;
	/*
	 * Get a request queue.
	 */
	Queue = blk_init_queue(sbd_request, &Device.lock);
	if (Queue == NULL)
		goto out;
	blk_queue_logical_block_size(Queue, logical_block_size);
	/*
	 * Get registered.
	 */
	major_num = register_blkdev(major_num, "sbd");
	if (major_num < 0) {
		printk(KERN_WARNING "sbd: unable to get major number\n");
		goto out;
	}
	/*
	 * And the gendisk structure.
	 */
	Device.gd = alloc_disk(16);
	if (!Device.gd)
		goto out_unregister;
	Device.gd->major = major_num;
	Device.gd->first_minor = 0;
	Device.gd->fops = &sbd_ops;
	Device.gd->private_data = &Device;
	strcpy(Device.gd->disk_name, "sbd0");
	set_capacity(Device.gd, nsectors);
	Device.gd->queue = Queue;
	add_disk(Device.gd);

	return 0;

out_unregister:
	unregister_blkdev(major_num, "sbd");
out:
	vfree(Device.data);
	return -ENOMEM;
}

static void __exit sbd_exit(void)
{
	del_gendisk(Device.gd);
	put_disk(Device.gd);
	unregister_blkdev(major_num, "sbd");
	blk_cleanup_queue(Queue);
	vfree(Device.data);
	if(cipher_handle){
	   crypto_free_cipher(cipher_handle);
	}
}

module_init(sbd_init);
module_exit(sbd_exit);
