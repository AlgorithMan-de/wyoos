
#include "types.h"
#include "gdt.h"
<<<<<<< HEAD
=======
#include "interrupts.h"
#include "keyboard.h"

>>>>>>> a2f1cad... Files for part 6, https://www.youtube.com/watch?v=RTKj4jdo_2s

void printf(char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x=0,y=0;

    for(int i = 0; str[i] != '\0'; ++i)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80)
        {
            x = 0;
            y++;
        }

        if(y >= 25)
        {
            for(y = 0; y < 25; y++)
                for(x = 0; x < 80; x++)
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void clear()
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint8_t x=0,y=0;

    for(y = 0; y < 25; y++)
        for(x = 0; x < 80; x++)
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
    x = 0;
    y = 0;
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}



extern "C" void kernelMain(const void* multiboot_structure, uint32_t /*multiboot_magic*/)
{
    printf("Hello World!");
    printf("more text that'll be erased");

    clear();

    printf("not erased!")    ;
    GlobalDescriptorTable gdt;
<<<<<<< HEAD
=======
    InterruptManager interrupts(0x20, &gdt);
    KeyboardDriver keyboard(&interrupts);
    interrupts.Activate();
>>>>>>> a2f1cad... Files for part 6, https://www.youtube.com/watch?v=RTKj4jdo_2s

    while(1);
}
