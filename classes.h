#define TYPE_NORMAL 0
#define TYPE_PSI    1
#define TYPE_CALL   2
#define TYPE_ITEM   3               


#include "stdio.h"
#include "string.h"
#include "bgm.h"
#include "effects.h"

//classes
struct Enemy
{
   void ChangeTheFlag(FILE*, unsigned char);
   void ChangeName(FILE*, char*);
   void ChangeInsidePic(FILE*, unsigned char);
   void ChangeOutsidePic(FILE*, int);
   void ChangeRunFlag(FILE*, unsigned char);
   void ChangeHp(FILE*, unsigned int);
   void ChangePp(FILE*, unsigned int);
   void ChangeExp(FILE*, unsigned int);
   void ChangeMoney(FILE*, unsigned int);
   void ChangeSpeed(FILE*, unsigned char);
   void ChangeStartAction(FILE*, unsigned int);
   void ChangeDieAction(FILE*, unsigned int);
   void ChangePalette(FILE*, unsigned char);
   void ChangeOffense(FILE*, unsigned char);
   void ChangeDefense(FILE*, unsigned char);
   void ChangeItem(FILE*, unsigned char);
   void ChangeFreq(FILE*, unsigned char);
   void ChangeStatus(FILE*, unsigned char);
   void ChangeAction(FILE*, int, int);
   void ChangeFinalAction(FILE*, int);
   void ChangeLevel(FILE*, unsigned char);
   void ChangeGuts(FILE*, unsigned char);
   void ChangeIq(FILE*, unsigned char);
   void ChangeMovement(FILE*, unsigned char);
   void ChangeOrder(FILE*, unsigned char);
   void ChangeFriend(FILE*, unsigned char, unsigned char);
   void ChangeMaxCall(FILE*, unsigned char);
   void ChangeMissRate(FILE*, unsigned char);
   void ChangeDieSound(FILE*, unsigned char);
   void ChangeType(FILE*, unsigned char);
   void ChangeMusic(FILE*, unsigned char);

   int  CanCall(void);

   long          address;
   unsigned char theFlag;
   char          name[25];
   unsigned char number;
   unsigned char insidePic;
   int           outsidePic;
   unsigned char runFlag;
   unsigned int  hp;
   unsigned int  pp;
   unsigned int  exp;
   unsigned int  money;
   unsigned char speed;
   unsigned int  startAddress;
   unsigned int  dieAddress;
   unsigned char palette;
   unsigned char offense;
   unsigned char defense;
   unsigned char item;
   unsigned char freq;
   unsigned char status;
   int           action[4];
   int           finalAction;
   unsigned char level;
   unsigned char guts;
   unsigned char iq;
   unsigned char bossFlag;  // still not worked out fully
   unsigned char dieSound;
   unsigned char movement;
   unsigned char order;
   unsigned char amigo[4];
   unsigned char maxCall;
   unsigned char missRate;
   unsigned char type;
   unsigned char music;
   unsigned char unknown;
};

struct Item
{
   void ChangeName(FILE*, char*);
   void ChangeType(FILE*, unsigned char);
   void ChangeCost(FILE*, int);
   void ChangeOwnership(FILE*, unsigned char);
   void ChangeEffect(FILE*, int);
   void ChangeStrength(FILE*, char);
   void ChangeIncrease(FILE*, unsigned char);
   void ChangeExtraPower(FILE*, char);
   void ChangeSpecialProperties(FILE*, unsigned char);
   void ChangeDescAddress(FILE*, unsigned int);

   long          address;
   char          name[25];
   unsigned char number;
   unsigned char type;
   int           cost;
   unsigned char ownership;
   int           effect;
   char          strength;
   unsigned char increase;
   char          extraPower;
   unsigned char specialProperties;
   unsigned int  descAddress;
};


struct Psi
{
   void ChangeName(FILE*, char*);

   long address;
   char name[25];
};

struct Teleport
{
   void ChangeName(FILE*, char*);
   void ChangeX(FILE*, int);
   void ChangeY(FILE*, int);

   long address;
   char name[25];
   int  x;
   int  y;
};


struct Phone
{
   void ChangeName(FILE*, char*);
   void ChangePointer(FILE*, unsigned int);

   long         address;
   char         name[25];
   unsigned int pointer;
};

struct Ailment
{
   void ChangeName(FILE*, char*);

   long address;
   char name[16];
};


struct BattleMenu
{
   void ChangeName(FILE*, char*);

   long address;
   char name[16];
};
// --classes


//functions
void PullEnemies(FILE* rom, struct Enemy* enemy);
void SortEnemiesAlpha(Enemy*);
void SortEnemiesGameOrder(Enemy*);
void SortEnemiesOriginal(Enemy*);
void SwapEnemy(Enemy&, Enemy&);
void DumpEnemies(Enemy*, char*, int, int, int);

void PullItems(FILE* rom, Item* item);
void SortItemsAlpha(Item*);
void SortItemsType(Item*);
void SortItemsOriginal(Item*);
void SwapItem(Item&, Item&);
void DumpItems(Item*, char*, int);

void PullPsi(FILE*, Psi*);

void PullTeleport(FILE*, Teleport*);

void PullPhone(FILE*, Phone*);
void PullAilments(FILE*, Ailment*);
void PullBattleMenu(FILE*, BattleMenu*);

int SearchEnemy(Enemy* enemy, char* searchStr, int results[]);
int SearchItem(Item* item, char* searchStr, int results[]);

int CreateIPS(FILE*, FILE*, char*, int*);
int IsCorrectRom(FILE* rom);
long FileSize(FILE* rom);

void PossessROM(FILE*);
void ExorciseROM(FILE*);
int IsPossessed(FILE*);
//--functions
