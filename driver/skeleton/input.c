#include <asm/irq.h>
#include <asm/io.h>

static struct input_dev *button_dev;
static irqreturn_t button_interrupt(int irq, void *dummy)
{
	input_report_key(button_dev, BTN_0, inb(BUTTON_PORT) & 1);
	input_sync(button_dev);
	return IRQ_HANDLED;
}

static int __init button_init(void)
{
	 int error;
	 if(request_irq(BUTTON_IRQ, button_interrupt, 0, "button", NULL))
	{
		printk(KERN_ERR "button.c: Can't allocate irq %d\n", button_irq);
		return - EBUSY;
	}
}
