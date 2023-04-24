
#include <filesystem/fat.h>
using namespace myos;
using namespace myos::common;
using namespace myos::filesystem;
using namespace myos::drivers;


void printf(char*);
void printfHex(uint8_t);

void myos::filesystem::ReadBiosParameterBlock(AdvancedTechnologyAttachment *hd, uint32_t partitionOffset)
{
   /*
      * Read Bios Parameter block i.e. first sector of partition also known as VolumeId
      * Use bpb to find fatStart, fatSize, dataStart, rootStart
      * read first rootCluster to find location of files/directory on filesystem
      * iterate over directory entries and find location of files using firstClusterLow, firstClusterHi
      * then finally access 1st sector of data and print it
    */

   BiosParameterBlock32 bpb;
   hd->Read28(partitionOffset, (uint8_t*)&bpb, sizeof(BiosParameterBlock32));

   // Raw printing of VolumeID sector
   /*
   for(int i=0x00; i<sizeof(BiosParameterBlock32); i++)
   {
      printfHex(((uint8_t*)&bpb)[i]);
      printf(" ");
   }
   printf("\n");
   */

   uint32_t fatStart = partitionOffset + bpb.reservedSectors;
   uint32_t fatSize = bpb.tableSize;

   uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
   uint32_t rootStart = dataStart + bpb.sectorPerCluster*(bpb.rootCluster -2);

   DirectoryEntryFat32 dirent[16];
   hd->Read28(rootStart, (uint8_t*)&dirent[0], 16*sizeof(DirectoryEntryFat32));

   for(int i = 0; i<16; i++)
   {
      if(dirent[i].name[0] == 0x00)
         break;

      if((dirent[i].attributes & 0x0F) == 0x0F)
         continue;

      char *foo = "        \n";
      for(int j = 0; j<8; j++)
         foo[j] = dirent[i].name[j];
      printf(foo);

      if((dirent[i].attributes & 0x10) == 0x10) // directory
         continue;

      uint32_t fileCluster = ((uint32_t)dirent[i].firstClusterHi) << 16
                           | ((uint32_t)dirent[i].firstClusterLow);

      uint32_t fileSector = dataStart + bpb.sectorPerCluster * (fileCluster - 2) ;

      uint8_t buffer[512];

      hd->Read28(fileSector, buffer, 512);
      buffer[dirent[i].size] = '\0';
      printf((char*)buffer);
   }
}
