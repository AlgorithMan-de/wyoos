
#include <drivers/keyboard.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;


KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::OnKeyDown(char)
{
}

void KeyboardEventHandler::OnKeyUp(char)
{
}





KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler *handler)
: InterruptHandler(manager, 0x21),
dataport(0x60),
commandport(0x64)
{
    this->handler = handler;
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);
void printfHex(uint8_t);

void KeyboardDriver::Activate()
{
    while(commandport.Read() & 0x1)
        dataport.Read();
    commandport.Write(0xae); // activate interrupts
    commandport.Write(0x20); // command 0x20 = read controller command byte
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // command 0x60 = set controller command byte
    dataport.Write(status);
    dataport.Write(0xf4);
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();
    
    if(handler == 0)
        return esp;
    
    if(key < 0x80)
    {
        switch(key)
        {
            case 0x02: handler->OnKeyDown('1'); break;
            case 0x03: handler->OnKeyDown('2'); break;
            case 0x04: handler->OnKeyDown('3'); break;
            case 0x05: handler->OnKeyDown('4'); break;
            case 0x06: handler->OnKeyDown('5'); break;
            case 0x07: handler->OnKeyDown('6'); break;
            case 0x08: handler->OnKeyDown('7'); break;
            case 0x09: handler->OnKeyDown('8'); break;
            case 0x0A: handler->OnKeyDown('9'); break;
            case 0x0B: handler->OnKeyDown('0'); break;

            case 0x10: handler->OnKeyDown('q'); break;
            case 0x11: handler->OnKeyDown('w'); break;
            case 0x12: handler->OnKeyDown('e'); break;
            case 0x13: handler->OnKeyDown('r'); break;
            case 0x14: handler->OnKeyDown('t'); break;
            case 0x15: handler->OnKeyDown('z'); break;
            case 0x16: handler->OnKeyDown('u'); break;
            case 0x17: handler->OnKeyDown('i'); break;
            case 0x18: handler->OnKeyDown('o'); break;
            case 0x19: handler->OnKeyDown('p'); break;

            case 0x1E: handler->OnKeyDown('a'); break;
            case 0x1F: handler->OnKeyDown('s'); break;
            case 0x20: handler->OnKeyDown('d'); break;
            case 0x21: handler->OnKeyDown('f'); break;
            case 0x22: handler->OnKeyDown('g'); break;
            case 0x23: handler->OnKeyDown('h'); break;
            case 0x24: handler->OnKeyDown('j'); break;
            case 0x25: handler->OnKeyDown('k'); break;
            case 0x26: handler->OnKeyDown('l'); break;

            case 0x2C: handler->OnKeyDown('y'); break;
            case 0x2D: handler->OnKeyDown('x'); break;
            case 0x2E: handler->OnKeyDown('c'); break;
            case 0x2F: handler->OnKeyDown('v'); break;
            case 0x30: handler->OnKeyDown('b'); break;
            case 0x31: handler->OnKeyDown('n'); break;
            case 0x32: handler->OnKeyDown('m'); break;
            case 0x33: handler->OnKeyDown(','); break;
            case 0x34: handler->OnKeyDown('.'); break;
            case 0x35: handler->OnKeyDown('-'); break;

            case 0x1C: handler->OnKeyDown('\n'); break;
            case 0x39: handler->OnKeyDown(' '); break;

            default:
            {
                printf("KEYBOARD 0x");
                printfHex(key);
                break;
            }
        }
    }
    return esp;
}
