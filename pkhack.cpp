#include <windows.h>
#include <ctl3d.h>
#include <stdio.h>
#include <commctrl.h>
#include "stuff.h"
#include <condefs.h>

//---------------------------------------------------------------------------
#define FILEMENU_LOAD 1
#define FILEMENU_CLOSE 2
#define FILEMENU_EXIT 3
#define HELPMENU_ABOUT 4

#define NUMOFITEMTEMPS 9

struct Enemy
{
   int ChangeTheFlag(FILE*, unsigned char);
   int ChangeName(FILE*, char*);
   int ChangeInsidePic(FILE*, unsigned char);
   int ChangeOutsidePic(FILE*, int);
   int ChangeRunFlag(FILE*, unsigned char);
   int ChangeHp(FILE*, unsigned int);
   int ChangePp(FILE*, unsigned int);
   int ChangeExp(FILE*, unsigned int);
   int ChangeMoney(FILE*, unsigned int);
   int ChangeSpeed(FILE*, unsigned char);
   int ChangePalette(FILE*, unsigned char);
   int ChangeOffense(FILE*, unsigned char);
   int ChangeDefense(FILE*, unsigned char);
   int ChangeItem(FILE*, unsigned char);
   int ChangeFreq(FILE*, unsigned char);
   int ChangeStatus(FILE*, unsigned char);
   int ChangeAction(FILE*, int, int);

   long          address;
   unsigned char theFlag;
   char          name[25];
   unsigned char insidePic;
   int           outsidePic;
   unsigned char runFlag;
   unsigned int  hp;
   unsigned int  pp;
   unsigned int  exp;
   unsigned int  money;
   unsigned char speed;
   long          startAddress;
   long          dieAddress;
   unsigned char palette;
   unsigned char offense;
   unsigned char defense;
   unsigned char item;
   unsigned char freq;
   unsigned char status;
   //new stuff
   int           action[4];

   //not-usable
   unsigned char level;
   unsigned char guts;
   unsigned char iq;
   int           finalAction;
   unsigned char meetOutside;
   unsigned char isBoss;
};

struct Item
{
   int ChangeName(FILE*, char*);
   int ChangeType(FILE*, unsigned char);
   int ChangeCost(FILE*, int);
   int ChangeOwnership(FILE*, unsigned char);
   int ChangeEffect(FILE*, int);
   int ChangeStrength(FILE*, char);
   int ChangeIncrease(FILE*, unsigned char);
   int ChangeExtraPower(FILE*, char);
   int ChangeSpecialProperties(FILE*, unsigned char);

   unsigned long          address;
   char          name[25];
   unsigned char type;
   int           cost;
   unsigned char ownership;
   int           effect;
   char          strength;
   unsigned char increase;
   char          extraPower;
   unsigned char specialProperties;
   unsigned long descAddress;
};

//windows variables
HINSTANCE hInst;
WNDCLASS wc;
HWND hWnd;
HMENU filemenu;
HMENU helpmenu;
HMENU menubar;
HFONT fonty;
HFONT bigfonty;
HWND tabs;

 //boxes for enemy properties
 HWND namelist;
   HWND namebox;
   HWND hpbox;
   HWND ppbox;
   HWND expbox;
   HWND moneybox;
   HWND speedbox;
   HWND offbox;
   HWND defbox;
   HWND itembox;
   HWND freqbox;
   HWND statbox;
   HWND iPicbox;
   HWND oPicbox;
   HWND palbox;

   HWND theflagbox;
   HWND runflagbox;
   HWND actbox[4];
   HWND temp[21];

//boxes for item stuff
   HWND item_itemlist;
   HWND item_namebox;
   HWND item_costbox;
   HWND item_typebox;
   HWND item_effectbox;
   HWND item_strbox;
   HWND item_incbox;
   HWND item_extrabox;
   HWND item_specpropbox;
   HWND item_ownereffect[4];
   HWND item_owner[4];
   HWND item_temp[NUMOFITEMTEMPS];

//other
   HWND applybuttn;




//other variables
Enemy enemies[230];
Item items[256];
char filename[1024];
OPENFILENAME ofn;
bool fileloaded=false;
FILE *rom;

//hack procedures
void PullEnemies(FILE* rom, Enemy* enemy);
void PullItems(FILE* rom, Item* item);

//other procedures
LRESULT CALLBACK WindowProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK SplashProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK AboutProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

void quitting(void);
int init(void);
void refreshlist(void);
void item_refreshlist(void);
void setselected(int select);
void item_setselected(int select);
void SetWinCap(unsigned int number, HWND window);
void cleanup(void);


unsigned int junkstuff(HWND window, unsigned int max);
signed int junkstuffsigned(HWND window, signed int max);

void settoenemies(void);
void settoitems(void);

//rom stuff
int applyenemy(void);
void writeenemy(int selected);
int applyitem(void);
void writeitem(int selected);


//---------------------------------------------------------------------------
#pragma argsused
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    fonty=CreateFont(15,0,0,0,200,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
    bigfonty=CreateFont(36,0,0,0,300,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");

    menubar=CreateMenu();
    filemenu=CreateMenu();
    helpmenu=CreateMenu();

    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_LOAD,"Load ROM");
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_CLOSE,"Close ROM");
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_EXIT,"Exit");
    InsertMenu(helpmenu,0xffffffff,MF_BYPOSITION|MF_STRING,HELPMENU_ABOUT,"About");
    InsertMenu(menubar,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)filemenu,"File");
    InsertMenu(menubar,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)helpmenu,"Help");

    hInst=hInstance;
    Ctl3dEnabled();
    Ctl3dRegister(hInstance);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(1));
    wc.hInstance = hInstance;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "EB-hackWindow";
    if(RegisterClass(&wc)==NULL)
    {
     return 1;
    }

    int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-315;
    int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-230;


    hWnd=CreateWindow("EB-hackWindow","PK Hack!",WS_POPUP|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,xpos,ypos,630,460,NULL,menubar,hInstance,NULL);
    InitCommonControls();
    tabs=CreateWindowEx(0,WC_TABCONTROL,"Tabs",WS_CHILD|WS_VISIBLE,5,5,615,409,hWnd,NULL,hInstance,NULL);

   //THE ENEMY PAGE STUFF
    applybuttn=CreateWindow("BUTTON","Apply",WS_CHILD|WS_VISIBLE|WS_BORDER,540,350,70,35,hWnd,NULL,hInstance,NULL);
    SendMessage(applybuttn,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    CreateWindow("STATIC","Bevel1",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,0,1500,2,hWnd,NULL,hInstance,NULL);
    namelist=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,85,35,210,140,hWnd,NULL,hInstance,NULL);
     temp[0]=CreateWindow("STATIC","Names:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,38,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    hpbox=CreateWindow("EDIT","",WS_CHILD|ES_NUMBER|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,115,65,65,19,hWnd,NULL,hInstance,NULL);
     temp[1]=CreateWindow("STATIC","HP:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,65,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    ppbox=CreateWindow("EDIT","",WS_CHILD|ES_NUMBER|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,115,90,65,19,hWnd,NULL,hInstance,NULL);
     temp[2]=CreateWindow("STATIC","PP:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,90,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    expbox=CreateWindow("EDIT","",WS_CHILD|ES_NUMBER|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,115,115,65,19,hWnd,NULL,hInstance,NULL);
     temp[3]=CreateWindow("STATIC","EXP:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,115,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    moneybox=CreateWindow("EDIT","",WS_CHILD|ES_NUMBER|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,115,140,65,19,hWnd,NULL,hInstance,NULL);
     temp[4]=CreateWindow("STATIC","Money:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,140,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[4],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    speedbox=CreateWindow("EDIT","",WS_CHILD|ES_NUMBER|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,115,165,65,19,hWnd,NULL,hInstance,NULL);
     temp[5]=CreateWindow("STATIC","Speed:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,165,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[5],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));       
    offbox=CreateWindow("EDIT","",WS_CHILD|ES_NUMBER|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,115,190,65,19,hWnd,NULL,hInstance,NULL);
     temp[6]=CreateWindow("STATIC","Offense:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,190,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[6],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    defbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,115,215,65,19,hWnd,NULL,hInstance,NULL);
     temp[7]=CreateWindow("STATIC","Defense:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,215,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[7],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    itembox=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,115,264,165,110,hWnd,NULL,hInstance,NULL);
     temp[8]=CreateWindow("STATIC","Item:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,265,100,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[8],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    freqbox=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,115,289,85,90,hWnd,NULL,hInstance,NULL);
     temp[9]=CreateWindow("STATIC","Item %:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,290,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[9],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    statbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,115,240,65,19,hWnd,NULL,hInstance,NULL);
     temp[10]=CreateWindow("STATIC","Status:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,15,240,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[10],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    namebox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_BORDER,375,37,175,19,hWnd,NULL,hInstance,NULL);
     temp[11]=CreateWindow("STATIC","Name:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,315,37,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[11],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    iPicbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,315,65,75,19,hWnd,NULL,hInstance,NULL);
     temp[12]=CreateWindow("STATIC","Inside Pic:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,205,65,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[12],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    oPicbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,315,90,75,19,hWnd,NULL,hInstance,NULL);
     temp[13]=CreateWindow("STATIC","Outside Pic:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,205,90,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[13],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    palbox=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,315,114,100,90,hWnd,NULL,hInstance,NULL);
//    palbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,315,115,75,19,hWnd,NULL,hInstance,NULL);
     temp[14]=CreateWindow("STATIC","Palette:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,205,115,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[14],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    theflagbox=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,314,140,75,60,hWnd,NULL,hInstance,NULL);
//    theflagbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,315,140,75,19,hWnd,NULL,hInstance,NULL);
     temp[15]=CreateWindow("STATIC","TheFlag:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,205,140,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[15],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    runflagbox=CreateWindow("EDIT","",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_AUTOHSCROLL|WS_BORDER,315,165,75,19,hWnd,NULL,hInstance,NULL);
     temp[16]=CreateWindow("STATIC","RunFlag:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,205,165,110,19,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[16],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

    actbox[0]=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,60,340,210,140,hWnd,NULL,hInstance,NULL);
     temp[17]=CreateWindow("STATIC","Action 1:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,10,343,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[17],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(actbox[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    actbox[1]=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,60,365,210,140,hWnd,NULL,hInstance,NULL);
     temp[18]=CreateWindow("STATIC","Action 2:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,10,368,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[18],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(actbox[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    actbox[2]=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,325,340,210,140,hWnd,NULL,hInstance,NULL);
     temp[19]=CreateWindow("STATIC","Action 3:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,275,343,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[19],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(actbox[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    actbox[3]=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,325,365,210,140,hWnd,NULL,hInstance,NULL);
     temp[20]=CreateWindow("STATIC","Action 4:",WS_CHILD|WS_VISIBLE|SS_SIMPLE,275,368,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(temp[20],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(actbox[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));



   //ITEM PAGE STUFF
    item_itemlist=CreateWindow("COMBOBOX","",WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL,85,35,210,140,hWnd,NULL,hInstance,NULL);
     item_temp[0]=CreateWindow("STATIC","Items:",WS_CHILD|SS_SIMPLE,15,38,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_itemlist,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_namebox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,100,60,150,20,hWnd,NULL,hInstance,NULL);
     item_temp[1]=CreateWindow("STATIC","Name:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,60,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_namebox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_namebox);
    item_costbox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,100,85,80,20,hWnd,NULL,hInstance,NULL);
     item_temp[2]=CreateWindow("STATIC","Cost:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,85,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_costbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_costbox);
    item_typebox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,100,110,80,20,hWnd,NULL,hInstance,NULL);
     item_temp[3]=CreateWindow("STATIC","Type:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,110,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_typebox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_typebox);
    item_effectbox=CreateWindow("COMBOBOX","",WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL,100,135,150,120,hWnd,NULL,hInstance,NULL);
     item_temp[4]=CreateWindow("STATIC","Effect:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,135,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[4],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_effectbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_effectbox);
    item_strbox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,100,160,80,20,hWnd,NULL,hInstance,NULL);
     item_temp[5]=CreateWindow("STATIC","Strength:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,160,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[5],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_strbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_strbox);
    item_incbox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,100,185,80,20,hWnd,NULL,hInstance,NULL);
     item_temp[6]=CreateWindow("STATIC","Increase:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,185,55,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[6],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_incbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_incbox);
    item_extrabox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,100,210,80,20,hWnd,NULL,hInstance,NULL);
     item_temp[7]=CreateWindow("STATIC","Extra Power:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,210,70,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[7],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_extrabox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_extrabox);
    item_specpropbox=CreateWindow("EDIT","",WS_CHILD|WS_BORDER,135,235,80,20,hWnd,NULL,hInstance,NULL);
     item_temp[8]=CreateWindow("STATIC","Special Properites:",WS_CHILD|SS_SIMPLE|ES_AUTOHSCROLL,25,235,105,21,hWnd,NULL,hInstance,NULL);
     SendMessage(item_temp[8],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     SendMessage(item_specpropbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
     Ctl3dSubclassCtl(item_specpropbox);

    item_owner[0]=CreateWindow("BUTTON","Ness",WS_CHILD|BS_AUTOCHECKBOX,35,290,65,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_owner[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_owner[1]=CreateWindow("BUTTON","Paula",WS_CHILD|BS_AUTOCHECKBOX,110,290,65,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_owner[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_owner[2]=CreateWindow("BUTTON","Jeff",WS_CHILD|BS_AUTOCHECKBOX,185,290,65,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_owner[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_owner[3]=CreateWindow("BUTTON","Poo",WS_CHILD|BS_AUTOCHECKBOX,260,290,65,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_owner[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

    item_ownereffect[0]=CreateWindow("BUTTON","Use up",WS_CHILD|BS_AUTOCHECKBOX,35,260,75,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_ownereffect[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_ownereffect[1]=CreateWindow("BUTTON","Permanent1",WS_CHILD|BS_AUTOCHECKBOX,100,260,85,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_ownereffect[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_ownereffect[2]=CreateWindow("BUTTON","Permanent2",WS_CHILD|BS_AUTOCHECKBOX,190,260,85,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_ownereffect[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    item_ownereffect[3]=CreateWindow("BUTTON","Chicken?",WS_CHILD|BS_AUTOCHECKBOX,280,260,75,20,hWnd,NULL,hInstance,NULL);
     SendMessage(item_ownereffect[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

//    item_ownereffect=CreateWindow("COMBOBOX","",WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL,135,315,225,100,hWnd,NULL,hInstance,NULL);
//     SendMessage(item_ownereffect,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));


    //Uh... stuff that does things to the windows
    SendMessage(namebox,EM_SETLIMITTEXT,24,0);
    SendMessage(item_namebox,EM_SETLIMITTEXT,24,0);
    SendMessage(expbox,EM_SETLIMITTEXT,10,0);

    SendMessage(namelist,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(hpbox);
    SendMessage(hpbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(ppbox);
    SendMessage(ppbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(moneybox);
    SendMessage(moneybox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(expbox);
    SendMessage(expbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(speedbox);
    SendMessage(speedbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(offbox);
    SendMessage(offbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(defbox);
    SendMessage(defbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(itembox);
    SendMessage(itembox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(freqbox);
    SendMessage(freqbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(statbox);
    SendMessage(statbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(namebox);
    SendMessage(namebox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(iPicbox);
    SendMessage(iPicbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(oPicbox);
    SendMessage(oPicbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(palbox);
    SendMessage(palbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(theflagbox);
    SendMessage(theflagbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(runflagbox);
    SendMessage(runflagbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

    DialogBox(hInstance,MAKEINTRESOURCE(10000),hWnd,(DLGPROC)SplashProc);

    ShowWindow(hWnd,nCmdShow);

    MSG msg;
    init();
   while(GetMessage(&msg,NULL,0,0))
   {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
   }
}

void quitting(void)
{
 DeleteObject(fonty);
 DeleteObject(bigfonty);
 if(fileloaded)fclose(rom);
 fileloaded=false;
 PostQuitMessage(0);
}

int init(void)
{
    TC_ITEM tc;
    tc.pszText="Enemies";
    tc.mask=TCIF_TEXT;
    TabCtrl_InsertItem(tabs,0,&tc);
    tc.pszText="Items";
    TabCtrl_InsertItem(tabs,1,&tc);



    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner=hWnd;
    ofn.hInstance=hInst;
    ofn.lpstrFilter="SNES ROM\0*.smc\0All files\0*.*\0";
    ofn.lpstrCustomFilter=NULL;
    ofn.nMaxCustFilter=NULL;
    ofn.nFilterIndex=1;
    ofn.lpstrFile=filename;
    filename[0]='\0';
    ofn.nMaxFile=1024;
    ofn.lpstrFileTitle=NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir=NULL;
    ofn.lpstrTitle=NULL;
    ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
    ofn.lpstrDefExt="smc";

    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"1/128");
    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"2/128");
    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"4/128");
    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"16/128");
    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"32/128");
    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"64/128");
    SendMessage(freqbox,CB_ADDSTRING,0,(LPARAM)"128/128");

    SendMessage(theflagbox,CB_ADDSTRING,0,(LPARAM)"No");
    SendMessage(theflagbox,CB_ADDSTRING,0,(LPARAM)"Yes");

 for(int m=0;m<4;m++)
 {
  for(int n=0;n<319;n++)
  {
   SendMessage(actbox[m],CB_ADDSTRING,0,(LPARAM)action[n]);
  }
 }
 
 for(int y=0;y<32;y++)
 {
  char buf[25];
  itoa(y,buf,10);
  SendMessage(palbox,CB_ADDSTRING,0,(LPARAM)buf);
 }

 for(int z=0;z<319;z++)
 {
  SendMessage(item_effectbox,CB_ADDSTRING,0,(LPARAM)action[z]);
 }

}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;
    if((hdr->code==TCN_SELCHANGE)&&(hdr->hwndFrom==tabs))
    {
     int tabsel=TabCtrl_GetCurSel(tabs);
     if(tabsel==0)
     {
      settoenemies();
     }
     if(tabsel==1)
     {
      settoitems();
     }
    }
   break;
  }

  case WM_COMMAND:
  {
   if(HIWORD(wParam)==BN_CLICKED)
   {
    if(lParam==(LPARAM)applybuttn)
    {
     int tabsel=TabCtrl_GetCurSel(tabs);
     if(tabsel==0)  //if on enemies
     {
          if(fileloaded)applyenemy();
     }
     if(tabsel==1)  //if on items
     {
          if(fileloaded)applyitem();
     }
    }
   }

   if(HIWORD(wParam)==CBN_SELCHANGE)
   {
    if(lParam==(LPARAM)item_itemlist)
    {
     int selected=SendMessage(item_itemlist,CB_GETCURSEL,0,0);
     if(selected>-1)
     {
     item_setselected(selected);
     }
    }

    if(lParam==(LPARAM)namelist)
    {
     int selected=SendMessage(namelist,CB_GETCURSEL,0,0);
     if(selected>-1)
     {
      setselected(selected);
     }
    }
   }

   if(HIWORD(wParam)==0)
   {
    if(LOWORD(wParam)==FILEMENU_CLOSE)
    {
     if(fileloaded)fclose(rom);
     fileloaded=false;
     cleanup();
    }

    if(LOWORD(wParam)==FILEMENU_LOAD)
    {
     if(GetOpenFileName(&ofn))
     {
      if(fileloaded){fclose(rom);fileloaded=false;cleanup();}
      rom=fopen(filename,"r+b");
      if(!rom){break;}
      else
      {
       PullEnemies(rom,enemies);
       PullItems(rom,items);
       refreshlist();
       item_refreshlist();
       setselected(0);
       item_setselected(0);
       fileloaded=true;
      }
     }
    }
    if(LOWORD(wParam)==FILEMENU_EXIT)
    {
     quitting();
    }
    if(LOWORD(wParam)==HELPMENU_ABOUT)
    {
     DialogBox(hInst,MAKEINTRESOURCE(10001),hWnd,(DLGPROC)AboutProc);
    }
   }
   break;
  }
  case WM_CLOSE:
  {
   quitting();
   break;
  }
  default: return DefWindowProc(hwnd,wMsg,wParam,lParam);
 }
}

void refreshlist(void)
{
 char joe[45];
 joe[0]='[';
 joe[1]='\0';
 char buf[15];

 for(int n=0;n<230;n++)
 {
  joe[0]='[';
  joe[1]='\0';
  itoa(n,buf,10);
  strcat(joe,buf);
  strcat(joe,"] - ");
  strcat(joe,enemies[n].name);
  SendMessage(namelist,CB_ADDSTRING,0,(LPARAM)&joe);
 }
 SendMessage(namelist,CB_SETCURSEL,0,0);

 for(int j=0;j<256;j++)
 {
  joe[0]='[';
  joe[1]='\0';
  itoa(j,buf,10);
  strcat(joe,buf);
  strcat(joe,"] - ");
  strcat(joe,items[j].name);
  SendMessage(itembox,CB_ADDSTRING,0,(LPARAM)&joe);
 }
}

void item_refreshlist(void)
{
 char joe[45];
 joe[0]='[';
 joe[1]='\0';
 char buf[15];
 for(int j=0;j<256;j++)
 {
  joe[0]='[';
  joe[1]='\0';
  itoa(j,buf,10);
  strcat(joe,buf);
  strcat(joe,"] - ");
  strcat(joe,items[j].name);
  SendMessage(item_itemlist,CB_ADDSTRING,0,(LPARAM)&joe);
 }
 SendMessage(item_itemlist,CB_SETCURSEL,0,0);
}


void PullEnemies(FILE* rom, struct Enemy* enemy)
{
   long int      offset = 0;
   int           a;
   int           b;
   unsigned char ch;

   fseek(rom, 1415143, SEEK_SET);
   fflush(rom);

   for (a = 0; a < 230; a++)
   {
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
      ch = fgetc(rom);  // Unknown, varies from 0-3 or so

      enemy[a].insidePic = fgetc(rom);

      ch = fgetc(rom);  // Appears to always be 0...

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

      enemy[a].speed = fgetc(rom);

      ch = fgetc(rom); // Appears to always be 0

      enemy[a].startAddress = fgetc(rom);
      enemy[a].startAddress += (fgetc(rom) * 256);
      enemy[a].startAddress += (fgetc(rom) * 256 * 256);

      ch = fgetc(rom);  // Appears to always be 0

      enemy[a].dieAddress = fgetc(rom);
      enemy[a].dieAddress += (fgetc(rom) * 256);
      enemy[a].dieAddress += (fgetc(rom) * 256 * 256);

      ch = fgetc(rom);  // Appears to always be 0

      enemy[a].palette = fgetc(rom);

      enemy[a].level = fgetc(rom);

      ch = fgetc(rom);  // Unknown value, but varies greatly

      enemy[a].offense = fgetc(rom);

      ch = fgetc(rom); // Appears to always be 0

      enemy[a].defense = fgetc(rom);

      ch = fgetc(rom);  // Unknown, but mostly 0, sometimes 1

      ch = fgetc(rom);  // Vitality or something

      enemy[a].guts = fgetc(rom);

      enemy[a].iq = fgetc(rom);

      ch = fgetc(rom);  // Appears to vary from 0-4 or so
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3
      ch = fgetc(rom);  // Appears to vary from 0-3

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

      ch = fgetc(rom);  // Varies greatly, maybe strengths against psi?
      ch = fgetc(rom);  // Varies greatly
      ch = fgetc(rom);  // Varies greatly
      ch = fgetc(rom);  // Varies greatly, looks like weaknesses or something
      ch = fgetc(rom);  // dunno, always 0, except for Carbon Dog, who has 83
      ch = fgetc(rom);  // dunno, but varies from 0-3

      enemy[a].meetOutside = fgetc(rom);

      enemy[a].freq = fgetc(rom);
      enemy[a].item = fgetc(rom);

      enemy[a].status = fgetc(rom);

      enemy[a].meetOutside = fgetc(rom);
      enemy[a].isBoss = fgetc(rom);

      ch = fgetc(rom);  // Has to do with calling enemies
      ch = fgetc(rom);  // Unknown, numbers vary somewhat

      offset += 94;
   }
}


void setselected(int select)
{
 SetWinCap(enemies[select].hp,hpbox);
 SetWinCap(enemies[select].pp,ppbox);
 SetWinCap(enemies[select].exp,expbox);
 SetWinCap(enemies[select].money,moneybox);
 SetWinCap(enemies[select].speed,speedbox);
 SetWinCap(enemies[select].offense,offbox);
 SetWinCap(enemies[select].defense,defbox);
// SetWinCap(enemies[select].item,itembox);
// SetWinCap(enemies[select].freq,freqbox);
 SendMessage(itembox,CB_SETCURSEL,enemies[select].item,0);
 SendMessage(freqbox,CB_SETCURSEL,enemies[select].freq-1,0);
for(int n=0;n<4;n++)
{
 SendMessage(actbox[n],CB_SETCURSEL,enemies[select].action[n],0);
}
 SetWinCap(enemies[select].status,statbox);
 SetWinCap(enemies[select].insidePic,iPicbox);
 SetWinCap(enemies[select].outsidePic,oPicbox);
// SetWinCap(enemies[select].palette,palbox);
 SendMessage(palbox,CB_SETCURSEL,enemies[select].palette,0);

 SetWindowText(namebox,enemies[select].name);

// SetWinCap(enemies[select].theFlag,theflagbox);
 if(enemies[select].theFlag<1)SendMessage(theflagbox,CB_SETCURSEL,0,0);
 if(enemies[select].theFlag>0)SendMessage(theflagbox,CB_SETCURSEL,1,0);
 SetWinCap(enemies[select].runFlag,runflagbox);
}

void item_setselected(int select)
{
 SetWindowText(item_namebox,items[select].name);
 SetWinCap(items[select].cost,item_costbox);
 SetWinCap(items[select].type,item_typebox);
 SetWinCap(items[select].effect,item_effectbox);
 SendMessage(item_effectbox,CB_SETCURSEL,items[select].effect,0);

// SetWinCap(items[select].strength,item_strbox);
 char buf[15];
 itoa(items[select].strength,buf,10);
 SetWindowText(item_strbox,buf);

 SetWinCap(items[select].increase,item_incbox);

 // SetWinCap(items[select].extraPower,item_extrabox);
 itoa(items[select].extraPower,buf,10);
 SetWindowText(item_extrabox,buf);

 SetWinCap(items[select].specialProperties,item_specpropbox);
 if((items[select].ownership>>4)&(1)){SendMessage(item_ownereffect[3],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_ownereffect[3],BM_SETCHECK,BST_UNCHECKED,0);}
 if((items[select].ownership>>4)&(2)){SendMessage(item_ownereffect[2],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_ownereffect[2],BM_SETCHECK,BST_UNCHECKED,0);}
 if((items[select].ownership>>4)&(4)){SendMessage(item_ownereffect[1],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_ownereffect[1],BM_SETCHECK,BST_UNCHECKED,0);}
 if((items[select].ownership>>4)&(8)){SendMessage(item_ownereffect[0],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_ownereffect[0],BM_SETCHECK,BST_UNCHECKED,0);}

 if(items[select].ownership&1){SendMessage(item_owner[0],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_owner[0],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&2){SendMessage(item_owner[1],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_owner[1],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&4){SendMessage(item_owner[2],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_owner[2],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&8){SendMessage(item_owner[3],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_owner[3],BM_SETCHECK,BST_UNCHECKED,0);}
}

void SetWinCap(unsigned int number, HWND window)
{
 char buf[15];
 ultoa(number,buf,10);
 SetWindowText(window,buf);

}


void cleanup(void)
{
 char *temp="";
     SendMessage(namelist,CB_RESETCONTENT,0,0);
     SendMessage(itembox,CB_RESETCONTENT,0,0);
     SendMessage(item_itemlist,CB_RESETCONTENT,0,0);

     SendMessage(itembox,CB_SETCURSEL,-1,0);
     SendMessage(freqbox,CB_SETCURSEL,-1,0);
     SendMessage(actbox[0],CB_SETCURSEL,-1,0);
     SendMessage(actbox[1],CB_SETCURSEL,-1,0);
     SendMessage(actbox[2],CB_SETCURSEL,-1,0);
     SendMessage(actbox[3],CB_SETCURSEL,-1,0);

     SetWindowText(hpbox,temp);
     SetWindowText(ppbox,temp);
     SetWindowText(expbox,temp);
     SetWindowText(moneybox,temp);
     SetWindowText(speedbox,temp);
     SetWindowText(offbox,temp);
     SetWindowText(defbox,temp);
     SetWindowText(namebox,temp);
//     SetWindowText(itembox,temp);
     SetWindowText(statbox,temp);
     SetWindowText(iPicbox,temp);
     SetWindowText(oPicbox,temp);
//     SetWindowText(palbox,temp);
     SendMessage(palbox,CB_SETCURSEL,-1,0);
//     SetWindowText(theflagbox,temp);
     SendMessage(theflagbox,CB_SETCURSEL,-1,0);
     SetWindowText(runflagbox,temp);

     //item cleanup
     SetWindowText(item_namebox,temp);
     SetWindowText(item_costbox,temp);
     SetWindowText(item_typebox,temp);
     SendMessage(item_effectbox,CB_SETCURSEL,-1,0);
     SetWindowText(item_strbox,temp);
     SetWindowText(item_incbox,temp);
     SetWindowText(item_extrabox,temp);
     SetWindowText(item_specpropbox,temp);
//     SendMessage(item_ownereffect,CB_SETCURSEL,-1,0);
     SendMessage(item_ownereffect[0],BM_SETCHECK,BST_UNCHECKED,0);
     SendMessage(item_ownereffect[1],BM_SETCHECK,BST_UNCHECKED,0);
     SendMessage(item_ownereffect[2],BM_SETCHECK,BST_UNCHECKED,0);
     SendMessage(item_ownereffect[3],BM_SETCHECK,BST_UNCHECKED,0);

     SendMessage(item_owner[0],BM_SETCHECK,BST_UNCHECKED,0);
     SendMessage(item_owner[1],BM_SETCHECK,BST_UNCHECKED,0);
     SendMessage(item_owner[2],BM_SETCHECK,BST_UNCHECKED,0);
     SendMessage(item_owner[3],BM_SETCHECK,BST_UNCHECKED,0);
}

int applyenemy(void)
{
 int selected=SendMessage(namelist,CB_GETCURSEL,0,0);
 if(selected>-1)
 {
  enemies[selected].hp=junkstuff(hpbox,65535);
  enemies[selected].pp=junkstuff(ppbox,65535);
  enemies[selected].exp=junkstuff(expbox,4294967295);
  enemies[selected].money=junkstuff(moneybox,65535);
  enemies[selected].speed=junkstuff(speedbox,255);
  enemies[selected].offense=junkstuff(offbox,255);
  enemies[selected].defense=junkstuff(defbox,255);
  enemies[selected].item=SendMessage(itembox,CB_GETCURSEL,0,0);
   if(enemies[selected].item<0)enemies[selected].item=0;
  enemies[selected].freq=SendMessage(freqbox,CB_GETCURSEL,0,0)+1;
   if(enemies[selected].freq<0)enemies[selected].freq=0;
  enemies[selected].status=junkstuff(statbox,255);
  enemies[selected].insidePic=junkstuff(iPicbox,255);
  enemies[selected].outsidePic=junkstuff(oPicbox,65535);
//  enemies[selected].palette=junkstuff(palbox,255);
  enemies[selected].palette=SendMessage(palbox,CB_GETCURSEL,0,0);
//   GetWindowText(namebox,enemies[selected].name,25);
//   SetWindowText(hWnd,enemies[selected].name);
//  enemies[selected].theFlag=junkstuff(theflagbox,1);
  enemies[selected].theFlag=SendMessage(theflagbox,CB_GETCURSEL,0,0);
  enemies[selected].runFlag=junkstuff(runflagbox,255);

for(int i=0;i<4;i++)
{
  enemies[selected].action[i]=SendMessage(actbox[i],CB_GETCURSEL,0,0);
  if(enemies[selected].action[i]<0)enemies[selected].action[i]=0;
}

  writeenemy(selected);

  SendMessage(namelist,CB_RESETCONTENT,0,0);
  SendMessage(itembox,CB_RESETCONTENT,0,0);
  SendMessage(item_itemlist,CB_RESETCONTENT,0,0);
  refreshlist();
  item_refreshlist();
  setselected(selected);
  SendMessage(namelist,CB_SETCURSEL,selected,0);
 }
}

int applyitem(void)
{
 int selected=SendMessage(item_itemlist,CB_GETCURSEL,0,0);
 if(selected>-1)
 {
  GetWindowText(item_namebox,items[selected].name,25);
  items[selected].type=junkstuff(item_typebox,255);
  items[selected].cost=junkstuff(item_costbox,65535);
  items[selected].effect=SendMessage(item_effectbox,CB_GETCURSEL,0,0);
   if(items[selected].effect<0)items[selected].effect=0;

//  items[selected].ownership=(SendMessage(item_ownereffect,CB_GETCURSEL,0,0)<<4)&240;
  items[selected].ownership=0;
  if(SendMessage(item_owner[0],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=1;}
  if(SendMessage(item_owner[1],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=2;}
  if(SendMessage(item_owner[2],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=4;}
  if(SendMessage(item_owner[3],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=8;}

  if(SendMessage(item_ownereffect[0],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=128;}
  if(SendMessage(item_ownereffect[1],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=64;}
  if(SendMessage(item_ownereffect[2],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=32;}
  if(SendMessage(item_ownereffect[3],BM_GETCHECK,0,0)==BST_CHECKED){items[selected].ownership|=16;}


  items[selected].strength=junkstuffsigned(item_strbox,255);
  items[selected].increase=junkstuff(item_incbox,255);
  items[selected].extraPower=junkstuffsigned(item_extrabox,255);
  items[selected].specialProperties=junkstuff(item_specpropbox,255);


  writeitem(selected);

  SendMessage(namelist,CB_RESETCONTENT,0,0);
  SendMessage(itembox,CB_RESETCONTENT,0,0);
  SendMessage(item_itemlist,CB_RESETCONTENT,0,0);
  refreshlist();
  item_refreshlist();
  item_setselected(selected);
  SendMessage(item_itemlist,CB_SETCURSEL,selected,0);
 }
}

void writeitem(int selected)
{
 items[selected].ChangeName(rom, items[selected].name);
 items[selected].ChangeType(rom, items[selected].type);
 items[selected].ChangeCost(rom, items[selected].cost);
 items[selected].ChangeOwnership(rom, items[selected].ownership);
 items[selected].ChangeEffect(rom, items[selected].effect);
 items[selected].ChangeStrength(rom, items[selected].strength);
 items[selected].ChangeIncrease(rom, items[selected].increase);
 items[selected].ChangeExtraPower(rom, items[selected].extraPower);
 items[selected].ChangeSpecialProperties(rom, items[selected].specialProperties);
}


unsigned int junkstuff(HWND window, unsigned int max)
{
 char text[25];
 long double temp=0;
 GetWindowText(window,text,25);
 temp=_strtold(text,NULL);
 if(temp>max){return max;}
 return temp;
}

signed int junkstuffsigned(HWND window, signed int max)
{
 char text[25];
 signed int temp=0;
 GetWindowText(window,text,25);
 temp=atoi(text);
 if(temp>max){return max;}
 return temp;
}

void writeenemy(int selected)
{
enemies[selected].ChangeTheFlag(rom, enemies[selected].theFlag);

char temp[25];
GetWindowText(namebox,temp,25);
enemies[selected].ChangeName(rom, temp);

enemies[selected].ChangeInsidePic(rom, enemies[selected].insidePic);
enemies[selected].ChangeOutsidePic(rom, enemies[selected].outsidePic);
enemies[selected].ChangeRunFlag(rom, enemies[selected].runFlag);
enemies[selected].ChangeHp(rom, enemies[selected].hp);
enemies[selected].ChangePp(rom, enemies[selected].pp);
enemies[selected].ChangeExp(rom, enemies[selected].exp);
enemies[selected].ChangeMoney(rom, enemies[selected].money);
enemies[selected].ChangeSpeed(rom, enemies[selected].speed);
enemies[selected].ChangePalette(rom, enemies[selected].palette);
enemies[selected].ChangeOffense(rom, enemies[selected].offense);
enemies[selected].ChangeDefense(rom, enemies[selected].defense);
enemies[selected].ChangeItem(rom, enemies[selected].item);
enemies[selected].ChangeFreq(rom, enemies[selected].freq);
enemies[selected].ChangeStatus(rom, enemies[selected].status);
 for(int n=0;n<4;n++)
 {
  enemies[selected].ChangeAction(rom,enemies[selected].action[n],n);
 }
}





//ROM writing functions
int Enemy::ChangeTheFlag(FILE* rom, unsigned char newValue)
{
   // rom must already be open, and in write mode
   // is assumed newValue is 0 or 1
   // returns 1 on success

   fseek(rom, address, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   theFlag = newValue;

   return 1;
}

int Enemy::ChangeName(FILE* rom, char* newName)
{
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

   return 1;
}

int Enemy::ChangeInsidePic(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 28, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   insidePic = newValue;

   return 1;
}


int Enemy::ChangeOutsidePic(FILE* rom, int newValue)
{
   fseek(rom, address + 30, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);


   outsidePic = newValue;

   return 1;
}

int Enemy::ChangeRunFlag(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 32, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   runFlag = newValue;

   return 1;
}

int Enemy::ChangeHp(FILE* rom, unsigned int newValue)
{
   // assumed integer is from 0 to 65535

   fseek(rom, address + 33, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   hp = newValue;

   return 1;
}

int Enemy::ChangePp(FILE* rom, unsigned int newValue)
{
   // assumed integer is from 0 to 65535

   fseek(rom, address + 35, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   pp = newValue;

   return 1;
}

int Enemy::ChangeExp(FILE* rom, unsigned int newValue)
{
   // assumed integer is from 0 to 2^32

   fseek(rom, address + 37, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255 , rom);
   fputc((newValue >> 16) & 255, rom);
   fputc((newValue >> 24) & 255, rom);

   exp = newValue;

   return 1;
}

int Enemy::ChangeMoney(FILE* rom, unsigned int newValue)
{
   // assumed integer is from 0 to 65535

   fseek(rom, address + 41, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   money = newValue;

   return 1;
}

int Enemy::ChangeSpeed(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 43, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   speed = newValue;

   return 1;
}

int Enemy::ChangePalette(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 53, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   palette = newValue;

   return 1;
}

int Enemy::ChangeOffense(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 56, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   offense = newValue;

   return 1;
}

int Enemy::ChangeDefense(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 58, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   defense = newValue;

   return 1;
}

int Enemy::ChangeItem(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 88, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   item = newValue;

   return 1;
}

int Enemy::ChangeFreq(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 87, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   freq = newValue;

   return 1;
}

int Enemy::ChangeStatus(FILE* rom, unsigned char newValue)
{
   fseek(rom, address + 89, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   status = newValue;

   return 1;
}


int Enemy::ChangeAction(FILE* rom, int newValue, int index)
{

   fseek(rom, address + 70 + (index * 2), SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   action[index] = newValue;

   return 1;
}



void settoenemies(void)
{
//hide all other windows
 ShowWindow(item_itemlist,SW_HIDE);
 ShowWindow(item_namebox,SW_HIDE);
 ShowWindow(item_costbox,SW_HIDE);
 ShowWindow(item_typebox,SW_HIDE);
 ShowWindow(item_effectbox,SW_HIDE);
 ShowWindow(item_strbox,SW_HIDE);
 ShowWindow(item_incbox,SW_HIDE);
 ShowWindow(item_extrabox,SW_HIDE);
 ShowWindow(item_specpropbox,SW_HIDE);
 ShowWindow(item_owner[0],SW_HIDE);
 ShowWindow(item_owner[1],SW_HIDE);
 ShowWindow(item_owner[2],SW_HIDE);
 ShowWindow(item_owner[3],SW_HIDE);
 ShowWindow(item_ownereffect[0],SW_HIDE);
 ShowWindow(item_ownereffect[1],SW_HIDE);
 ShowWindow(item_ownereffect[2],SW_HIDE);
 ShowWindow(item_ownereffect[3],SW_HIDE);

 for(int m=0;m<NUMOFITEMTEMPS;m++)
 {
  ShowWindow(item_temp[m],SW_HIDE);
 }


//show enemy windows
 ShowWindow(namelist,SW_SHOW);
 ShowWindow(namebox,SW_SHOW);
 ShowWindow(hpbox,SW_SHOW);
 ShowWindow(ppbox,SW_SHOW);
 ShowWindow(expbox,SW_SHOW);
 ShowWindow(moneybox,SW_SHOW);
 ShowWindow(speedbox,SW_SHOW);
 ShowWindow(offbox,SW_SHOW);
 ShowWindow(defbox,SW_SHOW);
 ShowWindow(itembox,SW_SHOW);
 ShowWindow(freqbox,SW_SHOW);
 ShowWindow(statbox,SW_SHOW);
 ShowWindow(oPicbox,SW_SHOW);
 ShowWindow(iPicbox,SW_SHOW);
 ShowWindow(palbox,SW_SHOW);
 ShowWindow(theflagbox,SW_SHOW);
 ShowWindow(runflagbox,SW_SHOW);
 ShowWindow(actbox[0],SW_SHOW);
 ShowWindow(actbox[1],SW_SHOW);
 ShowWindow(actbox[2],SW_SHOW);
 ShowWindow(actbox[3],SW_SHOW);

 for(int n=0;n<21;n++)
 {
  ShowWindow(temp[n],SW_SHOW);
 }
}

void settoitems(void)
{
//hide all other windows
 ShowWindow(namelist,SW_HIDE);
 ShowWindow(namebox,SW_HIDE);
 ShowWindow(hpbox,SW_HIDE);
 ShowWindow(ppbox,SW_HIDE);
 ShowWindow(expbox,SW_HIDE);
 ShowWindow(moneybox,SW_HIDE);
 ShowWindow(speedbox,SW_HIDE);
 ShowWindow(offbox,SW_HIDE);
 ShowWindow(defbox,SW_HIDE);
 ShowWindow(itembox,SW_HIDE);
 ShowWindow(freqbox,SW_HIDE);
 ShowWindow(statbox,SW_HIDE);
 ShowWindow(oPicbox,SW_HIDE);
 ShowWindow(iPicbox,SW_HIDE);
 ShowWindow(palbox,SW_HIDE);
 ShowWindow(theflagbox,SW_HIDE);
 ShowWindow(runflagbox,SW_HIDE);
 ShowWindow(actbox[0],SW_HIDE);
 ShowWindow(actbox[1],SW_HIDE);
 ShowWindow(actbox[2],SW_HIDE);
 ShowWindow(actbox[3],SW_HIDE);

 for(int n=0;n<21;n++)
 {
  ShowWindow(temp[n],SW_HIDE);
 }

 //show items
 ShowWindow(item_itemlist,SW_SHOW);
 ShowWindow(item_namebox,SW_SHOW);
 ShowWindow(item_costbox,SW_SHOW);
 ShowWindow(item_typebox,SW_SHOW);
 ShowWindow(item_effectbox,SW_SHOW);
 ShowWindow(item_strbox,SW_SHOW);
 ShowWindow(item_incbox,SW_SHOW);
 ShowWindow(item_extrabox,SW_SHOW);
 ShowWindow(item_specpropbox,SW_SHOW);
 ShowWindow(item_owner[0],SW_SHOW);
 ShowWindow(item_owner[1],SW_SHOW);
 ShowWindow(item_owner[2],SW_SHOW);
 ShowWindow(item_owner[3],SW_SHOW);
 ShowWindow(item_ownereffect[0],SW_SHOW);
 ShowWindow(item_ownereffect[1],SW_SHOW);
 ShowWindow(item_ownereffect[2],SW_SHOW);
 ShowWindow(item_ownereffect[3],SW_SHOW);

 for(int m=0;m<NUMOFITEMTEMPS;m++)
 {
  ShowWindow(item_temp[m],SW_SHOW);
 }
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


int Item::ChangeName(FILE* rom, char* newName)
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

   return 1;
}

int Item::ChangeType(FILE* rom, unsigned char newValue)
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
   // 58  = no idea (bicycle, hawk eye, zombie paper only
   // 59  = important event item
   // 255 = Null

   fseek(rom, address + 25, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   type = newValue;

   return 1;
}

int Item::ChangeCost(FILE* rom, int newValue)
{
   // Assumes rom is in write binary mode
   // newValue must be from 0 to 65535

   fseek(rom, address + 26, SEEK_SET);
   fflush(rom);

   fputc(newValue & 255, rom);
   fputc((newValue >> 8) & 255, rom);

   cost = newValue;

   return 1;
}

int Item::ChangeOwnership(FILE* rom, unsigned char newValue)
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

   return 1;
}

int Item::ChangeEffect(FILE* rom, int newValue)
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

   return 1;
}

int Item::ChangeStrength(FILE* rom, char newValue)
{
   // Assumes rom is in write binary mode
   // Note: Values are from -127 to 127

   fseek(rom, address + 31, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   strength = newValue;

   return 1;
}

int Item::ChangeIncrease(FILE* rom, unsigned char newValue)
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

   fseek(rom, address + 32, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   increase = newValue;

   return 1;
}

int Item::ChangeExtraPower(FILE* rom, char newValue)
{
   // "extraPower" is related to "increase".
   // While "increase" describes what stats will be increased,
   // "extraPower" describes how much that stat will be increased.
   // Like when you equip the Gutsy Bat, your Guts go up 127.
   // Or some things like the Rabbit's Foot increase your luck.
   // Essentially this function tells how much to increase by.

   // Assumes rom is in write binary mode
   // Input values are from -127 to 127.

   fseek(rom, address + 33, SEEK_SET);
   fflush(rom);

   fputc(newValue, rom);

   extraPower = newValue;

   return 1;
}

int Item::ChangeSpecialProperties(FILE* rom, unsigned char newValue)
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

   return 1;
}




//dialog box procs
HBITMAP logo;
bool CALLBACK SplashProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  SetTimer(hwnd,1,3000,NULL);
  logo=LoadBitmap(hInst,MAKEINTRESOURCE(100));
  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-250;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-125;
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,500,250,SWP_SHOWWINDOW);
  PlaySound(MAKEINTRESOURCE(1000),hInst,SND_ASYNC|SND_RESOURCE);
  return true;
  }

  case WM_TIMER:
  {
   KillTimer(hwnd,1);
   DeleteObject(logo);
   EndDialog(hwnd,5);
   return false;
  }

  case WM_PAINT:
  {
   HDC hdc=GetDC(hwnd);
    HDC other=CreateCompatibleDC(hdc);
    SelectObject(other,logo);
    BitBlt(hdc,0,0,500,250,other,0,0,SRCCOPY);
   ReleaseDC(hwnd,hdc);
   DeleteDC(other);
   return false;
  }

  case WM_LBUTTONDOWN:
  {
   DeleteObject(logo);
   EndDialog(hwnd,5);
  }

  default: return false;
 }
}

bool CALLBACK AboutProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  SendMessage(CreateWindow("STATIC","PK Hack, version 0.10",WS_CHILD|WS_VISIBLE|SS_SIMPLE,0,0,400,50,hwnd,NULL,hInst,NULL),WM_SETFONT,(WPARAM)bigfonty,MAKELPARAM(true,0));
  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-250;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-150;
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,500,300,SWP_SHOWWINDOW);
  return true;
  }

  case WM_TIMER:
  {
   KillTimer(hwnd,1);
   DeleteObject(logo);
   EndDialog(hwnd,5);
   return false;
  }

  case WM_CLOSE:
  case WM_LBUTTONDOWN:
  {
   DeleteObject(logo);
   EndDialog(hwnd,5);
  }

  default: return false;
 }
}

