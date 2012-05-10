    /* 
     * ===================================================================================== 
     * 
     *       Filename:  s3c2440_serial.c 
     * 
     *    Description:  s3c2440 串口驱动程序 
     * 
     *        Version:  1.0 
     *        Created:  2011年01月11日 00时29分43秒 
     *       Revision:  none 
     *       Compiler:  linux 内核 
     * 
     *         Author:  sunsea1026 
     *          Email:  sunsea1026@gmail.com 
     * 
     * ===================================================================================== 
     */  
    #include <linux/module.h>  
    #include <linux/kernel.h>  
    #include <asm/uaccess.h>          //copy_to_use, copy_from_user  
    #include <linux/serial_core.h>  
    #include <../arch/arm/plat-samsung/include/plat/regs-serial.h>     //寄存器宏  
    #include <asm/io.h>               //readl, readb, writel, writeb  
    #define BUFSIZE 100  
    #define ttyS0_MAJOR 240  
    #define iobase      S3C24XX_VA_UART1  
    #define UART_ULCON1 iobase  
    #define UART_UCON1  iobase + 0x4  
    #define UART_UFCON1 iobase + 0x8  
    #define UART_UTRSTAT1   iobase + 0x10  
    #define UART_UTXH1  iobase + 0x20  
    #define UART_URXH1  iobase + 0x24  
    #define UART_UBRDIV1    iobase + 0x28  
      
    MODULE_AUTHOR("sunsea");  
    MODULE_DESCRIPTION("s3c2440 serial driver");  
    MODULE_LICENSE("GPL");  
    int sunsea_open(struct inode *inode, struct file *filp)  
    {  
        /* 8N1 */  
        writel(3, UART_ULCON1);  
        /* poll mode */  
        writel(5, UART_UCON1);  
        /* 115200 */  
        writel(26, UART_UBRDIV1);  
        return 0;  
    }  
    ssize_t sunsea_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)  
    {  
        char wbuf[BUFSIZE] = {0};  
        int state;  
        int i = 0;  
        copy_from_user(wbuf, buf, count);  
        while(wbuf[i] != '/0')  
        {  
            state = readl(UART_UTRSTAT1);  
            if((0x02 & state) == 2)  
            {  
                writeb(wbuf[i], UART_UTXH1);  
                i++;  
            }  
        }  
        return 0;  
    }  
    ssize_t sunsea_read(struct file *filp, char __user *buf, size_t count, loff_t *f_ops)  
    {  
        char rbuf[1] = {0};  
        int state;  
        state = readl(UART_UTRSTAT1);  
        if((0x01 & state) == 1)  
        {  
            rbuf[0] = readb(UART_URXH1);  
            copy_to_user(buf, rbuf, 1);  
        }  
        else  
        {  
            set_current_state(TASK_INTERRUPTIBLE);  
            schedule_timeout(10);  
        }  
        return 0;   
    }  
    int sunsea_release(struct inode *inode, struct file *filp)  
    {  
        return 0;  
    }  
    struct file_operations ttyS0_fops =   
    {  
        .owner = THIS_MODULE,  
        .open = sunsea_open,  
        .write = sunsea_write,  
        .read = sunsea_read,  
        .release = sunsea_release,  
    };  
    int __init  
    sunsea_init(void)  
    {  
        int rc;  
        printk("s3c2440 serial module loaded!/n");   
        rc = register_chrdev(ttyS0_MAJOR, "ttyS0", &ttyS0_fops);  
        if(rc < 0)  
        {  
            printk("Error: register ttyS0 device error!/n") ;  
            return -1;  
        }  
        return 0;   
    }  
    void __exit  
    sunsea_exit(void)  
    {  
        unregister_chrdev(ttyS0_MAJOR, "ttyS0");  
        printk("s3c2440 serial module exit!/n");   
        return;  
    }  
    module_init(sunsea_init);  
    module_exit(sunsea_exit);  
