#include "graphics.h"


void DrawTile(FILE* rom, long address, int xx, int yy, int pal, unsigned char sprite[80][64])
{
   unsigned char ch;
   int           i;
   int           y;
   char          sprLine[8][8];

   fseek(rom, address, SEEK_SET);

   for (y = 0; y < 8; y++)
   {
      ch = fgetc(rom);

      for (i = 0; i < 8; i++)
         sprLine[y][7 - i] = (ch & (1 << i)) >> i;

      ch = fgetc(rom);

      for (i = 0; i < 8; i++)
         sprLine[y][7 - i] += ((ch & (1 << i)) >> i) << 1;
   }

   for (y = 0; y < 8; y++)
   {
      ch = fgetc(rom);

      for (i = 0; i < 8; i++)
         sprLine[y][7 - i] += ((ch & (1 << i)) >> i) << 2;

      ch = fgetc(rom);

      for (i = 0; i < 8; i++)
         sprLine[y][7 - i] += ((ch & (1 << i)) >> i) << 3;
   }

   for (y = 0; y < 8; y++)
   {
      for (i = 0; i < 8; i++)
      {
         if (sprLine[y][i] == 0)
            sprite[yy + y][xx + i] = 0;
         else
            sprite[yy + y][xx + i] = (16 * pal) + sprLine[y][i];
      }
   }
}

void GetSprite(FILE* rom, int spriteNum, unsigned char sprite[80][64], int* width, int* height)
{
   // width and height and palette will have to be predetermined and then
   // selected in this function.  Calling functions do not require this info.
   // Same goes for address of each sprite.  This function just takes a
   // sprite "number", which is determined by where it appears in the rom.

   static OSprite osprite[1144];  // DJGPP doesn't like 1144 for some reason
   static int     firstTime = 1;
   unsigned int   address   = 0;
   int            offset    = 0;
   int            pal       = 0;

   if (firstTime == 1)
   {
      #include "osprites.h"
      firstTime = 0;
   }

   address = osprite[spriteNum].address;
   *width = osprite[spriteNum].width;
   *height = osprite[spriteNum].height;
   pal = osprite[spriteNum].palette;

   for (int y = 0; y < 80; y++)
   {
      for (int x = 0; x < 64; x++)
      {
         sprite[y][x] = 0;
      }
   }

{
   for (int a = 0; a < *height; a++)
   {
      for (int i = 0; i < *width; i++)
      {
         DrawTile(rom, address + offset, i * 8, a * 8, pal, sprite);
         offset += 32;
      }
   }
}
}

