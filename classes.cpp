#include "classes.h"
#include "eorder.h"

//ENEMY STUFF

void PullEnemies(FILE* rom, struct Enemy* enemy)
{
   long int      offset = 0;
   int           a;
   int           b;
   unsigned char ch;

   fseek(rom, 0x1597e7, SEEK_SET);
   fflush(rom);

   for (a = 0; a < 230; a++)
   {
      enemy[a].number = a;
      enemy[a].address = 1415143 + offset;
      enemy[a].theFlag = fgetc(rom);

      for (b = 0; b < 25; b++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            enemy[a].name[b] = 0;
         else
            enemy[a].name[b] = ch - 48;
      }

      ch = fgetc(rom);  // Unknown, varies from 0-3 or so
                        // unknown5.txt

      enemy[a].type = fgetc(rom); // 0 - normal, 1 - insect, 2 - metal

      enemy[a].insidePic = fgetc(rom);
      enemy[a].insidePic += (fgetc(rom) << 8);

      enemy[a].outsidePic = fgetc(rom);
      enemy[a].outsidePic += (fgetc(rom) << 8);

      enemy[a].runFlag = fgetc(rom);

      enemy[a].hp = fgetc(rom);
      enemy[a].hp += (fgetc(rom) * 256);

      enemy[a].pp = fgetc(rom);
      enemy[a].pp += (fgetc(rom) * 256);

      enemy[a].exp = fgetc(rom);
      enemy[a].exp += (fgetc(rom) * 256);
      enemy[a].exp += (fgetc(rom) * 256 * 256);
      enemy[a].exp += (fgetc(rom) * 256 * 256 * 256);

      enemy[a].money = fgetc(rom);
      enemy[a].money += (fgetc(rom) * 256);

      enemy[a].movement = fgetc(rom);
      ch = fgetc(rom); // Appears to always be 0

      enemy[a].startAddress = fgetc(rom);
      enemy[a].startAddress += (fgetc(rom) << 8);
      enemy[a].startAddress += (fgetc(rom) << 16);
      enemy[a].startAddress += (fgetc(rom) << 24);

      enemy[a].dieAddress = fgetc(rom);
      enemy[a].dieAddress += (fgetc(rom) << 8);
      enemy[a].dieAddress += (fgetc(rom) << 16);
      enemy[a].dieAddress += (fgetc(rom) << 24);

      enemy[a].palette = fgetc(rom);

      enemy[a].level = fgetc(rom);

      enemy[a].music = fgetc(rom);

      enemy[a].offense = fgetc(rom);

      ch = fgetc(rom); // Appears to always be 0

      enemy[a].defense = fgetc(rom);

      ch = fgetc(rom);  // Unknown, but mostly 0, sometimes 1
                        // unknown4.txt

      enemy[a].speed = fgetc(rom);
      enemy[a].guts = fgetc(rom);
      enemy[a].iq = fgetc(rom);

      ch = fgetc(rom);  // Appears to vary from 0-4 or so
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      enemy[a].missRate = fgetc(rom);  // 0 = almost never, 255 = always

      enemy[a].order = fgetc(rom);

      enemy[a].action[0] = fgetc(rom);
      enemy[a].action[0] += (fgetc(rom) << 8);
      enemy[a].action[1] = fgetc(rom);
      enemy[a].action[1] += (fgetc(rom) << 8);
      enemy[a].action[2] = fgetc(rom);
      enemy[a].action[2] += (fgetc(rom) << 8);
      enemy[a].action[3] = fgetc(rom);
      enemy[a].action[3] += (fgetc(rom) << 8);

      enemy[a].finalAction = fgetc(rom);
      enemy[a].finalAction += (fgetc(rom) << 8);

      enemy[a].amigo[0] = fgetc(rom);
      enemy[a].amigo[1] = fgetc(rom);
      enemy[a].amigo[2] = fgetc(rom);
      enemy[a].amigo[3] = fgetc(rom);

      ch = fgetc(rom);  // dunno, always 0, except Carbon Dog, who has 83

      ch = fgetc(rom);  // seems to be size, or maybe precedence, possibly
                        // linked to calling enemies, unknown3.txt

      enemy[a].bossFlag = fgetc(rom);

      enemy[a].freq = fgetc(rom);
      enemy[a].item = fgetc(rom);

      enemy[a].status = fgetc(rom);
      enemy[a].dieSound = fgetc(rom);

      ch = fgetc(rom);  // Has to do with battle layout, 0 or 1
                        // unknown2.txt

      enemy[a].maxCall = fgetc(rom);  // Very max = 8
      
      ch = fgetc(rom);  // Unknown, numbers vary somewhat, maybe enemy type
                        // unknown1.txt

      offset += 94;
   }
}


void Enemy::ChangeTheFlag(FILE* rom, unsigned char newValue)
{
   // rom must already be open, and in write mode
   // is assumed newValue is 0 or 1

   // if newValue is 1, then this enemy will have the word "the"
   // displayed in front of its name during battle

   fseek(rom, address, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   theFlag = newValue;
}


void Enemy::ChangeName(FILE* rom, char* newName)
{
   // Assumes rom is in write binary mode
   // Assumes newName is 25 characters long, last one is 0

   fseek(rom, address + 1, SEEK_SET);
   fflush(rom);

   for (int i = 0; i < 25; i++)
   {
      if (newName[i] == 0)
      {
         fputc(0, rom);
         name[i] = 0;
      }
      else
      {
         fputc(newName[i] + 48, rom);
         name[i] = newName[i];
      }
   }
}


void Enemy::ChangeInsidePic(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are found in a file called inside.txt.
   // It may turn out that the inside pic is decided using an address
   // instead of an array index.  I'll keep you posted.

   fseek(rom, address + 28, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   insidePic = newValue;
}


void Enemy::ChangeOutsidePic(FILE* rom, int newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are written in a text file called
   // outside.txt

   fseek(rom, address + 30, SEEK_SET);
   fflush(rom);

//   fputc(newValue, rom);
   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   outsidePic = newValue;
}


void Enemy::ChangeRunFlag(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Have no idea of possible #'s, except that only 6 and 7 are used
   // in EarthBound.  I'll figure this out later.

   fseek(rom, address + 32, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   runFlag = newValue;
}


void Enemy::ChangeHp(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Assumes integer is from 0 to 65535

   fseek(rom, address + 33, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   hp = newValue;
}


void Enemy::ChangePp(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Assumes integer is from 0 to 65535

   fseek(rom, address + 35, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   pp = newValue;
}


void Enemy::ChangeExp(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Assumes integer is from 0 to 2^32 (over 4 billion!)

   fseek(rom, address + 37, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255 , rom);
   fputc((newValue >> 16) & 255, rom);
   fputc((newValue >> 24) & 255, rom);

   exp = newValue;
}


void Enemy::ChangeMoney(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Assumes integer is from 0 to 65535

   fseek(rom, address + 41, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   money = newValue;
}


void Enemy::ChangeSpeed(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are from 0 - 255

   fseek(rom, address + 60, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   speed = newValue;
}


void Enemy::ChangePalette(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Valid #'s are from 0 - 31
   // Maybe later I'll have exact palette info for ya

   fseek(rom, address + 53, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   palette = newValue;
}


void Enemy::ChangeOffense(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are from 0-255

   fseek(rom, address + 56, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   offense = newValue;
}


void Enemy::ChangeDefense(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are from 0-255

   fseek(rom, address + 58, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   defense = newValue;
}


void Enemy::ChangeItem(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are 0-255, but 254 and 255 are sorta non-existent
   // items (at least in EB...)

   fseek(rom, address + 88, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   item = newValue;
}


void Enemy::ChangeFreq(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are from 0-7

   // The frequency equation seems to work this way:
   // drop percentage = (2^newValue)/128 of a chance
   // Thanks for bringing that to my attention, Spiffage ;)

   fseek(rom, address + 87, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   freq = newValue;
}


void Enemy::ChangeStatus(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are found in a file called enstatus.txt.

   fseek(rom, address + 89, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   status = newValue;
}


void Enemy::ChangeAction(FILE* rom, int newValue, int index)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are shown in effects.txt
   // Allowed values for index are 0-3.  index is the specific action
   // to be edited.  i.e. action[0], action[1], action[2], or action[3]

   fseek(rom, address + 70 + (index * 2), SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   action[index] = newValue;
}


void Enemy::ChangeFinalAction(FILE* rom, int newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are listed in effects.txt

   fseek(rom, address + 78, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   finalAction = newValue;
}


void Enemy::ChangeLevel(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are 0 to 255

   fseek(rom, address + 54, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   level = newValue;
}


void Enemy::ChangeGuts(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are 0 to 255

   fseek(rom, address + 61, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   guts = newValue;
}


void Enemy::ChangeIq(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are 0 to 255

   fseek(rom, address + 62, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   iq = newValue;
}


void Enemy::ChangeStartAction(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue appear to be 0 to 2^32.  But not all
   // values will be happy.  newValue acts sort of as a pointer to
   // the game text.  Except I haven't figured out why it's a 32 bit
   // address or number.

   fseek(rom, address + 45, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);
   fputc((newValue >> 16) & 255, rom);
   fputc((newValue >> 24) & 255, rom);

   startAddress = newValue;
}


void Enemy::ChangeDieAction(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue appear to be 0 to 2^32.  Works like
   // ChangeStartAction.

   fseek(rom, address + 49, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);
   fputc((newValue >> 16) & 255, rom);
   fputc((newValue >> 24) & 255, rom);

   dieAddress = newValue;
}

void Enemy::ChangeMovement(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values for newValue are given in movement.txt
   // Note that any values other than listed in that text file
   // may result in unplayable roms and or totally screwy stuff
   // may occur.

   fseek(rom, address + 43, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   movement = newValue;
}

void Enemy::ChangeOrder(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Changes the order in which enemies attack, depending on
   // newValue, which can be one of 4 numbers:
   // 0 - Random
   // 1 - Random, but tends to do the 3rd attack often
   // 2 - In order (Attack 1, Attack 2, Attack 3, Attack 4, repeat...)
   // 3 - "Staggered Order" (difficult to explain, just it goes something
   //     like "Attack 3, 4, 3, 4, 3, 4, 3, 1, 2, 1, 2, etc.  It's not
   //     a very logical pattern, but it's a pattern.  This is what Buzz
   //     Buzz does when he's in your party, and what other enemies do to.
   //     Just call it "Staggered Order" in the GUI, cuz it's too hard
   //     to figure out a better name for it.

   fseek(rom, address + 69, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   order = newValue;
}

void Enemy::ChangeFriend(FILE* rom, unsigned char slot, unsigned char enVal)
{
   // Assumes rom is in write binary mode
   // slot is from 0 to 3, designates one of the four friends this enemy
   // can have.
   // enVal is the number ID of the new enemy to be put in the given slot.

   // (Internal note: I have to add 1 to enVal, for some reason.  Weird.)

   fseek(rom, address + 80 + slot, SEEK_SET);
   fflush(rom);

   fputc(enVal, rom);

   amigo[slot] = enVal; // I don't add 1 here for confusing reasons
}

void Enemy::ChangeMaxCall(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // newValue can be from 0 to 255, but the game will only allow a max of
   // 8 it seems. Anything over 8 seems to be treated as 8.  Best to leave
   // this fully editable with a text field instead of a dropdown list.
   // This function changes how many called enemies there can be total.

   fseek(rom, address + 92, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   maxCall = newValue;
}

void Enemy::ChangeMissRate(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // newValue can be from 0 to 255.  0 means never miss, 255 means always
   // miss.  This is when they attack you.  Anyway, I'm not completely sure
   // how this figures in with your speed setting or whatever.  Perhaps there
   // are supposed to be enemies that always hit you, no matter what your
   // speed or something.

   fseek(rom, address + 68, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   missRate = newValue;
}

void Enemy::ChangeDieSound(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // newValue can be 0 or 1.  Other values *may* be allowed, but all the
   // other ones I tried just did the same as if it were just 1.
   // This changes the sound as the enemy is fading away.  0 makes it sound
   // like how a normal enemy dies.  1 makes it sound a bit like how bosses
   // die.

   fseek(rom, address + 90, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   dieSound = newValue;
}


void Enemy::ChangeType(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are from 0 - normal, 1 - insect, 2 - metal

   fseek(rom, address + 27, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   type = newValue;
}


void Enemy::ChangeMusic(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary form
   // Allowed values are from 0 to 191

   fseek(rom, address + 55, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   music = newValue;
}


int Enemy::CanCall(void)
{
   // Returns 1 if this enemy has at least one action that will let it
   // call enemies.  Returns 0 if it doesn't have any enemy-calling
   // actions.
   // I wrote this in here for some possible stuff we'll want to use for
   // internal code of PK Hack, but for now you prolly won't need it.

   int returnVal = 0;

   if (action[0] == 62 || action[1] == 62 || action[2] == 62 || action[3] == 62)
      returnVal = 1;
   else if (action[0] == 63 || action[1] == 63 || action[2] == 63 || action[3] == 63)
      returnVal = 1;

   return returnVal;
}


void SortEnemiesAlpha(Enemy* enemy)
{
   // Assumes enemy is allocated with at least 230 entries.
   // Only entries 0 through 229 will be rearranged alphabetically
   // (ascending order).
   // In the case that two enemies have the same name, they're rearranged
   // according to the "number" field, in ascending order.
   // No complaining anymore, spaan! ;)

   Enemy temp;
   char  currName[25];
   char  bigName[25];
   int   biggest;
   int   result;
   int   tempNum;
   int   i;
   int   j;

   for (i = 0; i < 229; i++)
   {
      biggest = i;

      for (j = i; j < 230; j++)
      {
         strcpy(bigName, enemy[biggest].name);
         strcpy(currName, enemy[j].name);

         strupr(bigName);
         strupr(currName);

         result = strcmp(currName, bigName);
         if (result < 0)
            biggest = j;
         else if (result == 0)
         {
            if (enemy[biggest].number > enemy[j].number)
               biggest = j;
         }
      }

      if (biggest != i)
         SwapEnemy(enemy[i], enemy[biggest]);
   }
}


void SwapEnemy(Enemy& enemy1, Enemy& enemy2)
{
   Enemy temp;

   temp = enemy1;
   strcpy(temp.name, enemy1.name);
   temp.action[0] = enemy1.action[0];
   temp.action[1] = enemy1.action[1];
   temp.action[2] = enemy1.action[2];
   temp.action[3] = enemy1.action[3];

   enemy1 = enemy2;
   strcpy(enemy1.name, enemy2.name);
   enemy1.action[0] = enemy2.action[0];
   enemy1.action[1] = enemy2.action[1];
   enemy1.action[2] = enemy2.action[2];
   enemy1.action[3] = enemy2.action[3];

   enemy2 = temp;
   strcpy(enemy2.name, temp.name);
   enemy2.action[0] = temp.action[0];
   enemy2.action[1] = temp.action[1];
   enemy2.action[2] = temp.action[2];
   enemy2.action[3] = temp.action[3];
}



void SortEnemiesGameOrder(Enemy* enemy)
{
   // Sorts this enemy array into the order listed in eorder.h
   // Assumes there are at least 230 entries allocated in the array
   // Only entries 0 thru 229 are sorted.

   int i;
   int j;

   for (i = 0; i < 229; i++)
   {
      for (j = i; j < 230; j++)
      {
         if (enemy[j].number == enemyGameOrder[i])
            SwapEnemy(enemy[j], enemy[i]);
      }
   }
}


void SortEnemiesOriginal(Enemy* enemy)
{
   // Assumes enemy is allocated with at least 230 entries.
   // Only entries 0 through 229 will be rearranged.
   // The enemy array gets rearranged to the original order as the items
   // appear in the ROM.  In other words, the order you get is exactly
   // the same as if you had just called PullEnemies.

   Enemy temp;
   char  currName[25];
   char  bigName[25];
   int   biggest;
   int   result;
   int   tempNum;
   int   i;
   int   j;

   for (i = 0; i < 229; i++)
   {
      biggest = i;

      for (j = i; j < 230; j++)
      {
         if (enemy[biggest].number > enemy[j].number)
            biggest = j;
      }

      if (biggest != i)
         SwapEnemy(enemy[i], enemy[biggest]);
   }
}

int SearchEnemy(Enemy* enemy, char* searchStr, int results[])
{
   int index = 0;

   for (int i = 0; i < 230; i++)
   {
       if (strstr(enemy[i].name, searchStr) != NULL)
          results[index++] = i; // enemy[i].number;
   }

   return index;
}


void DumpEnemies(Enemy* enemy, char* filename, int stats, int actions, int gfx)
{
   // Dumps enemy information into a text file.
   // If stats is non-zero, stats info will be printed
   // If actions is non-zero, action info will be printed
   // If gfx is non-zero, gfx info will be printed

   FILE* dumpFile;

   dumpFile = fopen(filename, "w");

   fprintf(dumpFile, "EarthBound enemy info dump generated by PK Hack\n");
   fprintf(dumpFile, "PK Hack by Tomato (tomato@starmen.net)");
   fprintf(dumpFile, " and spaanoft (spaanoft@starmen.net)\n");
   fprintf(dumpFile, "--------------------------------------");
   fprintf(dumpFile, "------------------------------------\n\n");

   for (int i = 0; i < 230; i++)
   {
      fprintf(dumpFile, "Enemy #:   %d\n", enemy[i].number);
      fprintf(dumpFile, "Name:      %s\n", enemy[i].name);

      if (stats)
      {
         fprintf(dumpFile, "the Flag:  %d\n", enemy[i].theFlag);
         fprintf(dumpFile, "HP:        %d\n", enemy[i].hp);
         fprintf(dumpFile, "PP:        %d\n", enemy[i].pp);
         fprintf(dumpFile, "Exp:       %u\n", enemy[i].exp);
         fprintf(dumpFile, "Money:     $%d\n", enemy[i].money);
         fprintf(dumpFile, "Speed:     %d\n", enemy[i].speed);
         fprintf(dumpFile, "Offense:   %d\n", enemy[i].offense);
         fprintf(dumpFile, "Defense:   %d\n", enemy[i].defense);
         fprintf(dumpFile, "Status:    %d\n", enemy[1].status);
         fprintf(dumpFile, "Level:     %d\n", enemy[i].level);
         fprintf(dumpFile, "Guts:      %d\n", enemy[i].guts);
         fprintf(dumpFile, "IQ:        %d\n", enemy[i].iq);
         fprintf(dumpFile, "Miss rate: %d\n", enemy[i].missRate);
         fprintf(dumpFile, "Item:      %d\n", enemy[i].item);
         fprintf(dumpFile, "Drop %%:    %d\n", enemy[i].freq);
         fprintf(dumpFile, "Type:      %d\n", enemy[i].type);
         fprintf(dumpFile, "Music:     %s\n", bgm[enemy[i].music]);
         fprintf(dumpFile, "Die Sound: %d\n", enemy[i].dieSound);
         fprintf(dumpFile, "Is a Boss: %d\n", enemy[i].bossFlag);
      }

      if (actions)
      {
         fprintf(dumpFile, "Action 1:  %s\n", action[enemy[i].action[0]]);
         fprintf(dumpFile, "Action 2:  %s\n", action[enemy[i].action[1]]);
         fprintf(dumpFile, "Action 3:  %s\n", action[enemy[i].action[2]]);
         fprintf(dumpFile, "Action 4:  %s\n", action[enemy[i].action[3]]);
         fprintf(dumpFile, "Order:     %d\n", enemy[i].order);
         fprintf(dumpFile, "F. Action: %s\n", action[enemy[i].finalAction]);

         fprintf(dumpFile, "Arg. 1:    %d\n", enemy[i].amigo[0]);
         fprintf(dumpFile, "Arg. 2:    %d\n", enemy[i].amigo[1]);
         fprintf(dumpFile, "Arg. 3:    %d\n", enemy[i].amigo[2]);
         fprintf(dumpFile, "Arg. 4:    %d\n", enemy[i].amigo[3]);

         fprintf(dumpFile, "Max Call:  %d\n", enemy[i].maxCall);

         fprintf(dumpFile, "S Address: 0x%x\n", enemy[i].startAddress);
         fprintf(dumpFile, "D Address: 0x%x\n", enemy[i].dieAddress);
      }

      if (gfx)
      {
         fprintf(dumpFile, "Out Pic #: %d\n", enemy[i].outsidePic);
         fprintf(dumpFile, "Movement:  %d\n", enemy[i].movement);
         fprintf(dumpFile, "In Pic #:  %d\n", enemy[i].insidePic);
         fprintf(dumpFile, "Palette #: %d\n", enemy[i].palette);
      }

      fprintf(dumpFile, "\n");
   }

   fclose(dumpFile);
}

void PullItems(FILE* rom, Item* item)
{
   // Assumes rom is in read binary
   // Assumes item is an array of 256 Item types

   char ch;
   int  offset = 0;

   fseek(rom, 0x155200, SEEK_SET);
   fflush(rom);

   for (int i = 0; i < 256; i++)
   {
      item[i].number = i;

      item[i].address = 0x155200 + offset;

      for (int j = 0; j < 25; j++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            item[i].name[j] = 0;
         else
            item[i].name[j] = ch - 48;
      }

      item[i].type = fgetc(rom);
      item[i].cost = fgetc(rom);
      item[i].cost += (fgetc(rom) << 8);
      item[i].ownership = fgetc(rom);
      item[i].effect = fgetc(rom);
      item[i].effect += (fgetc(rom) << 8);
      item[i].strength = fgetc(rom);
      item[i].increase = fgetc(rom);
      item[i].extraPower = fgetc(rom);
      item[i].specialProperties = fgetc(rom);
      item[i].descAddress = fgetc(rom);
      item[i].descAddress += (fgetc(rom) << 8);
      item[i].descAddress += (fgetc(rom) << 16);
      item[i].descAddress += (fgetc(rom) << 24);

      offset += 39;
   }
}

void Item::ChangeName(FILE* rom, char* newName)
{
   // Assumes rom is in write binary
   // Assumes newName is at most 25 characters (last one being null)

   fseek(rom, address, SEEK_SET);
   fflush(rom);

   for (int i = 0; i < 25; i++)
   {
      if (newName[i] == 0)
         fputc(0, rom);
      else
         fputc(newName[i] + 48, rom);

      name[i] = newName[i];
   }
}

void Item::ChangeType(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode
   // Input values are limited, so please use the following chart:
   // 16 = Weapon
   // 20 = Body equipment
   // 24 = Arm Equipment
   // 28 = Other Equipment
   // 32 = if edible item, will say "... ate the ..."
   // 36 = if edible item, will say "... drank the ..."
   // 40 = condiment

   // Here are other possible #'s, but their effect on the game
   // don't appear to be noticeable, so don't implement these until
   // I figure them out better.
   // 0   = no idea (only the Franklin Badge uses it)
   // 2   = only one of the semi-non-existent items uses it, prolly useless
   // 4   = Teddy Bear flag
   // 8   = Broken item
   // 44  = large pizza?
   // 48  = healing item
   // 52  = weird item, seems to mostly be stuff that affects enemy status
   // 53  = no idea (dragonite, fly honey, etc.)
   // 56  = no idea (piggy nose, for sale sign, etc.)
   // 58  = no idea (bicycle, hawk eye, zombie paper only)
   // 59  = important event item
   // 255 = Null

   fseek(rom, address + 25, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   type = newValue;
}

void Item::ChangeCost(FILE* rom, int newValue)
{
   // Assumes rom is in write binary mode
   // newValue must be from 0 to 65535

   fseek(rom, address + 26, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   cost = newValue;
}

void Item::ChangeOwnership(FILE* rom, unsigned char newValue)
{
   // Assumes rom is in write binary mode.

   // This is somewhat more complex than other things, so pay
   // special attention.  Ownership refers to who can use what
   // item, and it also goes so far as to specify who can hold
   // what items.  I'll give some examples later.

   // newValue is 8 bits long.
   // bit:   _ _ _ _  _ _ _ _
   // bit #: 8 7 6 5  4 3 2 1

   // Bit 1 determines if Ness can use this item
   // Bit 2 determines if Paula can use this item
   // Bit 3 determines if Jeff can use this item
   // Bit 4 determines if Poo can use this item

   // Bit 5 is weird, I don't know what it does, but it's chicken related
   // Bit 6 and 7 are weird.  If both are 1, then once you get an item,
   // you can't give them to anybody else in your party (and obviously to
   // any non-playable characters).
   // Bit 8 signifies if the item disappears after use or not.

   // NOTE: If the item is food, anybody can eat it, regardless of
   //       ownership flags.

   // Here are some examples:

   // dec: 01  bin: 0000 0001 Only Ness can use this, and it doesn't
   //                         disappear after use.

   // dec: 111 bin: 0110 1111 once somebody gets this item, they're not
   //                         able to get rid of it!  Ahh!  BTW, this is
   //                         how the ATM Card, Sound Stone, and Receiver
   //                         Phone work.

   // dec: 104 bin: 0110 1000 only Poo can hold this item, and he can't
   //                         give it to any other party members.  This
   //                         is how the Tiny Ruby works.

   // dec: 132 bin: 1000 0100 This item is for Jeff, and disappears after
   //                         being used.  This is how Bottle Rockets work.

   // dec: 4   bin: 0000 0100 Only Jeff can use this item.  And it doesn't
   //                         disappear after you use it.  Wouldn't it
   //                         be cool if a Multi Bottle Rocket used this? :)

   // dec: 8   bin: 0000 1111 This can be used by everybody, and doesn't
   //                         disappear after use.  This can let everybody
   //                         have a Gutsy bat, or even have food that
   //                         doesn't disappear after you eat it!  Yahooooo!

   // Of course there are a lot more possibilities.

   fseek(rom, address + 28, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   ownership = newValue;
}

void Item::ChangeEffect(FILE* rom, int newValue)
{
   // Assumes rom is in write binary mode

   // Effect means what will happen when you use this item.
   // Like, you can make a Teddy Bear edible (although the game will
   // do some funky stuff when you try to eat it), or even make
   // something like a Cookie heal all members, like a Large Pizza
   // does.

   // What's cool about this is that you can make it so when you
   // use any item, say a Cookie, you can do stuff you normally
   // could only do in battle.  You can even make items do stuff
   // only enemies could do!  But sometimes using an item like
   // that will make you die instantly or the screen will garble up.
   // But it's cool nonetheless.

   // Valid codes are given in a file I've created called effects.txt.

   fseek(rom, address + 29, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   effect = newValue;
}

void Item::ChangeStrength(FILE* rom, char newValue)
{
   // Assumes rom is in write binary mode
   // Note: Values are from -127 to 127

   fseek(rom, address + 31, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   strength = newValue;
}

void Item::ChangeExtraPower(FILE* rom, char newValue)
{
   // This function changes what this item will "increase"
   // when used.  Like how cookies restore HP, while PSI
   // caramels restore PP, and Luck Capsules increase Luck.

   // Assumes rom is in write binary mode
   // Input values are limited, so please use the following chart:
   // 0  = HP
   // 1  = PP
   // 2  = HP & PP
   // 3  = Like rock candy (random stat + 1)
   // 4  = IQ
   // 5  = Guts
   // 6  = Speed
   // 7  = Vitality
   // 8  = Luck
   // 9  = No visible effect (that's what it says if you use it)

   // Many items use numbers other than these, but I can't tell
   // what they signify yet, so for now just use this chart.

   fseek(rom, address + 33, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   extraPower = newValue;
}

void Item::ChangeIncrease(FILE* rom, unsigned char newValue)
{
   // "extraPower" is related to "increase".
   // While "extraPower" describes what stats will be increased,
   // "increase" describes how much that stat will be increased.
   // Like when you equip the Gutsy Bat, your Guts go up 127.
   // Or some things like the Rabbit's Foot increase your luck.
   // Essentially this function tells how much to increase by.

   // Assumes rom is in write binary mode
   // Input values are from -127 to 127.

   fseek(rom, address + 32, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   increase = newValue;
}

void Item::ChangeSpecialProperties(FILE* rom, unsigned char newValue)
{
   // This function changes what special properties an item has.
   // Like how some items help protect you against Fire Attacks,
   // others against Hypnosis, etc.

   // I still haven't completely figured out the numbering system
   // for this.  But numbers vary from 0 - 255.

   fseek(rom, address + 34, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   specialProperties = newValue;
}

void Item::ChangeDescAddress(FILE* rom, unsigned int newValue)
{
   // Assumes rom is in write binary mode
   // Allowed values are really unknown, oh well, that's
   // the user's problem.  Just let it be from 0 to FFFFFFFF.

   fseek(rom, address + 35, SEEK_SET);
   fflush(rom);

   fputc(newValue & 0xff, rom);
   fputc((newValue >> 8) & 0xff, rom);
   fputc((newValue >> 16) & 0xff, rom);
   fputc((newValue >> 24) & 0xff, rom);

   descAddress = newValue;
}


void SortItemsAlpha(Item* item)
{
   // Assumes item is allocated with at least 256 entries.
   // Only entries 0 through 255 will be rearranged alphabetically
   // (ascending order).
   // In the case that two items have the same name, they're rearranged
   // according to the "number" field, in ascending order.

   char currName[25];
   char bigName[25];
   int  biggest;
   int  result;
   int  i;
   int  j;

   for (i = 0; i < 255; i++)
   {
      biggest = i;

      for (j = i; j < 256; j++)
      {
         strcpy(bigName, item[biggest].name);
         strcpy(currName, item[j].name);

         strupr(bigName);
         strupr(currName);

         result = strcmp(currName, bigName);
         if (result < 0)
            biggest = j;
         else if (result == 0)
         {
            if (item[biggest].number > item[j].number)
               biggest = j;
         }
      }

      if (biggest != i)
         SwapItem(item[i], item[biggest]);
   }
}

void SortItemsType(Item* item)
{
   // Assumes item is allocated with at least 256 entries.
   // Only entries 0 through 255 will be rearranged in ascending order
   // by their "type" field.
   // In the case that two items have the same number, they're rearranged
   // by name.  If the names and types are the same, no special action
   // is taken, and they may be arranged in any order.

   char currName[25];
   char bigName[25];
   int  biggest;
   int  result;
   int  i;
   int  j;

   for (i = 0; i < 255; i++)
   {
      biggest = i;

      for (j = i; j < 256; j++)
      {
         if (item[biggest].type > item[j].type)
         {
            biggest = j;
         }
         else if (item[biggest].type == item[j].type)
         {
            strcpy(bigName, item[biggest].name);
            strcpy(currName, item[j].name);

            strupr(bigName);
            strupr(currName);

            result = strcmp(currName, bigName);
            if (result < 0)
               biggest = j;
         }
      }

      if (biggest != i)
         SwapItem(item[i], item[biggest]);
   }
}

void SortItemsOriginal(Item* item)
{
   // Assumes item is allocated with at least 256 entries.
   // Only entries 0 through 255 will be rearranged to their
   // in-ROM order.
   // In the case that two items have the same name, they're rearranged
   // according to the "number" field, in ascending order.

   int   biggest;
   int   i;
   int   j;

   for (i = 0; i < 255; i++)
   {
      biggest = i;

      for (j = i; j < 256; j++)
      {
         if (item[biggest].number > item[j].number)
            biggest = j;
      }

      if (biggest != i)
         SwapItem(item[i], item[biggest]);
   }
}


void SwapItem(Item& item1, Item& item2)
{
   Item temp;

   temp = item1;
   strcpy(temp.name, item1.name);

   item1 = item2;
   strcpy(item1.name, item2.name);

   item2 = temp;
   strcpy(item2.name, temp.name);
}


void DumpItems(Item* item, char* filename, int stats)
{
   // Dumps all the item information to a text file.
   // If stats is non-zero, all the stats of each item gets
   // printed out.  If it's zero, just the names and numbers of each
   // item get printed out.

   FILE* dumpFile;

   dumpFile = fopen(filename, "w");

   fprintf(dumpFile, "EarthBound item info dump generated by PK Hack\n");
   fprintf(dumpFile, "PK Hack by Tomato (tomato@starmen.net)");
   fprintf(dumpFile, " and spaanoft (spaanoft@starmen.net)\n");
   fprintf(dumpFile, "--------------------------------------");
   fprintf(dumpFile, "------------------------------------\n\n");

   for (int i = 0; i < 256; i++)
   {
      fprintf(dumpFile, "Item #:        %d\n", item[i].number);
      fprintf(dumpFile, "Name:          %s\n", item[i].name);

      if (stats)
      {
         fprintf(dumpFile, "Effect:        %d\n", item[i].effect);
         fprintf(dumpFile, "Cost           $%d\n", item[i].cost);
         fprintf(dumpFile, "Type:          %d\n", item[i].type);
         fprintf(dumpFile, "Strength:      %d\n", item[i].strength);
         fprintf(dumpFile, "Extra Power:   %d\n", item[i].extraPower);
         fprintf(dumpFile, "Increase #:    %d\n", item[i].increase);
         fprintf(dumpFile, "Special Prop.: %d\n", item[i].specialProperties);
         fprintf(dumpFile, "Desc. Address: 0x%x\n", item[i].descAddress);
         fprintf(dumpFile, "Ownership:     0x%.2x\n", item[i].ownership);
       }

       fprintf(dumpFile, "\n");
   }

   fclose(dumpFile);
}

//---diagnostic
int IsCorrectRom(FILE* rom)
{
   // Assumes rom is in read binary mode
   // Return Values:
   // 0 - Unknown/invalid ROM
   // 1 - EarthBound ROM
   // 2 - Mother 2 ROM

   char matchStr[16];
   int  returnVal = 0;
   int  i;

   for (i = 0; i < 16; i++)
      matchStr[i] = '\0';

   fseek(rom, 0x101c0, SEEK_SET);

   for (i = 0; i < 15; i++)
      matchStr[i] = fgetc(rom);

   if (!strcmp(matchStr, "EARTH BOUND (J)"))
      returnVal = 1;
   else if (!strcmp(matchStr, "MOTHER-2       "))
      returnVal = 2;

   return returnVal;
}

long FileSize(FILE* rom)
{
   long returnVal;

   fseek(rom, 0, SEEK_END);
   returnVal = ftell(rom);

   return returnVal;
}



int SearchItem(Item* item, char* searchStr, int results[])
{
   int index = 0;
   for (int i = 0; i < 256; i++)
   {
       if (strstr(item[i].name, searchStr) != NULL)
          results[index++] = i;
   }
   return index;
}


void PullPsi(FILE* rom, Psi* psi)
{
   // Assumes rom is in read binary mode
   // psi must already be allocated with at least 16 entries

   char ch;
   int  offset = 0;

   fseek(rom, 0x158f93, SEEK_SET);

   for (int i = 0; i < 16; i++)
   {
      psi[i].address = 0x158f93 + offset;

      for (int j = 0; j < 25; j++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            psi[i].name[j] = 0;
         else
            psi[i].name[j] = ch - 48;
      }

      offset += 25;
   }
}


void Psi::ChangeName(FILE* rom, char* newValue)
{
   // Assumes rom is in write binary mode
   // newValue is 25 characters in length, with the last character being 0.

   fseek(rom, address, SEEK_SET);

   for (int i = 0; i < 25; i++)
   {
      if (newValue[i] == 0)
      {
         fputc(0, rom);
         name[i] = 0;
      }
      else
      {
         fputc(newValue[i] + 48, rom);
         name[i] = newValue[i];
      }
   }
}

void PullTeleport(FILE* rom, Teleport* teleport)
{
   // Assumes rom is in read binary mode
   // psi must already be allocated with at least 16 entries

   char ch;
   int  offset = 0;

   fseek(rom, 0x157a9f, SEEK_SET);

   for (int i = 0; i < 16; i++)
   {
      teleport[i].address = 0x157a9f + offset;

      for (int j = 0; j < 25; j++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            teleport[i].name[j] = 0;
         else
            teleport[i].name[j] = ch - 48;
      }

      ch = fgetc(rom);  // seems to be map # or something???
      ch = fgetc(rom);  // seems to be map # or something???

      teleport[i].x = fgetc(rom);
      teleport[i].x += (fgetc(rom) << 8);
      teleport[i].y = fgetc(rom);
      teleport[i].y += (fgetc(rom) << 8);

      offset += 31;
   }
}


void Teleport::ChangeName(FILE* rom, char* newValue)
{
   // Assumes rom is in write binary mode
   // newValue is 25 characters in length, with the last character being 0.

   fseek(rom, address, SEEK_SET);

   for (int i = 0; i < 25; i++)
   {
      if (newValue[i] == 0)
      {
         fputc(0, rom);
         name[i] = 0;
      }
      else
      {
         fputc(newValue[i] + 48, rom);
         name[i] = newValue[i];
      }
   }
}


void Teleport::ChangeX(FILE* rom, int newValue)
{
   // This function changes the X coordinate of where you will
   // wind up when you teleport.

   // Assumes rom is in write binary form
   // Values allowed are 0 to 65535.  65535 might be insanely too big
   // sometimes, but oh well!

   // The game treats all maps as if they were on one big grid.  Using
   // coordinates, you can choose where you want to go on that big map.

   // For your convenience, coordinates used in the game are included in
   // a file called coords.txt.

   fseek(rom, address + 27, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   x = newValue;
}


void Teleport::ChangeY(FILE* rom, int newValue)
{
   // This function changes the Y coordinate of where you will
   // wind up when you teleport.

   // Assumes rom is in write binary form
   // Values allowed are 0 to 65535.  65535 might be insanely too big
   // sometimes, but that's the user's problem.  Heheh.

   // The game treats all maps as if they were on one big grid.  Using
   // coordinates, you can choose where you want to go on that big map.

   // For your convenience, coordinates used in the game are included in
   // a file called coords.txt.

   fseek(rom, address + 29, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   y = newValue;
}


 
void PullPhone(FILE* rom, Phone* phone)
{
   // Assumes rom is in read binary mode
   // phone must already be allocated with at least 5 entries

   char ch;
   int  offset = 0;

   fseek(rom, 0x157cae, SEEK_SET);

   for (int i = 0; i < 5; i++)
   {
      phone[i].address = 0x157cae + offset;

      for (int j = 0; j < 25; j++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            phone[i].name[j] = 0;
         else
            phone[i].name[j] = ch - 48;
      }

      ch = fgetc(rom);
      ch = fgetc(rom);

      phone[i].pointer = fgetc(rom);
      phone[i].pointer += (fgetc(rom) << 8);
      phone[i].pointer += (fgetc(rom) << 16);
      phone[i].pointer += (fgetc(rom) << 24);

      offset += 31;
   }
}


void Phone::ChangeName(FILE* rom, char* newValue)
{
   // Assumes rom is in write binary mode
   // newValue is 25 characters in length, with the last character being 0.

   fseek(rom, address, SEEK_SET);

   for (int i = 0; i < 25; i++)
   {
      if (newValue[i] == 0)
      {
         fputc(0, rom);
         name[i] = 0;
      }
      else
      {
         fputc(newValue[i] + 48, rom);
         name[i] = newValue[i];
      }
   }
}


void Phone::ChangePointer(FILE* rom, unsigned int newValue)
{
   fseek(rom, address + 27, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);
   fputc((newValue >> 16) & 255, rom);
   fputc((newValue >> 24) & 255, rom);

   pointer = newValue;
}

void PullAilments(FILE* rom, Ailment* ailment)
{
   // Assumes rom is in read binary mode
   // ailment must already be allocated with at least 10 entries

   char ch;
   int  offset = 0;

   fseek(rom, 0x045d70, SEEK_SET);

   for (int i = 0; i < 10; i++)
   {
      ailment[i].address = 0x045d70 + offset;

      for (int j = 0; j < 16; j++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            ailment[i].name[j] = 0;
         else
            ailment[i].name[j] = ch - 48;
      }

      offset += 16;
   }
}


void Ailment::ChangeName(FILE* rom, char* newValue)
{
   // Assumes rom is in write binary mode
   // newValue is 16 characters in length, with the last character being 0.

   fseek(rom, address, SEEK_SET);

   for (int i = 0; i < 16; i++)
   {
      if (newValue[i] == 0)
      {
         fputc(0, rom);
         name[i] = 0;
      }
      else
      {
         fputc(newValue[i] + 48, rom);
         name[i] = newValue[i];
      }
   }
}


void PullBattleMenu(FILE* rom, BattleMenu* bmEntry)
{
   // Assumes rom is in read binary mode
   // bmEntry must already be allocated with at least 11 entries

   char ch;
   int  offset = 0;

   fseek(rom, 0x04a1e1, SEEK_SET);

   for (int i = 0; i < 11; i++)
   {
      bmEntry[i].address = 0x04a1e1 + offset;

      for (int j = 0; j < 16; j++)
      {
         ch = fgetc(rom);

         if (ch == 0)
            bmEntry[i].name[j] = 0;
         else
            bmEntry[i].name[j] = ch - 48;
      }

      offset += 16;
   }
}


void BattleMenu::ChangeName(FILE* rom, char* newValue)
{
   // Assumes rom is in write binary mode
   // newValue is 16 characters in length, with the last character being 0.

   fseek(rom, address, SEEK_SET);

   for (int i = 0; i < 16; i++)
   {
      if (newValue[i] == 0)
      {
         fputc(0, rom);
         name[i] = 0;
      }
      else
      {
         fputc(newValue[i] + 48, rom);
         name[i] = newValue[i];
      }
   }
}


int CreateIPS(FILE* rom1, FILE* rom2, char* targetName, int* currByte)
{
   FILE* target;
   char  ch1;
   char  ch2;
   int   loc = 0;
   int   totalDiff = 0;

   // Now, let's create the IPS file
   target = fopen(targetName, "wb");

   fputc('P', target);
   fputc('A', target);
   fputc('T', target);
   fputc('C', target);
   fputc('H', target);

   fseek(rom1, 0, SEEK_SET);
   fseek(rom2, 0, SEEK_SET);

   *currByte = loc;

   while (!feof(rom1) && !feof(rom2))
   {
      ch1 = fgetc(rom1);
      ch2 = fgetc(rom2);

      if (ch1 != ch2)
      {
         fputc((loc >> 16) & 255, target);
         fputc((loc >> 8) & 255, target);
         fputc(loc & 255, target);

         fputc(0, target);
         fputc(1, target);

         fputc(ch1, target);
      }

      loc++;
      *currByte=loc;
   }

   fputc('E', target);
   fputc('O', target);
   fputc('F', target);

   fclose(target);

   return 0;
}



void PossessROM(FILE* rom)
{
   // Assumes rom is in write binary mode
   // Marks the rom as possessed.  Make sure you
   // only possess EarthBound or Mother 2 ROMs!!!!!

   fseek(rom, 0x101d0, SEEK_SET);
   fflush(rom);

   fputc('B', rom);
   fputc('o', rom);
   fputc('o', rom);
   fputc('!', rom);
}

void ExorciseROM(FILE* rom)
{
   // Assumes rom is in write binary mode
   // Marks the rom as unpossessed
   // Only run this on EarthBound or Mother 2 ROMS!!!

   fseek(rom, 0x101d0, SEEK_SET);
   fflush(rom);

   fputc(20, rom);
   fputc(20, rom);
   fputc(20, rom);
   fputc(20, rom);
}

int IsPossessed(FILE *rom)
{
   // Assumes rom is in read binary mode
   // Assumes rom is EarthBound or Mother 2 ROM
   // Returns 1 if marked as possessed, 0 if not

   char matchStr[5] = "\0\0\0\0";
   int  returnVal = 0;

   fseek(rom, 0x101d0, SEEK_SET);

   for (int i = 0; i < 4; i++)
      matchStr[i] = fgetc(rom);

   if (!strcmp(matchStr, "Boo!"))
      returnVal = 1;

   return returnVal;
}

