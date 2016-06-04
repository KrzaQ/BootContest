
struct huj
{
	static int foo(){
		return 42;
	}
};

//void foo(){}

#ifdef YES_MAIN

int main()
{
}

#else // YES_MAIN

void _start()
{
	while(1);
}

#endif // YES_MAIN

