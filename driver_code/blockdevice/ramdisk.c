// blk_status_t 在linux-4.0内核中不存在
// 直接替换其中的宏定义即可

#include <linux/major.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/hdreg.h>
#include <linux/blk_types.h>
#include <linux/types.h>

#include <asm/setup.h>

#define DEVICE_NAME "myrb"
#define MYRAMBLOCK_SIZE (1024 * 1024)

static int major = 0;
static unsigned char *myrb_buf;

static DEFINE_SPINLOCK(myrb_lock);

static struct gendisk *myrb_gendisk;
static struct request_queue *myrb_queue;

static int myrb_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
    /* 容量=heads * cylinders * sectors * 512 */
    geo->heads = 2;
    geo->cylinders = 32;
    geo->sectors = MYRAMBLOCK_SIZE / geo->heads / geo->cylinders / 512;
    return 0;
}

static const struct block_device_operations myrb_fops = {
    .owner  = THIS_MODULE,
    .getgeo = myrb_getgeo,
};

static void do_myrb_request(struct request_queue *q)
{
    static int r_cnt = 0;
    static int w_cnt = 0;
    struct request *req;

    req = blk_fetch_request(q);
    while(req) {
        /* 数据传输三要素：源，目的，长度 */
        unsigned long offset = blk_rq_pos(req) << 9;
        /* 目的/源 */
        //req->buffer

        /* 长度 */
        unsigned long len = blk_rq_cur_bytes(req);
        void *buffer = bio_data(req->bio);
        unsigned char err = 0;

        if (offset + len > MYRAMBLOCK_SIZE) {
            pr_err(DEVICE_NAME ": bad access: block=%llu, "
                    "count=%u\n",
                    (unsigned long long)blk_rq_pos(req),
                    blk_rq_cur_sectors(req));
            err = 10;
            goto done;
        }

        if (rq_data_dir(req) == READ) {
            printk("do_myrb_request read %d len = %d\n", ++r_cnt, len);
            memcpy(buffer, myrb_buf + offset, len);
        } else {
            printk("do_myrb_request write %d\n", ++w_cnt);
            memcpy(myrb_buf + offset, buffer, len);
        }
done:
        if (!__blk_end_request_cur(req, err))
            req = blk_fetch_request(q);
    }
}

static int __init myrb_init(void)
{
    int ret;

    printk("Hello World!\n");

    ret = -EBUSY;
    major = register_blkdev(0, DEVICE_NAME);
    if (major <= 0)
        goto err;

    /* 1. 分配一个gendisk结构体 */
    ret = -ENOMEM;
    myrb_gendisk = alloc_disk(1);
    if (!myrb_gendisk) {
        goto out_disk;
    }

    myrb_queue = blk_init_queue(do_myrb_request, &myrb_lock);
    if (!myrb_queue)
        goto out_queue;

    myrb_gendisk->major = major;
    myrb_gendisk->first_minor = 0;
    myrb_gendisk->fops = &myrb_fops;
    sprintf(myrb_gendisk->disk_name, DEVICE_NAME);

    myrb_gendisk->queue = myrb_queue;
    set_capacity(myrb_gendisk, MYRAMBLOCK_SIZE / 512);

    /* 硬件相关的操作 */
    myrb_buf = kzalloc(MYRAMBLOCK_SIZE, GFP_KERNEL);

    add_disk(myrb_gendisk);

        return 0;

out_queue:
    put_disk(myrb_gendisk);
out_disk:
    unregister_blkdev(major, DEVICE_NAME);
err:
    return ret;
}

static void __exit myrb_exit(void)
{
        printk("Goodbye Cruel World!\n");
    unregister_blkdev(major, DEVICE_NAME);
    del_gendisk(myrb_gendisk);
    put_disk(myrb_gendisk);
    blk_cleanup_queue(myrb_queue);
}

module_init(myrb_init);
module_exit(myrb_exit);

MODULE_LICENSE("GPL");
