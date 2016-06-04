inline void foo()
{
//	asm("movb 10h, %al;"
//		"movb 0Ah, %ah");
	asm("movw $0A31h, %ax");
	asm("movb $1, %cl");
	asm("int $10h");
}

#ifdef YES_MAIN

int main()
{
}

#else // YES_MAIN

void _start()
{
	while(1) foo();
}

#endif // YES_MAIN

