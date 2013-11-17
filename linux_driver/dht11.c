/**
 ** dht11驱动程序v1.0
 ** 此版本只提供简单的读写接口
 **  Author: LiuNing
 **  Date:   2013-11-15
 **/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/delay.h> 
#include <linux/uaccess.h>
#include <linux/miscdevice.h>

#include <asm/irq.h>
#include <mach/irqs.h>
#include <mach/io.h>
#include <mach/gpio.h>
#include <mach/at91_pio.h>
#include <mach/at91_pmc.h>
#include <mach/hardware.h>

#define DEVICE_NAME        "dht11"
#define GPIO_MAJOR            235

#define GPIO_OUT_0              0
#define GPIO_OUT_1              1
#define GPIO_PULLUP_0           0 
#define GPIO_PULLUP_1           1

#define DHT11_PIN    AT91_PIN_PA1   /**< 定义引脚 */

static void set_pin_high(void)
{
    at91_set_gpio_output(DHT11_PIN, GPIO_OUT_1);
}

static void set_pin_low(void)
{
    at91_set_gpio_output(DHT11_PIN, GPIO_OUT_0);
} 

static void set_pin_input(void)
{
    at91_set_gpio_input(DHT11_PIN, GPIO_PULLUP_0);
}

static int set_pin_get(void)
{
    if (at91_get_gpio_value(DHT11_PIN))
        return 1;
    else
        return 0;
}

static unsigned char DHT11_read_byte(void)
{
    char DHT11_byte;
    unsigned char i;
    unsigned char temp;

    DHT11_byte = 0;
    for (i = 0; i < 8; i++)
    {
        temp = 0;
        while (!(set_pin_get()))
        {
            temp++;
            if (temp > 12)
                return 1;
            udelay(5);
        }
        temp = 0;
        while (set_pin_get())
        {
            temp++;
            if (temp > 20)
                return 1;
            udelay(5);
        }
        if (temp > 6)
        {
            DHT11_byte <<= 1;
            DHT11_byte |= 1;
        }
        else
        {
            DHT11_byte <<= 1;
            DHT11_byte |= 0;
        }
    }
    return DHT11_byte;
}

static int
get_dht11_data(unsigned char *pbuf)
{
    int i;
    unsigned char DataTemp;
    int ret = -EFAULT;

    set_pin_high(); //拉高总线
    udelay(40);
    set_pin_low(); //拉低总线
    msleep(20);
    set_pin_high(); //拉高总线
    udelay(40);
    set_pin_input(); //设置输入模式

    do
    {
        DataTemp = 10;
        while (!(set_pin_get()) && DataTemp)
        {
            DataTemp--;
            udelay(10);
        }
        if (!DataTemp)
        {
            break;
        }
        //
        DataTemp = 10;
        while ((set_pin_get()) && DataTemp)
        {
            DataTemp--;
            udelay(10);
        }
        if (!DataTemp)
        {
            break;
        }
        //读取5字节
        for (i = 0; i < 5; i++)
        {
            pbuf[i] = DHT11_read_byte();
        }
        //计算校验和
        DataTemp = 0;
        for (i = 0; i < 4; i++)
        {
            DataTemp += pbuf[i];
        }
        if (DataTemp != pbuf[4])
        {
            break;
        }
        ret = 5;
    }
    while (0);

    set_pin_high(); //拉高总线
    return ret;
}

static ssize_t DHT11_read ( struct file* filp, char __user* buf, size_t count, loff_t* f_pos )
{
    unsigned char i;
    char tempBuf[5];
    ssize_t ret = -EFAULT;

    for (i = 0; i < 5; i++)
    {
        if (5 == get_dht11_data(tempBuf))
        {
            break;
        }
    }
    if (i < 5)
    {
        if (!copy_to_user(buf, tempBuf, 5))
        {
            ret = 5;
        }
    }

    return ret;
}


static struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .read = DHT11_read,
    };

static struct miscdevice misc = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,
    };

static int __init DHT11_init_module ( void )
{
    int ret;

    set_pin_high();
    ret = misc_register(&misc);
    printk (DEVICE_NAME"\tinitialized\n");
    return ret;
}

static void __exit DHT11_exit_module ( void )
{
    misc_deregister(&misc);
}

module_init(DHT11_init_module);
module_exit(DHT11_exit_module);

MODULE_AUTHOR("ln587");
MODULE_DESCRIPTION("QSSX DHT11 Driver");
MODULE_LICENSE("GPL");
