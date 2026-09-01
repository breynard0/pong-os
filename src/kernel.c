static volatile char* VGA_BUFFER = (void*)0xB8000;

static void push_hi()
{
    *(VGA_BUFFER) = 0x0f;
    *(VGA_BUFFER + 1) = 'H';
    *(VGA_BUFFER + 2) = 0x0f;
    *(VGA_BUFFER + 3) = 'i';
}

extern void _start(void)
{
    push_hi();
}
