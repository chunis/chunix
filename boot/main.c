void bootmain(void)
{
	__asm__ ("movb  %%al,   0xb8000+160*24+34"::"a"('C'));
	__asm__ ("movb  %%al,   0xb8000+160*24+36"::"a"('h'));
	__asm__ ("movb  %%al,   0xb8000+160*24+38"::"a"('u'));
	__asm__ ("movb  %%al,   0xb8000+160*24+40"::"a"('n'));
	__asm__ ("movb  %%al,   0xb8000+160*24+42"::"a"('i'));
	__asm__ ("movb  %%al,   0xb8000+160*24+44"::"a"('X'));

	while (1)
		;	// do nothing
}
