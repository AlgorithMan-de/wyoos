
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
      * then Read all sector/cluster belonging to file:-
        i)  Get next cluster belonging to file from FAT table
        ii) Get next sector belonging to file from current file cluster
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

    printf("sector per cluster: ");
    printfHex(bpb.sectorPerCluster);
    printf("\n");

   uint32_t fatStart = partitionOffset + bpb.reservedSectors;
   uint32_t fatSize = bpb.tableSize;

   uint32_t dataStart = fatStart + fatSize * bpb.fatCopies;
   uint32_t rootStart = dataStart + bpb.sectorPerCluster*(bpb.rootCluster -2);

   DirectoryEntryFat32 dirent[16];
   hd->Read28(rootStart, (uint8_t*)&dirent[0], 16*sizeof(DirectoryEntryFat32));

   for(int i = 0; i<16; i++)
   {
      if(dirent[i].name[0] == 0x00) // unused directory entry
         break;

      if((dirent[i].attributes & 0x0F) == 0x0F) // long name directory entry
         continue;

      char *foo = "        \n";
      for(int j = 0; j<8; j++)
         foo[j] = dirent[i].name[j];
      printf(foo);

      if((dirent[i].attributes & 0x10) == 0x10) // directory
         continue;



      /* Read all sector/cluster belonging to file */
      uint32_t firstFileCluster = ((uint32_t)dirent[i].firstClusterHi) << 16
                                | ((uint32_t)dirent[i].firstClusterLow);

      int32_t SIZE = dirent[i].size;
      int32_t nextFileCluster = firstFileCluster;
      uint8_t buffer[513];
      uint8_t fatBuffer[513];

      while(SIZE > 0)
      {
         uint32_t fileSector = dataStart + bpb.sectorPerCluster * (nextFileCluster - 2) ;
         int sectorOffset = 0;

         for(; SIZE > 0; SIZE -= 512)
         {
            /* Get next sector belonging to file from current file cluster
                1) read sector by sector data of file until SIZE < 0 or
                  all sectors in currentCluster are read
            */
            hd->Read28(fileSector + sectorOffset, buffer, 512);
            buffer[SIZE > 512 ? 512 : SIZE] = '\0';
            printf((char*)buffer);

            if(++sectorOffset > bpb.sectorPerCluster)
                break;
         }

         /* Get next cluster belonging to file from FAT table
            1) get sector in FAT table holding current file cluster
            2) fig out current file cluster entry num in FAT sector out of 128 directory entries
            3) update next cluster with most significant 28 bits
         */
         uint32_t fatSectorForCurrentCluster = nextFileCluster / (512/sizeof(uint32_t));
         hd->Read28(fatStart + fatSectorForCurrentCluster, fatBuffer, 512);

         uint32_t fatOffsetInFatSector_ForCurrentCluster = nextFileCluster % (512/sizeof(uint32_t));
         nextFileCluster = ((uint32_t*)&fatBuffer)[fatOffsetInFatSector_ForCurrentCluster] & 0x0FFFFFFF;
      }
   }
}
