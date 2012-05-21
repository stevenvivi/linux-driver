irq_handler()
{
	...
	int int_src = read_int_status(); /*读硬件中断相关寄存器*/
	switch(int_src)
	{
		case DEV_A:
			dev_a_hadler();
			break;
		case DEV_B:
			break;
		default:
			break;
	}
}:
