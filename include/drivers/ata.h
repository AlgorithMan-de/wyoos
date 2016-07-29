 
#ifndef __MYOS__DRIVERS__ATA_H
#define __MYOS__DRIVERS__ATA_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>

namespace myos
{
    namespace drivers
    {
        
        class AdvancedTechnologyAttachment
        {
        protected:
            bool master;
            hardwarecommunication::Port16Bit dataPort;
            hardwarecommunication::Port8Bit errorPort;
            hardwarecommunication::Port8Bit sectorCountPort;
            hardwarecommunication::Port8Bit lbaLowPort;
            hardwarecommunication::Port8Bit lbaMidPort;
            hardwarecommunication::Port8Bit lbaHiPort;
            hardwarecommunication::Port8Bit devicePort;
            hardwarecommunication::Port8Bit commandPort;
            hardwarecommunication::Port8Bit controlPort;
        public:
            
            AdvancedTechnologyAttachment(bool master, common::uint16_t portBase);
            ~AdvancedTechnologyAttachment();
            
            void Identify();
            void Read28(common::uint32_t sectorNum, int count = 512);
            void Write28(common::uint32_t sectorNum, common::uint8_t* data, common::uint32_t count);
            void Flush();
            
            
        };
        
    }
}

#endif
