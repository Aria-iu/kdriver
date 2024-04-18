#include <linux/module.h>
#include <linux/spi/spi.h>

#define DRIVER_NAME "spi_example"

static int spi_example_probe(struct spi_device *spi)
{
    printk(KERN_INFO "SPI device probed\n");
    // 在这里进行设备初始化工作，如配置SPI控制器、分配资源等
    return 0;
}

static int spi_example_remove(struct spi_device *spi)
{
    printk(KERN_INFO "SPI device removed\n");
    // 在这里进行设备清理工作，如释放资源等
    return 0;
}

static const struct of_device_id spi_example_of_match[] = {
    { .compatible = "spi_example", },
    { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, spi_example_of_match);

static struct spi_driver spi_example_driver = {
    .driver = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
        .of_match_table = of_match_ptr(spi_example_of_match),
    },
    .probe      = spi_example_probe,
    .remove     = spi_example_remove,
};

module_spi_driver(spi_example_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Example SPI Driver");
