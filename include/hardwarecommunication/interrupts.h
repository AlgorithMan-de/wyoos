
#ifndef __MYOS__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H
#define __MYOS__HARDWARECOMMUNICATION__INTERRUPTMANAGER_H

#include <gdt.h>
#include <multitasking.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>


namespace myos
{
    namespace hardwarecommunication
    {

        class InterruptManager;

        class InterruptHandler
        {
        protected:
            myos::common::uint8_t InterruptNumber;
            InterruptManager* interruptManager;
            InterruptHandler(InterruptManager* interruptManager, myos::common::uint8_t InterruptNumber);
            ~InterruptHandler();
        public:
            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
        };


        class InterruptManager
        {
            friend class InterruptHandler;
            protected:

                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256];
                TaskManager *taskManager;

                struct GateDescriptor
                {
                    myos::common::uint16_t handlerAddressLowBits;
                    myos::common::uint16_t gdt_codeSegmentSelector;
                    myos::common::uint8_t reserved;
                    myos::common::uint8_t access;
                    myos::common::uint16_t handlerAddressHighBits;
                } __attribute__((packed));

                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer
                {
                    myos::common::uint16_t size;
                    myos::common::uint32_t base;
                } __attribute__((packed));

                myos::common::uint16_t hardwareInterruptOffset;
                static void SetInterruptDescriptorTableEntry(myos::common::uint8_t interrupt,
                    myos::common::uint16_t codeSegmentSelectorOffset, void (*handler)(),
                    myos::common::uint8_t DescriptorPrivilegeLevel, myos::common::uint8_t DescriptorType);


                static void InterruptIgnore();

                static void HandleInterruptRequest0x00();
                static void HandleInterruptRequest0x01();
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C();
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();
                static void HandleInterruptRequest0x31();

                static void HandleException0x00();
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();

                static myos::common::uint32_t HandleInterrupt(myos::common::uint8_t interrupt, myos::common::uint32_t esp);
                myos::common::uint32_t DoHandleInterrupt(myos::common::uint8_t interrupt, myos::common::uint32_t esp);

                Port8BitSlow programmableInterruptControllerMasterCommandPort;
                Port8BitSlow programmableInterruptControllerMasterDataPort;
                Port8BitSlow programmableInterruptControllerSlaveCommandPort;
                Port8BitSlow programmableInterruptControllerSlaveDataPort;

            public:
                InterruptManager(myos::common::uint16_t hardwareInterruptOffset, myos::GlobalDescriptorTable* globalDescriptorTable, myos::TaskManager* taskManager);
                ~InterruptManager();
                myos::common::uint16_t HardwareInterruptOffset();
                void Activate();
                void Deactivate();
        };
        
    }
}

#endif