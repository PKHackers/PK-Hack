#include <windows.h>
#include <ctl3d.h>
#include <stdio.h>
#include <math.h>
#include <commctrl.h>
#include "classes.h"
#include <condefs.h>
#include "actiontype.h"
#include "psinames.h"
#include "graphics.h"


USERC("pkhack.rc");
USEUNIT("classes.cpp");
USEUNIT("graphics.cpp");
//---------------------------------------------------------------------------
#define FILEMENU_LOAD 1
#define FILEMENU_CLOSE 2
#define FILEMENU_EXIT 3
#define FILEMENU_MAKEIPS 19
#define FILEMENU_POSSESS 21
#define HELPMENU_ABOUT 4
#define HELPMENU_HELP 5
#define EDITMENU_FIND 6
#define EDITMENU_APPLY 7

#define WINDOWMENU_APPLY 11
#define WINDOWMENU_FIND 12
#define WINDOWMENU_HELP 13
#define WINDOWMENU_CLOSEROM 15

#define VIEW_ALPHA 16
#define VIEW_LEVEL 17
#define VIEW_NONE 18
#define VIEW_DUMP 20


Enemy enemies[230];
Item items[256];
Psi psis[16];
Teleport teles[16];
Phone phones[5];
Ailment ails[10];
BattleMenu bmenus[11];

FILE *rom;
int romtype=-1;
unsigned long romsize=0;
bool fileloaded=false;

int selected_enemy=0;
int selected_item=0;
int selected_psi=0;
int selected_tele=0;

//ips make stuff
int doneworking=false;
char romname[MAX_PATH];
char ipsname[MAX_PATH];
DWORD WINAPI IPSMakingThread(LPVOID);
int waiting=0;
int ipsmax=3000000;
bool stepping=false;
HBITMAP spaanr;
HBITMAP spaanstepr;

int enemy_sorting=VIEW_ALPHA;
int item_sorting=VIEW_ALPHA;

char searchstring[25];

HINSTANCE hInst;
WNDCLASS wc;
HWND hWnd;
HMENU filemenu;
HMENU helpmenu;
HMENU menubar;
HFONT smallfonty;
HFONT fonty;
HFONT bigfonty;
HFONT boldfonty;

HBITMAP background;

HICON pkhack_icon;
HICON pile_icon;
HICON fobby_icon;
HICON ufo_icon;
HICON garbage_icon;
HICON closepresent_icon;
HICON openpresent_icon;
HICON butterfly_icon;
HICON light_icon;
HICON diamond_icon;
HICON sign_icon;
HICON scorp_icon;
HICON phone_icon;
HICON wow_icon;
HICON frog_icon;
HICON help_icon;
HICON mail_icon;
HICON tv_icon;
HIMAGELIST iconlist;

HWND tree;
HWND statusbox;
HWND toolbar;
HWND title;
HWND descbox;

HTREEITEM tree_pkhack;
HTREEITEM tree_enemies;
HTREEITEM tree_enemies_stats;
HTREEITEM tree_enemies_acttexts;

HTREEITEM tree_items;
HTREEITEM tree_items_stats;
HTREEITEM tree_items_acttexts;

HTREEITEM tree_PSI;
HTREEITEM tree_PSI_names;
HTREEITEM tree_PSI_teleport;

HTREEITEM tree_text;
HTREEITEM tree_text_phone;
HTREEITEM tree_text_ailments;
HTREEITEM tree_text_battlemenu;

HTREEITEM tree_savestates;
HTREEITEM tree_about;
HTREEITEM tree_help;


HWND applybuttn;
HWND tabs;
HWND window_toolbar;
HMENU enemy_menu;
 HMENU enemy_view_menu;
HMENU item_menu;
 HMENU item_view_menu;
HMENU PSI_menu;
HMENU tele_menu;
HMENU window_stuff_menu;

HWND enemywindow;
//assorted windows for the editing enemies
HWND enemy_stats_box;
HWND enemy_acts_box;
HWND enemy_graphics_box;

   HWND enemy_NameList;

   HWND enemy_TheFlag;
   HWND enemy_Name;
   HWND enemy_InsidePic;

   HWND enemy_selector;

   HWND enemy_Hp;
   HWND enemy_Pp;
   HWND enemy_Exp;
   HWND enemy_Money;
   HWND enemy_Speed;
   HWND enemy_StartAction;
   HWND enemy_DieAction;
   HWND enemy_Palette;
   HWND enemy_Offense;
   HWND enemy_Defense;
   HWND enemy_Item;
   HWND enemy_Freq;
   HWND enemy_Status;
   HWND enemy_Action[4];
   HWND enemy_FinalAction;
   HWND enemy_Level;
   HWND enemy_Guts;
   HWND enemy_Iq;
   HWND enemy_Movement;
   HWND enemy_Order;
   HWND enemy_Friend[4];
   HWND enemy_MaxCall;
   HWND enemy_MissRate;
   HWND enemy_DieSound;
   HWND enemy_Type;
   HWND enemy_Music;
//------------------

// Assorted windows for editing Items
HWND item_stats_box;
  HWND item_NameList;
   HWND item_Name;
   HWND item_Type;
   HWND item_Cost;
   HWND item_Ownership[7];
   HWND item_Effect;
   HWND item_Strength;
   HWND item_Increase;
   HWND item_ExtraPower;
   HWND item_SpecialProperties;
   HWND item_DescAddress;
//-------------------

//assorted windows for PSI name editing
 HWND psi_name_box;

   HWND psi_name[16];
//-------------------

//teleport editing junk
  HWND tele_main_box;
  HWND tele_header;
   HWND tele_name[16];
   HWND tele_x[16];
   HWND tele_y[16];
//-------------------

//misc text editing junk
  HWND misc_main_box;

   HWND misc_phone_list;
    HWND misc_phone_edit;
    HWND misc_phone_pointer;
   HWND misc_ail_list;
    HWND misc_ail_edit;
   HWND misc_menu_list;
    HWND misc_menu_edit;
//-------------------
char filename[MAX_PATH];
OPENFILENAME ofn;
OPENFILENAME dumpofn;
OPENFILENAME ipsofn;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK SplashProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK AboutProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK SearchProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK MakeIPSProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK WaitingProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);

bool CALLBACK EnemyEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK ItemEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK PSINameEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK TeleEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
bool CALLBACK MiscTextEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
//setting and hacking function junk.  Whatnot.
void set_enemy_selected(int select);
void set_item_selected(int select);
void set_psi_selected(int select);
void set_tele_selected(int select);
void set_misc_selected(int select);

void apply_enemy_selected(int select);
void apply_item_selected(int select);
void apply_psi_selected(int select);
void apply_tele_selected(int select);
void apply_misc_selected(int select);

void refreshenemylist(HWND window);
void refreshitemlist(HWND window);
void refresheffectlist(HWND window);
void refreshpsipowerlist(HWND window);
void refreshpsinamelist(HWND window);

void refreshphonelist(HWND window);
void refreshaillist(HWND window);
void refreshmenulist(HWND window);

void DrawSprite(int x, int y, int select, HDC hdc);
//useful functions
RECT makerect(int a, int b, int x, int y);
void SetWinCap(HWND window, unsigned int number);
void SetWinHexCap(HWND window, unsigned int number);
void SetWinSignedCap(HWND window, signed int number);
unsigned int junkstuff(HWND window, unsigned int max);
signed int junkstuffsigned(HWND window, signed int max);
void init(void);
void tryopenfile(char *openfile);
void makewindowtoolbar(HWND window);
void makenofindwindowtoolbar(HWND window);

//stuff
char *recent[5];
void refreshrecent(void);
//---------------------------------------------------------------------------
#pragma argsused
WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
  
    smallfonty=CreateFont(13,0,0,0,200,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
    fonty=CreateFont(15,0,0,0,200,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
    boldfonty=CreateFont(15,0,0,0,600,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");
    bigfonty=CreateFont(36,0,0,0,300,false,false,false,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Arial");

    pkhack_icon=LoadIcon(hInstance,MAKEINTRESOURCE(1));
    pile_icon=LoadIcon(hInstance,MAKEINTRESOURCE(2));
    fobby_icon=LoadIcon(hInstance,MAKEINTRESOURCE(3));
    ufo_icon=LoadIcon(hInstance,MAKEINTRESOURCE(4));
    garbage_icon=LoadIcon(hInstance,MAKEINTRESOURCE(5));
    closepresent_icon=LoadIcon(hInstance,MAKEINTRESOURCE(6));
    openpresent_icon=LoadIcon(hInstance,MAKEINTRESOURCE(7));
    butterfly_icon=LoadIcon(hInstance,MAKEINTRESOURCE(8));
    light_icon=LoadIcon(hInstance,MAKEINTRESOURCE(9));
    diamond_icon=LoadIcon(hInstance,MAKEINTRESOURCE(10));
    phone_icon=LoadIcon(hInstance,MAKEINTRESOURCE(11));
    wow_icon=LoadIcon(hInstance,MAKEINTRESOURCE(12));
    sign_icon=LoadIcon(hInstance,MAKEINTRESOURCE(13));
    scorp_icon=LoadIcon(hInstance,MAKEINTRESOURCE(14));
    frog_icon=LoadIcon(hInstance,MAKEINTRESOURCE(15));
    help_icon=LoadIcon(hInstance,MAKEINTRESOURCE(16));
    mail_icon=LoadIcon(hInstance,MAKEINTRESOURCE(17));
    tv_icon=LoadIcon(hInstance,MAKEINTRESOURCE(18));

    menubar=CreateMenu();
    filemenu=CreateMenu();
    helpmenu=CreateMenu();


    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_LOAD,"&Load ROM");
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_CLOSE,"&Close ROM");
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
     InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,100,"");
     InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,101,"");
     InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,102,"");
     InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,103,"");
     InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,104,"");
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_MAKEIPS,"&Make IPS");
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
    InsertMenu(filemenu,0xffffffff,MF_BYPOSITION|MF_STRING,FILEMENU_EXIT,"&Exit");
    InsertMenu(helpmenu,0xffffffff,MF_BYPOSITION|MF_STRING,HELPMENU_HELP,"&Help");
    InsertMenu(helpmenu,0xffffffff,MF_BYPOSITION|MF_STRING,HELPMENU_ABOUT,"&About");
    InsertMenu(menubar,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)filemenu,"&File");
    InsertMenu(menubar,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)helpmenu,"&Help");


    window_stuff_menu=CreateMenu();
     InsertMenu(window_stuff_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_CLOSEROM,"Close &ROM");

   enemy_menu=CreateMenu();
    enemy_view_menu=CreateMenu();
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_FIND,"&Find");
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_NONE,"List by &Original Order");
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_ALPHA,"List by Enemy &Name");
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_LEVEL,"List by Game &Order");
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
     InsertMenu(enemy_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING,VIEW_DUMP,"&Dump To Text");
    InsertMenu(enemy_menu,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)window_stuff_menu,"&File");
    InsertMenu(enemy_menu,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)enemy_view_menu,"&View");
    InsertMenu(enemy_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_HELP,"&Help");

  item_menu=CreateMenu();
   item_view_menu=CreateMenu();
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_FIND,"&Find");
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_NONE,"List by &Original Order");
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_ALPHA,"List by Item &Name");
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_LEVEL,"&List by &Type");
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_SEPARATOR,NULL,NULL);
     InsertMenu(item_view_menu,0xffffffff,MF_BYPOSITION|MF_STRING,VIEW_DUMP,"&Dump To Text");
    InsertMenu(item_menu,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)window_stuff_menu,"&File");
    InsertMenu(item_menu,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)item_view_menu,"&View");
    InsertMenu(item_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_HELP,"&Help");

   PSI_menu=CreateMenu();
    InsertMenu(PSI_menu,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)window_stuff_menu,"&File");
    InsertMenu(PSI_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_HELP,"&Help");

   tele_menu=CreateMenu();
    InsertMenu(tele_menu,0xffffffff,MF_BYPOSITION|MF_POPUP,(UINT)window_stuff_menu,"&File");
    InsertMenu(tele_menu,0xffffffff,MF_BYPOSITION|MF_STRING,WINDOWMENU_HELP,"&Help");

    for(int j=0;j<5;j++)
    {
     recent[j]=new char[MAX_PATH];
    }

    //recently-opened list
      FILE *inifile=fopen("pkhack.ini","r+t");
       if(inifile)
       {
        for(int n=0;n<5;n++)
        {
         char *returned=fgets(recent[n],MAX_PATH,inifile);
         if(recent[n][strlen(recent[n])-1]=='\n')recent[n][(strlen(recent[n])-1)]=NULL;
         if(returned==0)recent[n][0]=NULL;
        }
       }
       else
       {
        for(int n=0;n<5;n++)
        {
         recent[n][0]=NULL;
        }
       }
     fclose(inifile);

    refreshrecent();

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

    int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-200;
    int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-200;

    InitCommonControls();

    hWnd=CreateWindow("EB-hackWindow","PK Hack!",WS_POPUP|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,xpos,ypos,400,375+GetSystemMetrics(SM_CYCAPTION)*2,NULL,menubar,hInstance,NULL);
    statusbox=CreateWindow(STATUSCLASSNAME,"PK Hack",WS_CHILD|WS_VISIBLE,0,0,0,0,hWnd,NULL,hInstance,NULL);

    toolbar=CreateWindow(TOOLBARCLASSNAME,"Tools",TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CCS_TOP,0,0,100,100,hWnd,NULL,hInstance,NULL);
    CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,33,500,2,hWnd,NULL,hInstance,NULL);
    SendMessage(toolbar,TB_SETBITMAPSIZE,0,MAKELONG(20,20));
    SendMessage(toolbar,TB_SETBUTTONSIZE,0,MAKELONG(26,27));
    SendMessage(toolbar,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);

    SendMessage(title,WM_SETFONT,(WPARAM)bigfonty,0);
    SendMessage(descbox,WM_SETFONT,(WPARAM)smallfonty,0);

    tree=CreateWindowEx(0,WC_TREEVIEW,"Tree",WS_BORDER|TVS_SHOWSELALWAYS|TVS_HASBUTTONS|TVS_HASLINES|TVS_LINESATROOT|WS_CHILD|WS_VISIBLE,5,40,200,290,hWnd,NULL,hInstance,NULL);
    DialogBox(hInstance,MAKEINTRESOURCE(10000),hWnd,(DLGPROC)SplashProc);

    ShowWindow(hWnd,nCmdShow);

    MSG msg;
    init();
   while(GetMessage(&msg,NULL,0,0))
   {
    if((msg.hwnd==tree)&&(msg.message==WM_LBUTTONDBLCLK))
    {
    HTREEITEM tempitem;
    TV_HITTESTINFO hit;
    hit.pt.x=LOWORD(msg.lParam);
    hit.pt.y=HIWORD(msg.lParam);
    hit.flags=TVHT_ONITEMLABEL;
    TreeView_HitTest(tree,&hit);
     if(hit.hItem==tree_about)DialogBox(hInst,MAKEINTRESOURCE(10001),hWnd,(DLGPROC)AboutProc);
     if((hit.hItem==tree_enemies)&&(fileloaded))DialogBox(hInst,MAKEINTRESOURCE(738),hWnd,(DLGPROC)EnemyEditProc);
     if((hit.hItem==tree_items)&&(fileloaded))DialogBox(hInst,MAKEINTRESOURCE(738),hWnd,(DLGPROC)ItemEditProc);
     if((hit.hItem==tree_PSI_names)&&(fileloaded))DialogBox(hInst,MAKEINTRESOURCE(738),hWnd,(DLGPROC)PSINameEditProc);
     if((hit.hItem==tree_PSI_teleport)&&(fileloaded))DialogBox(hInst,MAKEINTRESOURCE(738),hWnd,(DLGPROC)TeleEditProc);
     if((hit.hItem==tree_text)&&(fileloaded))DialogBox(hInst,MAKEINTRESOURCE(738),hWnd,(DLGPROC)MiscTextEditProc);

     if(hit.hItem==tree_help)
          {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
         char *temp=new char[strlen(line)+15];
         strcpy(temp,line);
//         strcat(temp,"pkfaq.txt");
         WinHelp(hWnd,temp,HELP_FINDER,0);
         ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
         delete temp;
         }
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
   }
}


void init(void)
{

//toolbar stuff
   TBADDBITMAP addbmp;
    addbmp.hInst=hInst;
    addbmp.nID=101;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=102;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=103;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=104;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=105;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=108;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=112;
    SendMessage(toolbar,TB_ADDBITMAP,1,(LPARAM)&addbmp);

   TBBUTTON button[8];
    button[0].iBitmap=0;
    button[0].idCommand=0;
    button[0].fsState=TBSTATE_ENABLED;
    button[0].fsStyle=TBSTYLE_SEP;
    button[0].iString=0;

    button[1].iBitmap=0;
    button[1].idCommand=FILEMENU_LOAD;
    button[1].fsState=TBSTATE_ENABLED;
    button[1].fsStyle=TBSTYLE_BUTTON;
    button[1].iString=0;

    button[2].iBitmap=1;
    button[2].idCommand=FILEMENU_CLOSE;
    button[2].fsState=TBSTATE_ENABLED;
    button[2].fsStyle=TBSTYLE_BUTTON;
    button[2].iString=0;

    button[3].iBitmap=5;
    button[3].idCommand=FILEMENU_MAKEIPS;
    button[3].fsState=TBSTATE_ENABLED;
    button[3].fsStyle=TBSTYLE_BUTTON;
    button[3].iString=0;

    button[4].iBitmap=0;
    button[4].idCommand=0;
    button[4].fsState=TBSTATE_ENABLED;
    button[4].fsStyle=TBSTYLE_SEP;
    button[4].iString=0;

    button[5].iBitmap=6;
    button[5].idCommand=FILEMENU_POSSESS;
    button[5].fsState=TBSTATE_ENABLED;
    button[5].fsStyle=TBSTYLE_BUTTON;
    button[5].iString=0;

    button[6].iBitmap=0;
    button[6].idCommand=0;
    button[6].fsState=TBSTATE_ENABLED;
    button[6].fsStyle=TBSTYLE_SEP;
    button[6].iString=0;

    button[7].iBitmap=4;
    button[7].idCommand=HELPMENU_HELP;
    button[7].fsState=TBSTATE_ENABLED;
    button[7].fsStyle=TBSTYLE_BUTTON;
    button[7].iString=0;

    SendMessage(toolbar,TB_ADDBUTTONS,8,(LPARAM)button);

    SendMessage(toolbar,WM_SIZE,0,0);




//tree items
    TV_ITEM tvitem;
    tvitem.mask=TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
    tvitem.iImage=0;
    tvitem.iSelectedImage=0;
    tvitem.pszText="PKHack";
    TV_INSERTSTRUCT ist;
    ist.hInsertAfter=NULL;
    ist.hParent=NULL;
    ist.item=tvitem;
    tree_pkhack=TreeView_InsertItem(tree,&ist);

    tvitem.pszText="Enemies";
     tvitem.iImage=1;
     tvitem.iSelectedImage=1;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
    tree_enemies=TreeView_InsertItem(tree,&ist);
/*     tvitem.iImage=2;
     tvitem.iSelectedImage=2;
     tvitem.pszText="Stats";
     ist.hParent=tree_enemies;
     ist.item=tvitem;
     tree_enemies_stats=TreeView_InsertItem(tree,&ist);
     tvitem.iImage=3;
     tvitem.iSelectedImage=3;
     tvitem.pszText="Actions/Text";
     ist.hParent=tree_enemies;
     ist.item=tvitem;
     tree_enemies_acttexts=TreeView_InsertItem(tree,&ist);*/

    tvitem.pszText="Items";
    tvitem.iImage=4;
    tvitem.iSelectedImage=4;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
    tree_items=TreeView_InsertItem(tree,&ist);
/*     tvitem.iImage=5;
     tvitem.iSelectedImage=5;
     tvitem.pszText="Stats";
     ist.hParent=tree_items;
     ist.item=tvitem;
     tree_items_stats=TreeView_InsertItem(tree,&ist);
     tvitem.iImage=6;
     tvitem.iSelectedImage=6;
     tvitem.pszText="Actions/Text";
     ist.hParent=tree_items;
     ist.item=tvitem;
     tree_items_acttexts=TreeView_InsertItem(tree,&ist);*/

    tvitem.pszText="PSI";
    tvitem.iImage=7;
    tvitem.iSelectedImage=7;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
    tree_PSI=TreeView_InsertItem(tree,&ist);
     tvitem.iImage=8;
     tvitem.iSelectedImage=8;
     tvitem.pszText="Names";
     ist.hParent=tree_PSI;
     ist.item=tvitem;
     tree_PSI_names=TreeView_InsertItem(tree,&ist);
     tvitem.iImage=9;
     tvitem.iSelectedImage=9;
     tvitem.pszText="Teleport";
     ist.hParent=tree_PSI;
     ist.item=tvitem;
     tree_PSI_teleport=TreeView_InsertItem(tree,&ist);

    tvitem.pszText="Text";
    tvitem.iImage=10;
    tvitem.iSelectedImage=10;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
    tree_text=TreeView_InsertItem(tree,&ist);
/*     tvitem.iImage=11;
     tvitem.iSelectedImage=11;
     tvitem.pszText="Phone";
     ist.hParent=tree_text;
     ist.item=tvitem;
     tree_text_phone=TreeView_InsertItem(tree,&ist);
     tvitem.iImage=12;
     tvitem.iSelectedImage=12;
     tvitem.pszText="Ailments";
     ist.hParent=tree_text;
     ist.item=tvitem;
     tree_text_ailments=TreeView_InsertItem(tree,&ist);
     tvitem.iImage=13;
     tvitem.iSelectedImage=13;
     tvitem.pszText="Battle Menu";
     ist.hParent=tree_text;
     ist.item=tvitem;
     tree_text_battlemenu=TreeView_InsertItem(tree,&ist);*/


    tvitem.pszText="Save States";
    tvitem.iImage=14;
    tvitem.iSelectedImage=14;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
//    tree_savestates=TreeView_InsertItem(tree,&ist);

    tvitem.pszText="About";
    tvitem.iImage=15;
    tvitem.iSelectedImage=15;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
    tree_about=TreeView_InsertItem(tree,&ist);

    tvitem.pszText="Help";
    tvitem.iImage=16;
    tvitem.iSelectedImage=16;
    ist.hParent=tree_pkhack;
    ist.item=tvitem;
    tree_help=TreeView_InsertItem(tree,&ist);

    TreeView_Expand(tree,tree_pkhack,TVE_EXPAND);

    iconlist=ImageList_Create(16,16,ILC_COLOR8|ILC_MASK,10,10);
    ImageList_AddIcon(iconlist,pkhack_icon);
    ImageList_AddIcon(iconlist,pile_icon);
    ImageList_AddIcon(iconlist,fobby_icon);
    ImageList_AddIcon(iconlist,ufo_icon);
    ImageList_AddIcon(iconlist,garbage_icon);
    ImageList_AddIcon(iconlist,closepresent_icon);
    ImageList_AddIcon(iconlist,openpresent_icon);
    ImageList_AddIcon(iconlist,butterfly_icon);
    ImageList_AddIcon(iconlist,light_icon);
    ImageList_AddIcon(iconlist,diamond_icon);
    ImageList_AddIcon(iconlist,sign_icon);
    ImageList_AddIcon(iconlist,phone_icon);
    ImageList_AddIcon(iconlist,scorp_icon);
    ImageList_AddIcon(iconlist,frog_icon);
    ImageList_AddIcon(iconlist,mail_icon);
    ImageList_AddIcon(iconlist,wow_icon);
    ImageList_AddIcon(iconlist,help_icon);
    ImageList_AddIcon(iconlist,tv_icon);
    TreeView_SetImageList(tree,iconlist,TVSIL_NORMAL);

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner=hWnd;
    ofn.hInstance=hInst;
    ofn.lpstrFilter="SNES ROM (*.smc, *.sfc, *.fig)\0*.smc;*.sfc;*.fig\0All files\0*.*\0";
    ofn.lpstrCustomFilter=NULL;
    ofn.nMaxCustFilter=NULL;
    ofn.nFilterIndex=1;
    ofn.lpstrFile=filename;
    filename[0]='\0';
    ofn.nMaxFile=MAX_PATH;
    ofn.lpstrFileTitle=NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir=NULL;
    ofn.lpstrTitle=NULL;
    ofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
    ofn.lpstrDefExt="smc";

    dumpofn.lStructSize = sizeof(OPENFILENAME);
    dumpofn.hwndOwner=hWnd;
    dumpofn.hInstance=hInst;
    dumpofn.lpstrFilter="Text files\0*.txt\0All files\0*.*\0";
    dumpofn.lpstrCustomFilter=NULL;
    dumpofn.nMaxCustFilter=NULL;
    dumpofn.nFilterIndex=1;
    dumpofn.lpstrFile=new char[MAX_PATH];
    dumpofn.lpstrFile[0]='\0';
    dumpofn.nMaxFile=MAX_PATH;
    dumpofn.lpstrFileTitle=NULL;
    dumpofn.nMaxFileTitle = 0;
    dumpofn.lpstrInitialDir=NULL;
    dumpofn.lpstrTitle=NULL;
    dumpofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
    dumpofn.lpstrDefExt="txt";

    ipsofn.lStructSize = sizeof(OPENFILENAME);
    ipsofn.hwndOwner=hWnd;
    ipsofn.hInstance=hInst;
    ipsofn.lpstrFilter="IPS files\0*.ips\0All files\0*.*\0";
    ipsofn.lpstrCustomFilter=NULL;
    ipsofn.nMaxCustFilter=NULL;
    ipsofn.nFilterIndex=1;
    ipsofn.lpstrFile=new char[MAX_PATH];
    ipsofn.lpstrFile[0]='\0';
    ipsofn.nMaxFile=MAX_PATH;
    ipsofn.lpstrFileTitle=NULL;
    ipsofn.nMaxFileTitle = 0;
    ipsofn.lpstrInitialDir=NULL;
    ipsofn.lpstrTitle=NULL;
    ipsofn.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY;
    ipsofn.lpstrDefExt="ips";

    //----making background----
    HDC hdc=GetDC(hWnd);
     HDC bgdc=CreateCompatibleDC(hdc);
     background=CreateCompatibleBitmap(hdc,640,480);
     HDC temp=CreateCompatibleDC(hdc);
    ReleaseDC(hWnd,hdc);
     HBITMAP tempbmp=LoadBitmap(hInst,MAKEINTRESOURCE(99));
     HBITMAP junk=SelectObject(bgdc,background);
     SelectObject(temp,tempbmp);


     for(int y=0;y<30;y++)
     {
      for(int x=0;x<40;x++)
      {
       BitBlt(bgdc,x*16,y*16,16,16,temp,0,0,SRCCOPY);
      }
     }
      DeleteDC(temp);
      DeleteObject(tempbmp);
      SelectObject(bgdc,junk);
      DeleteDC(bgdc);
      DeleteObject(junk);


}

void quitting(void)
{
 if(fileloaded)fclose(rom);


 //Find directory we're in then save ini file
 char *liney=GetCommandLine();
 char *line=new char[strlen(liney)+1];
 strcpy(line,liney);
 char commandy[MAX_PATH];
 while(line[strlen(line)-1]!='\\')
  {
   line[strlen(line)-1]=NULL;
  }
 line++;
 char *temp=new char[strlen(line)+15];
 strcpy(temp,line);
 strcat(temp,"pkhack.ini");
 FILE *inifile=fopen(temp,"w+t");
 if(inifile)
 {
  for(int n=0;n<5;n++)
  {
   fputs(recent[n],inifile);
   fputc('\n',inifile);
  }
 }
 fclose(inifile);

 PostQuitMessage(0);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;

       if(hdr->code==TTN_NEEDTEXT)
       {
            TOOLTIPTEXT *tooltip=(TOOLTIPTEXT*)lParam;
            tooltip->hinst = hInst;

            switch (hdr->idFrom){
                case FILEMENU_LOAD:{
                    tooltip->lpszText="Load ROM";
                    break;}
                case FILEMENU_CLOSE:{
                    tooltip->lpszText="Close ROM";
                    break;}
                case HELPMENU_HELP:{
                    tooltip->lpszText="Help";
                    break;}
                case FILEMENU_MAKEIPS:{
                    tooltip->lpszText="Create IPS";
                    break;}
                case FILEMENU_POSSESS:{
                    tooltip->lpszText="???";
                    break;}
            }
        return lParam;
       }

    if((hdr->code==TVN_SELCHANGED)&&(hdr->hwndFrom==tree))
    {
     HTREEITEM selected=TreeView_GetSelection(tree);
     char romstatus[250];
     strcpy(romstatus,"PK Hack!");
     if(selected==tree_pkhack){strcpy(romstatus,"PK Hack!");}
     if(selected==tree_enemies){strcpy(romstatus,"Modify Enemy Attributes");}
     if(selected==tree_items){strcpy(romstatus,"Modify Item Attributes");}
     if(selected==tree_PSI){strcpy(romstatus,"Modify PSI and PSI-related attributes");}
       if(selected==tree_PSI_names){strcpy(romstatus,"Modify PSI names");}
       if(selected==tree_PSI_teleport){strcpy(romstatus,"Modify teleport location and names");}
     if(selected==tree_text){strcpy(romstatus,"Modify game text");}
     if(selected==tree_about){strcpy(romstatus,"About PK Hack");}
     if(selected==tree_help){strcpy(romstatus,"View the PK Hack help file");}

     if(selected==tree_pkhack){
      if(romtype==-1)strcat(romstatus," - No ROM loaded - ");
      if(romtype==0)strcat(romstatus," - Unrecognized ROM - ");
      if(romtype==1)strcat(romstatus," - EarthBound ROM - ");
      if(romtype==2)strcat(romstatus," - Mother 2 ROM - ");
      char buf[15];
      ultoa(romsize,buf,10);
      strcat(romstatus,buf);
      strcat(romstatus," bytes");}
      if(fileloaded){if(IsPossessed(rom))strcat(romstatus," - Possessed!");}
      SetWindowText(statusbox,romstatus);

    }
   break;
  }

  case WM_COMMAND:
  {
   if(HIWORD(wParam)==0)
   {
    if((LOWORD(wParam)==FILEMENU_POSSESS)&&(fileloaded))
    {
     if((fileloaded)&&(IsCorrectRom(rom)!=0))
     {
      int reply=MessageBox(hWnd,"OK, Pop Quiz.  \"A Beatles song, XXXterday.\"\nCan you fill the blanks?","Quiz",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
      if(IsPossessed(rom))
      {
       if(reply==IDYES){MessageBox(hWnd,"Uaahg.  You got me!","Unpossessing...",MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);ExorciseROM(rom);PlaySound(MAKEINTRESOURCE(1006),hInst,SND_ASYNC|SND_RESOURCE);}
       if(reply==IDNO){MessageBox(hWnd,"Buzzzzt. \"A Beatles song, Noterday.\"  Wrong!","Boo!",MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);}
      }
      else
      {
       if(reply==IDYES){MessageBox(hWnd,"That's correct.  I'm impressed!","Hehehe....",MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);}
       if(reply==IDNO){MessageBox(hWnd,"Buzzzzt. \"A Beatles song, Noterday.\"  Wrong!  I will now possess your ROM!","Boo!",MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);PossessROM(rom);PlaySound(MAKEINTRESOURCE(1005),hInst,SND_ASYNC|SND_RESOURCE);}
      }
     }
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
    }

    if(LOWORD(wParam)==FILEMENU_MAKEIPS)
    {
     if(fileloaded)DialogBox(hInst,MAKEINTRESOURCE(10004),hWnd,(DLGPROC)MakeIPSProc);
    }

    if(LOWORD(wParam)==FILEMENU_LOAD)
    {
     strcpy(filename,recent[0]);
     if(GetOpenFileName(&ofn))
     {
      tryopenfile(filename);
     }
    }

    if(LOWORD(wParam)==FILEMENU_CLOSE)
    {
     if(fileloaded){fclose(rom);PlaySound(MAKEINTRESOURCE(1002),hInst,SND_ASYNC|SND_RESOURCE);}
     fileloaded=false;
     romtype=-1;
     romsize=0;
     selected_enemy=0;


     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
    }

    if(LOWORD(wParam)==FILEMENU_EXIT)
    {
     quitting();
    }
    if(LOWORD(wParam)==HELPMENU_ABOUT) 
    {
     DialogBox(hInst,MAKEINTRESOURCE(10001),hWnd,(DLGPROC)AboutProc);
    }

    if(LOWORD(wParam)==HELPMENU_HELP)
    {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
     char *temp=new char[strlen(line)+15];
     strcpy(temp,line);
//     strcat(temp,"pkfaq.txt");
     WinHelp(hWnd,temp,HELP_FINDER,0);
     ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
     delete temp;
    }

    for(int n=0;n<5;n++)
    {
     if(LOWORD(wParam)==100+n)
     {
      if(recent[n][0]!=NULL)tryopenfile(recent[n]);
      break;
     }
    }

   }
   break;
  }


  case WM_CLOSE:
  {
   quitting();
   break;
  }

  case WM_PAINT:
  {
   RECT rect;
   GetUpdateRect(hWnd,&rect,false);

   HDC hdc=GetDC(hWnd);
    HDC joe=CreateCompatibleDC(hdc);
     SelectObject(joe,background);
     BitBlt(hdc,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,joe,rect.left,rect.top,SRCCOPY);
    DeleteDC(joe);
   ReleaseDC(hWnd,hdc);
  }

  default: return DefWindowProc(hwnd,wMsg,wParam,lParam);
 }
}

void SetWinCap(HWND window, unsigned int number)
{
 char buf[15];
 ultoa(number,buf,10);
 SetWindowText(window,buf);
}

void SetWinHexCap(HWND window, unsigned int number)
{
 char buff[16];
 char buf[15];
 ultoa(number,buf,16);
 strcpy(buff,"$");
 strcat(buff,buf);
 strupr(buff);
 SetWindowText(window,buff);
}

void SetWinSignedCap(HWND window, signed int number)
{
 char buf[15];
 _ltoa(number,buf,10);
 SetWindowText(window,buf);
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
  SendMessage(CreateWindow("STATIC","PK Hack, version 0.20",WS_CHILD|WS_VISIBLE|SS_SIMPLE,0,0,400,50,hwnd,NULL,hInst,NULL),WM_SETFONT,(WPARAM)bigfonty,MAKELPARAM(true,0));
  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-250;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-150;
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,500,300,SWP_SHOWWINDOW);
  return true;
  }

  case WM_TIMER:
  {
   EndDialog(hwnd,5);
   return false;
  }

  case WM_CLOSE:
  case WM_LBUTTONDOWN:
  {
   EndDialog(hwnd,5);
   return false;
  }

  default: return false;
 }
}

//-----------------Dialog Box Functions

bool CALLBACK EnemyEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 static bool repaint=false;
 if((repaint)&&(wMsg!=WM_PAINT)&&(wMsg!=WM_NCPAINT)){
   int select=SendMessage(enemy_NameList,CB_GETCURSEL,0,0);
//   DrawSprite(50,200,sprConv[enemies[select].outsidePic],enemy_graphics_box);
   repaint=false;
  }

 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  enemywindow=hwnd;
  SetMenu(hwnd,enemy_menu);
  SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)pile_icon);

  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-310;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-(225+(GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU)/2));
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,620,450+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU),SWP_SHOWWINDOW);

    //toolbar stuff
     window_toolbar=CreateWindow(TOOLBARCLASSNAME,"Tools",TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CCS_TOP,0,0,100,100,hwnd,NULL,hInst,NULL);
     CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,33,640,2,hwnd,NULL,hInst,NULL);
     makewindowtoolbar(window_toolbar);

  SetWindowText(hwnd,"Enemy Editing");
  tabs=CreateWindowEx(0,WC_TABCONTROL,"Tabs",WS_CHILD|WS_VISIBLE,0,40,615,405,hwnd,NULL,hInst,NULL);
  SendMessage(tabs,TCM_SETIMAGELIST,0,(LPARAM)iconlist);

    TC_ITEM tc;
    tc.mask=TCIF_TEXT|TCIF_IMAGE;
    tc.pszText="Stats";
    tc.iImage=2;
    TabCtrl_InsertItem(tabs,0,&tc);
    tc.pszText="Actions";
    tc.iImage=3;
    TabCtrl_InsertItem(tabs,1,&tc);
    tc.pszText="Gfx / Sound";
    tc.iImage=17;
    TabCtrl_InsertItem(tabs,2,&tc);
  HWND temp;

  enemy_NameList=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,104,89,250,150,hwnd,NULL,hInst,NULL);


  enemy_stats_box=CreateWindow("BUTTON","Enemy Stats:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,5,32,605,370,tabs,NULL,hInst,NULL);
  temp=CreateWindow("STATIC","Enemy:",WS_CHILD|WS_VISIBLE,5,19,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Name=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,45,200,20,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Name:",WS_CHILD|WS_VISIBLE,5,46,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Hp=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,67,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","HP:",WS_CHILD|WS_VISIBLE,5,67,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Pp=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,88,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","PP:",WS_CHILD|WS_VISIBLE,5,88,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Exp=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,109,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Exp:",WS_CHILD|WS_VISIBLE,5,109,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Money=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,130,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Money:",WS_CHILD|WS_VISIBLE,5,130,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Speed=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,151,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Speed:",WS_CHILD|WS_VISIBLE,5,151,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Offense=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,172,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Offense:",WS_CHILD|WS_VISIBLE,5,172,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Defense=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,193,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Defense:",WS_CHILD|WS_VISIBLE,5,193,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Status=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,214,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Status:",WS_CHILD|WS_VISIBLE,5,214,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Level=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,235,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Level:",WS_CHILD|WS_VISIBLE,5,235,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Guts=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,256,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Guts:",WS_CHILD|WS_VISIBLE,5,256,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Iq=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,277,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","IQ:",WS_CHILD|WS_VISIBLE,5,277,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

    enemy_TheFlag=CreateWindow("BUTTON","'The' flag",WS_CHILD|WS_TABSTOP|WS_VISIBLE|BS_AUTOCHECKBOX,310,45,75,21,enemy_stats_box,NULL,hInst,NULL);
   enemy_Item=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,349,130,175,150,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Item:",WS_CHILD|WS_VISIBLE,255,130,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Freq=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,349,152,175,150,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Item freq.:",WS_CHILD|WS_VISIBLE,255,152,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_MissRate=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,350,180,100,19,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Miss rate:",WS_CHILD|WS_VISIBLE,255,180,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Type=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,349,200,102,90,enemy_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Type:",WS_CHILD|WS_VISIBLE,255,200,90,19,enemy_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

  enemy_acts_box=CreateWindow("BUTTON","Enemy Actions:",WS_CHILD|BS_GROUPBOX,5,32,605,370,tabs,NULL,hInst,NULL);
  temp=CreateWindow("STATIC","Enemy:",WS_CHILD|WS_VISIBLE,10,17,90,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Action[0]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,104,45+72,195,150,hwnd,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Action 1:",WS_CHILD|WS_VISIBLE,10,45,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Action[1]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,104,68+72,195,150,hwnd,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Action 2:",WS_CHILD|WS_VISIBLE,10,68,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Action[2]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,104,91+72,195,150,hwnd,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Action 3:",WS_CHILD|WS_VISIBLE,10,91,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Action[3]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|CBS_DROPDOWNLIST|WS_VSCROLL,104,114+72,195,150,hwnd,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Action 4:",WS_CHILD|WS_VISIBLE,10,114,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

   enemy_Friend[0]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWN|WS_VSCROLL,385,45,195,150,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Argument 1:",WS_CHILD|WS_VISIBLE,305,45,80,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Friend[1]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWN|WS_VSCROLL,385,68,195,150,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Argument 2:",WS_CHILD|WS_VISIBLE,305,68,80,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Friend[2]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWN|WS_VSCROLL,385,91,195,150,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Argument 3:",WS_CHILD|WS_VISIBLE,305,91,80,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Friend[3]=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWN|WS_VSCROLL,385,114,195,150,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Argument 4:",WS_CHILD|WS_VISIBLE,305,114,80,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

   enemy_Order=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,99,157,195,150,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Order:",WS_CHILD|WS_VISIBLE,10,157,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_FinalAction=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,99,180,195,150,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Final action:",WS_CHILD|WS_VISIBLE,10,180,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_StartAction=CreateWindow("EDIT","",WS_CHILD|ES_AUTOHSCROLL|WS_VISIBLE|WS_BORDER,100,204,100,19,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Start text:",WS_CHILD|WS_VISIBLE,10,204,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_DieAction=CreateWindow("EDIT","",WS_CHILD|ES_AUTOHSCROLL|WS_VISIBLE|WS_BORDER,390,204,100,19,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Death text:",WS_CHILD|WS_VISIBLE,305,204,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_MaxCall=CreateWindow("EDIT","",WS_CHILD|ES_AUTOHSCROLL|WS_VISIBLE|WS_BORDER,100,225,100,19,enemy_acts_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Max call:",WS_CHILD|WS_VISIBLE,10,225,70,19,enemy_acts_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

 enemy_graphics_box=CreateWindow("BUTTON","Gfx / Sound:",WS_CHILD|BS_GROUPBOX,5,32,605,370,tabs,NULL,hInst,NULL);
  temp=CreateWindow("STATIC","Enemy:",WS_CHILD|WS_VISIBLE,5,17,90,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

   enemy_Movement=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,95,175,95,19,enemy_graphics_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Movement:",WS_CHILD|WS_VISIBLE,5,175,90,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_InsidePic=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,400,50,100,19,enemy_graphics_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Battle sprite:",WS_CHILD|WS_VISIBLE,305,50,90,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Palette=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,400,71,100,19,enemy_graphics_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Palette:",WS_CHILD|WS_VISIBLE,305,71,90,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

   enemy_DieSound=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,399,91,162,90,enemy_graphics_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Death Sound:",WS_CHILD|WS_VISIBLE,305,91,90,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   enemy_Music=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,399,113,162,90,enemy_graphics_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Music:",WS_CHILD|WS_VISIBLE,305,113,80,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

   enemy_selector=CreateWindow("COMBOBOX","",WS_CHILD|WS_BORDER|CBS_OWNERDRAWFIXED|WS_VSCROLL|CBS_DROPDOWNLIST,99,50+78,100,350,hwnd,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Outside pic:",WS_CHILD|WS_VISIBLE,5,50,90,19,enemy_graphics_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
    Ctl3dSubclassCtl(enemy_selector);


    for(int n=0;n<464;n++)
    {
     int temp=SendMessage(enemy_selector,CB_ADDSTRING,0,(LPARAM)"Hi");
     SendMessage(enemy_selector,CB_SETITEMDATA,temp,n);
    }

   applybuttn=CreateWindow("BUTTON","Apply Changes",WS_CHILD|WS_VISIBLE|BS_TEXT,465,385,100,30,hwnd,NULL,hInst,NULL);

 //stats messages, etc.
   SendMessage(enemy_NameList,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_NameList);
   SendMessage(enemy_TheFlag,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_TheFlag);
   SendMessage(enemy_Name,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Name);
   SendMessage(enemy_InsidePic,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_InsidePic);
//   SendMessage(enemy_OutsidePic,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_OutsidePic);
   SendMessage(enemy_Hp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Hp);
   SendMessage(enemy_Pp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Pp);
   SendMessage(enemy_Exp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Exp);
   SendMessage(enemy_Money,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Money);
   SendMessage(enemy_Speed,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Speed);
   SendMessage(enemy_Palette,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Palette);
   SendMessage(enemy_Offense,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Offense);
   SendMessage(enemy_Defense,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Defense);
   SendMessage(enemy_Item,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Item);
   SendMessage(enemy_Freq,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Freq);
   SendMessage(enemy_Status,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Status);
   SendMessage(enemy_Level,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Level);
   SendMessage(enemy_Guts,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Guts);
   SendMessage(enemy_Iq,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Iq);
   SendMessage(enemy_Movement,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Movement);
   SendMessage(enemy_MissRate,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_MissRate);
   SendMessage(enemy_DieSound,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_DieSound);
   SendMessage(enemy_Type,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Type);
   SendMessage(enemy_Music,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));  Ctl3dSubclassCtl(enemy_Music);

   SendMessage(applybuttn,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);
   SendMessage(tabs,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);
   SendMessage(enemy_stats_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);
   SendMessage(enemy_acts_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);
   SendMessage(enemy_graphics_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);

   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"1/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"2/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"4/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"8/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"16/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"32/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"64/128");
   SendMessage(enemy_Freq,CB_ADDSTRING,0,(LPARAM)"128/128");

   SendMessage(enemy_DieSound,CB_ADDSTRING,0,(LPARAM)"Normal");
   SendMessage(enemy_DieSound,CB_ADDSTRING,0,(LPARAM)"Boss");

   SendMessage(enemy_Type,CB_ADDSTRING,0,(LPARAM)"Normal");
   SendMessage(enemy_Type,CB_ADDSTRING,0,(LPARAM)"Insect");
   SendMessage(enemy_Type,CB_ADDSTRING,0,(LPARAM)"Metal");

   for(int n=0;n<192;n++)
   {
    SendMessage(enemy_Music,CB_ADDSTRING,0,(LPARAM)bgm[n]);
   }
   refreshitemlist(enemy_Item);
   SendMessage(enemy_NameList,CB_SETCURSEL,selected_enemy,0);

  //acts messages,etc.
   SendMessage(enemy_StartAction,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_StartAction);
   SendMessage(enemy_DieAction,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_DieAction);
   SendMessage(enemy_Action[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Action[0]);
   SendMessage(enemy_Action[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Action[1]);
   SendMessage(enemy_Action[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Action[2]);
   SendMessage(enemy_Action[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Action[3]);
   SendMessage(enemy_FinalAction,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_FinalAction);
   SendMessage(enemy_Order,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Order);
   SendMessage(enemy_Friend[0],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Friend[0]);
   SendMessage(enemy_Friend[1],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Friend[1]);
   SendMessage(enemy_Friend[2],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Friend[2]);
   SendMessage(enemy_Friend[3],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_Friend[3]);
   SendMessage(enemy_MaxCall,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(enemy_MaxCall);
    refreshenemylist(enemy_NameList);
    for(int n=0;n<4;n++)
    {
    refresheffectlist(enemy_Action[n]);
    refreshenemylist(enemy_Friend[n]);
    }
    refresheffectlist(enemy_FinalAction);
   SendMessage(enemy_Order,CB_ADDSTRING,0,(LPARAM)"Random");
   SendMessage(enemy_Order,CB_ADDSTRING,0,(LPARAM)"Random, favour third");
   SendMessage(enemy_Order,CB_ADDSTRING,0,(LPARAM)"In order");
   SendMessage(enemy_Order,CB_ADDSTRING,0,(LPARAM)"Staggered");
   SendMessage(enemy_NameList,CB_SETCURSEL,selected_enemy,0);
  set_enemy_selected(selected_enemy);
  break;
  }
  case WM_MEASUREITEM:
  {
   LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;
   lpmis->itemHeight = 95;
   lpmis->itemWidth = 64;
  return true;
  }

  case WM_DRAWITEM:
  {
   DRAWITEMSTRUCT *drawitem=(DRAWITEMSTRUCT*)lParam;
    DrawSprite(drawitem->rcItem.left,drawitem->rcItem.top,sprConv[drawitem->itemData],drawitem->hDC);
    char buf[15];
    itoa(drawitem->itemData,buf,10);
    SelectObject(drawitem->hDC,smallfonty);
    TextOut(drawitem->hDC,drawitem->rcItem.left,drawitem->rcItem.top+80,buf,strlen(buf));
    if(drawitem->itemState&ODS_SELECTED)
    {
     RECT rect={drawitem->rcItem.left,drawitem->rcItem.top,drawitem->rcItem.left+64,drawitem->rcItem.top+80};
     DrawFocusRect(drawitem->hDC,&rect);
    }
   return true;
  }

  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;
       if(hdr->code==TTN_NEEDTEXT)
       {
            TOOLTIPTEXT *tooltip=(TOOLTIPTEXT*)lParam;
            tooltip->hinst = hInst;

            switch (hdr->idFrom){
                case WINDOWMENU_FIND:{
                    tooltip->lpszText="Find Enemy Name";
                    break;}
                case WINDOWMENU_CLOSEROM:{
                    tooltip->lpszText="Close ROM";
                    break;}
                case VIEW_ALPHA:{
                    tooltip->lpszText="List By Alphabetical Order";
                    break;}
                case VIEW_LEVEL:{
                    tooltip->lpszText="List By Game Order";
                    break;}
                case VIEW_NONE:{
                    tooltip->lpszText="List By Original Order";
                    break;}
                case WINDOWMENU_HELP:{
                    tooltip->lpszText="Help";
                    break;}
            }
        return lParam;
       }

    if((hdr->code==TCN_SELCHANGE)&&(hdr->hwndFrom==tabs))
    {
     int tabsel=TabCtrl_GetCurSel(tabs);
     if(tabsel!=1){for(int n=0;n<4;n++)ShowWindow(enemy_Action[n],false);}
     ShowWindow(enemy_selector,false);
     if(tabsel==0)
     {
      ShowWindow(enemy_stats_box,true);
      ShowWindow(enemy_acts_box,false);
      ShowWindow(enemy_graphics_box,false);
     }
     if(tabsel==1)
     {
      ShowWindow(enemy_stats_box,false);
      ShowWindow(enemy_acts_box,true);
      ShowWindow(enemy_graphics_box,false);
      for(int n=0;n<4;n++)ShowWindow(enemy_Action[n],true);
     }
     if(tabsel==2)
     {
      ShowWindow(enemy_stats_box,false);
      ShowWindow(enemy_acts_box,false);
      ShowWindow(enemy_graphics_box,true);
      ShowWindow(enemy_selector,true);      
     }
    ShowWindow(enemy_NameList,false);
    ShowWindow(enemy_NameList,true);
    ShowWindow(applybuttn,false);
    ShowWindow(applybuttn,true);
    }
   break;
  }

  case WM_COMMAND:
  {
   if((HIWORD(wParam)==BN_CLICKED)&&(lParam==(LPARAM)applybuttn))
     {
      int selected=SendMessage(enemy_NameList,CB_GETCURSEL,0,0);
      if(selected>-1)apply_enemy_selected(selected);
     }

   for(int n=0;n<4;n++){
   if((HIWORD(wParam)==CBN_SELENDOK)&&(lParam==(LPARAM)enemy_Action[n])){
   int joe=SendMessage(enemy_Action[n],CB_GETCURSEL,0,0);
   int select=SendMessage(enemy_NameList,CB_GETCURSEL,0,0);
    if(actionType[joe]==TYPE_NORMAL){SendMessage(enemy_Friend[n],CB_RESETCONTENT,0,0);}
    if(actionType[joe]==TYPE_PSI){refreshpsipowerlist(enemy_Friend[n]); SendMessage(enemy_Friend[n],CB_SETCURSEL,enemies[select].amigo[n],0);}
     if(actionType[joe]==TYPE_CALL){
       int thingy=-1;  //defaults to -1
       refreshenemylist(enemy_Friend[n]);
       for(int i=0;i<230;i++){
       if(enemies[select].amigo[n]==0)break;
       if(enemies[i].number==enemies[select].amigo[n]-1){thingy=i;break;}}
       SendMessage(enemy_Friend[n],CB_SETCURSEL,thingy,0);
     }
     if(actionType[joe]==TYPE_ITEM){
       int thingy=0;  //defaults to 0... crazy stuff...
       refreshitemlist(enemy_Friend[n]);
       for(int i=0;i<256;i++){
       if(enemies[select].amigo[n]==0)break;
       if(items[i].number==enemies[select].amigo[n]){thingy=i;break;}}  //no -1 here, guess the 0 is fine with items...
       SendMessage(enemy_Friend[n],CB_SETCURSEL,thingy,0);
     }
   }}

   if((HIWORD(wParam)==CBN_SELENDOK)&&(lParam==(LPARAM)enemy_NameList))
   {
      int selected=SendMessage(enemy_NameList,CB_GETCURSEL,0,0);
      if(selected>-1)
      {
       set_enemy_selected(selected);
       selected_enemy=selected;
      }
   }

//menu-toolbar stuff
   if(HIWORD(wParam)==0)
   {
    switch(LOWORD(wParam))
    {
     case VIEW_ALPHA:
     {
       PlaySound(MAKEINTRESOURCE(1004),hInst,SND_ASYNC|SND_RESOURCE);
       enemy_sorting=VIEW_ALPHA;
       int numba=enemies[SendMessage(enemy_NameList,CB_GETCURSEL,0,0)].number;
       SortEnemiesAlpha(enemies);
       refreshenemylist(enemy_NameList);
       ModifyMenu(enemy_view_menu,2,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_NONE,"List by &Original Order");
       ModifyMenu(enemy_view_menu,3,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_ALPHA,"List by Enemy &Name");
       ModifyMenu(enemy_view_menu,4,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_LEVEL,"List by Game &Order");
       for(int n=0;n<230;n++){if(enemies[n].number==numba){SendMessage(enemy_NameList,CB_SETCURSEL,n,0);set_enemy_selected(n);break;}}
       break;
     }
     case VIEW_LEVEL:
     {
       PlaySound(MAKEINTRESOURCE(1004),hInst,SND_ASYNC|SND_RESOURCE);
       enemy_sorting=VIEW_LEVEL;
       int numba=enemies[SendMessage(enemy_NameList,CB_GETCURSEL,0,0)].number;
       SortEnemiesGameOrder(enemies);
       refreshenemylist(enemy_NameList);
       ModifyMenu(enemy_view_menu,2,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_NONE,"List by &Original Order");
       ModifyMenu(enemy_view_menu,3,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_ALPHA,"List by Enemy &Name");
       ModifyMenu(enemy_view_menu,4,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_LEVEL,"List by Game &Order");
       for(int n=0;n<230;n++){if(enemies[n].number==numba){SendMessage(enemy_NameList,CB_SETCURSEL,n,0);set_enemy_selected(n);break;}}
       break;
     }
     case VIEW_NONE:
     {
       PlaySound(MAKEINTRESOURCE(1004),hInst,SND_ASYNC|SND_RESOURCE);
       enemy_sorting=VIEW_NONE;
       int numba=enemies[SendMessage(enemy_NameList,CB_GETCURSEL,0,0)].number;
       SortEnemiesOriginal(enemies);
       refreshenemylist(enemy_NameList);
       ModifyMenu(enemy_view_menu,2,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_NONE,"List by &Original Order");
       ModifyMenu(enemy_view_menu,3,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_ALPHA,"List by Enemy &Name");
       ModifyMenu(enemy_view_menu,4,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_LEVEL,"&List by Game &Order");
       SendMessage(enemy_NameList,CB_SETCURSEL,numba,0);set_enemy_selected(numba);
       break;
     }
     case VIEW_DUMP:
     {
       if(GetSaveFileName(&dumpofn))
       {
        DumpEnemies(enemies,dumpofn.lpstrFile,1,1,1);
       }
      break;
     }

     case WINDOWMENU_CLOSEROM:
     {
     SetMenu(hwnd,NULL);
     PlaySound(MAKEINTRESOURCE(1002),hInst,SND_ASYNC|SND_RESOURCE);
     EndDialog(hwnd,5);
     fclose(rom);
     fileloaded=false;
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
     return false;
     }
     case WINDOWMENU_HELP:
     {
          {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
         char *temp=new char[strlen(line)+15];
         strcpy(temp,line);
//         strcat(temp,"pkfaq.txt");
         WinHelp(hWnd,temp,HELP_FINDER,0);
         ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
         delete temp;
         }
     break;
     }

     case WINDOWMENU_APPLY:
     {
      int selected=SendMessage(enemy_NameList,CB_GETCURSEL,0,0);
      if(selected>-1)apply_enemy_selected(selected);
      break;
     }

     case WINDOWMENU_FIND:
     {
      if(DialogBox(hInst,MAKEINTRESOURCE(10002),hWnd,(DLGPROC)SearchProc))
      {
       if(searchstring[0]!=NULL)
       {
        int findings[230];
        int max=SearchEnemy(enemies,searchstring,findings);
        int place=0;
        bool oki=true;
        while(oki)
        {
         if(place==max){oki=false;MessageBox(hwnd,"String not found","Done",MB_OK|MB_ICONQUESTION|MB_TASKMODAL);break;}
         SendMessage(enemy_NameList,CB_SETCURSEL,findings[place],0);
         set_enemy_selected(findings[place]);
         if(MessageBox(hwnd,"Find Next?","Search found",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL)==IDNO)oki=false;
         place++;
        }
       }
      }
      break;
     }

    }
   }
//done WM_COMMAND
   break;
  }

  case WM_CLOSE:
  {
   SetMenu(hwnd,NULL);
   EndDialog(hwnd,5);
   break;
  }
  case WM_PAINT:
  {
  repaint=true;
  return false;
  }

  default: return false;
 }
}

//----------------------item
bool CALLBACK ItemEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)closepresent_icon);

  SetMenu(hwnd,item_menu);

  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-295;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-(225+(GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU)/2));
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,590,450+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU),SWP_SHOWWINDOW);

    //toolbar stuff
     window_toolbar=CreateWindow(TOOLBARCLASSNAME,"Tools",TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CCS_TOP,0,0,100,100,hwnd,NULL,hInst,NULL);
     CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,33,640,2,hwnd,NULL,hInst,NULL);
     makewindowtoolbar(window_toolbar);

  SetWindowText(hwnd,"Item Editing");

  tabs=CreateWindowEx(0,WC_TABCONTROL,"Tabs",WS_CHILD|WS_VISIBLE,0,40,585,405,hwnd,NULL,hInst,NULL);
  SendMessage(tabs,TCM_SETIMAGELIST,0,(LPARAM)iconlist);
    TC_ITEM tc;
    tc.mask=TCIF_TEXT|TCIF_IMAGE;
    tc.pszText="Stats";
    tc.iImage=6;
    TabCtrl_InsertItem(tabs,3,&tc);

HWND temp;

item_NameList=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,104,89,250,150,hwnd,NULL,hInst,NULL);
  item_stats_box=CreateWindow("BUTTON","Item Stats:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,5,32,575,370,tabs,NULL,hInst,NULL);
  temp=CreateWindow("STATIC","Item:",WS_CHILD|WS_VISIBLE,5,17,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_Name=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,45,200,20,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Name:",WS_CHILD|WS_VISIBLE,5,46,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_Effect=CreateWindow("COMBOBOX","",WS_CHILD|WS_TABSTOP|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,99,66,175,150,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Effect:",WS_CHILD|WS_VISIBLE,5,66,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_Cost=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,90,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Cost:",WS_CHILD|WS_VISIBLE,5,90,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_Type=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,111,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Type:",WS_CHILD|WS_VISIBLE,5,111,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_Strength=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,132,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Strength:",WS_CHILD|WS_VISIBLE,5,132,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_ExtraPower=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,153,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Extra Power:",WS_CHILD|WS_VISIBLE,5,153,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_Increase=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,174,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","E.P. Increase:",WS_CHILD|WS_VISIBLE,5,174,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_SpecialProperties=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,195,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Special prop.:",WS_CHILD|WS_VISIBLE,5,195,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   item_DescAddress=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,216,100,19,item_stats_box,NULL,hInst,NULL);
    temp=CreateWindow("STATIC","Desc. address:",WS_CHILD|WS_VISIBLE,5,216,90,19,item_stats_box,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));

     HWND surroundbox=CreateWindow("BUTTON","Misc. options",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,395,110,100,90,item_stats_box,NULL,hInst,NULL);
     SendMessage(surroundbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(surroundbox);
     item_Ownership[0]=CreateWindow("BUTTON","Infinite Use",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,20,90,19,surroundbox,NULL,hInst,NULL);
     item_Ownership[1]=CreateWindow("BUTTON","Permanent",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,40,90,19,surroundbox,NULL,hInst,NULL);
     item_Ownership[2]=CreateWindow("BUTTON","Chicken",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,60,90,19,surroundbox,NULL,hInst,NULL);

     surroundbox=CreateWindow("BUTTON","Who can use?",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,280,110,100,110,item_stats_box,NULL,hInst,NULL);
     SendMessage(surroundbox,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(surroundbox);
     item_Ownership[3]=CreateWindow("BUTTON","Ness",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,20,90,19,surroundbox,NULL,hInst,NULL);
     item_Ownership[4]=CreateWindow("BUTTON","Paula",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,40,90,19,surroundbox,NULL,hInst,NULL);
     item_Ownership[5]=CreateWindow("BUTTON","Jeff",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,60,90,19,surroundbox,NULL,hInst,NULL);
     item_Ownership[6]=CreateWindow("BUTTON","Poo",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,5,80,90,19,surroundbox,NULL,hInst,NULL);

   applybuttn=CreateWindow("BUTTON","Apply Changes",WS_CHILD|WS_VISIBLE|BS_TEXT,465,385,100,30,hwnd,NULL,hInst,NULL);
   SendMessage(applybuttn,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);
   SendMessage(tabs,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(tabs);
   SendMessage(item_stats_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);

 //stats messages, etc.
   SendMessage(item_NameList,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_NameList);
   SendMessage(item_Name,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Name);
   SendMessage(item_Cost,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Cost);
   SendMessage(item_Type,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Type);
   SendMessage(item_Effect,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Effect);
   SendMessage(item_Strength,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Strength);
   SendMessage(item_Increase,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Increase);
   SendMessage(item_ExtraPower,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_ExtraPower);
   SendMessage(item_SpecialProperties,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_SpecialProperties);
   SendMessage(item_DescAddress,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_DescAddress);
for(int n=0;n<8;n++)
{
 SendMessage(item_Ownership[n],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(item_Ownership[n]);
}

   refreshitemlist(item_NameList);
   refresheffectlist(item_Effect);

   SendMessage(item_NameList,CB_SETCURSEL,selected_item,0);
  set_item_selected(selected_item);
  break;
  }


  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;
   
       if(hdr->code==TTN_NEEDTEXT)
       {
            TOOLTIPTEXT *tooltip=(TOOLTIPTEXT*)lParam;
            tooltip->hinst = hInst;

            switch (hdr->idFrom){
                case WINDOWMENU_FIND:{
                    tooltip->lpszText="Find Item Name";
                    break;}
                case WINDOWMENU_CLOSEROM:{
                    tooltip->lpszText="Close ROM";
                    break;}
                case VIEW_ALPHA:{
                    tooltip->lpszText="List By Alphabetical Order";
                    break;}
                case VIEW_LEVEL:{
                    tooltip->lpszText="List By Type";
                    break;}
                case VIEW_NONE:{
                    tooltip->lpszText="List By Original Order";
                    break;}
                case WINDOWMENU_HELP:{
                    tooltip->lpszText="Help";
                    break;}
            }
        return lParam;
       }

    if((hdr->code==TCN_SELCHANGE)&&(hdr->hwndFrom==tabs))
    {
     int tabsel=TabCtrl_GetCurSel(tabs);
     if(tabsel==0)
     {
     }
    }
   break;
  }

  case WM_COMMAND:
  {
   if((HIWORD(wParam)==BN_CLICKED)&&(lParam==(LPARAM)applybuttn))
     {
      int selected=SendMessage(item_NameList,CB_GETCURSEL,0,0);
      if(selected>-1)apply_item_selected(selected);
     }

   if((HIWORD(wParam)==CBN_SELENDOK)&&(lParam==(LPARAM)item_NameList))
   {
      int selected=SendMessage(item_NameList,CB_GETCURSEL,0,0);
      if(selected>-1)
      {
       set_item_selected(selected);
       selected_item=selected;
      }
   }

//menu-toolbar stuff
   if(HIWORD(wParam)==0)
   {
    switch(LOWORD(wParam))
    {
     case VIEW_ALPHA:
     {
       PlaySound(MAKEINTRESOURCE(1004),hInst,SND_ASYNC|SND_RESOURCE);
       item_sorting=VIEW_ALPHA;
       int numba=items[SendMessage(item_NameList,CB_GETCURSEL,0,0)].number;
       SortItemsAlpha(items);
       refreshitemlist(item_NameList);
       ModifyMenu(item_view_menu,2,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_NONE,"List by &Original Order");
       ModifyMenu(item_view_menu,3,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_ALPHA,"List by Item &Name");
       ModifyMenu(item_view_menu,4,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_LEVEL,"&List by &Type");
       for(int n=0;n<256;n++){if(items[n].number==numba){SendMessage(item_NameList,CB_SETCURSEL,n,0);set_item_selected(n);break;}}
       break;
     }
     case VIEW_LEVEL:
     {
       PlaySound(MAKEINTRESOURCE(1004),hInst,SND_ASYNC|SND_RESOURCE);
       item_sorting=VIEW_LEVEL;
       int numba=items[SendMessage(item_NameList,CB_GETCURSEL,0,0)].number;
       SortItemsType(items);
       refreshitemlist(item_NameList);
       ModifyMenu(item_view_menu,2,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_NONE,"List by &Original Order");
       ModifyMenu(item_view_menu,3,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_ALPHA,"List by Item &Name");
       ModifyMenu(item_view_menu,4,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_LEVEL,"List by &Type");
       for(int n=0;n<256;n++){if(items[n].number==numba){SendMessage(item_NameList,CB_SETCURSEL,n,0);set_item_selected(n);break;}}
       break;
     }
     case VIEW_NONE:
     {
       PlaySound(MAKEINTRESOURCE(1004),hInst,SND_ASYNC|SND_RESOURCE);
       item_sorting=VIEW_NONE;
       int numba=items[SendMessage(item_NameList,CB_GETCURSEL,0,0)].number;
       SortItemsOriginal(items);
       refreshitemlist(item_NameList);
       ModifyMenu(item_view_menu,2,MF_BYPOSITION|MF_STRING|MF_CHECKED,VIEW_NONE,"List by &Original Order");
       ModifyMenu(item_view_menu,3,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_ALPHA,"List by Item &Name");
       ModifyMenu(item_view_menu,4,MF_BYPOSITION|MF_STRING|MF_UNCHECKED,VIEW_LEVEL,"List by &Type");
       SendMessage(item_NameList,CB_SETCURSEL,numba,0);set_item_selected(numba);
       break;
     }
     case VIEW_DUMP:
     {
       if(GetSaveFileName(&dumpofn))
       {
        DumpItems(items,dumpofn.lpstrFile,1);
       }
     break;
     }

     case WINDOWMENU_CLOSEROM:
     {
     SetMenu(hwnd,NULL);
     PlaySound(MAKEINTRESOURCE(1002),hInst,SND_ASYNC|SND_RESOURCE);     
     EndDialog(hwnd,5);
     fclose(rom);
     fileloaded=false;
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
     return false;
     }
     case WINDOWMENU_HELP:
     {
          {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
         char *temp=new char[strlen(line)+15];
         strcpy(temp,line);
//         strcat(temp,"pkfaq.txt");
         WinHelp(hWnd,temp,HELP_FINDER,0);
         ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
         delete temp;
         }
     break;
     }

     case WINDOWMENU_APPLY:
     {
      int selected=SendMessage(item_NameList,CB_GETCURSEL,0,0);
      if(selected>-1)apply_item_selected(selected);
      break;
     }

     case WINDOWMENU_FIND:
     {
      if(DialogBox(hInst,MAKEINTRESOURCE(10003),hWnd,(DLGPROC)SearchProc))
      {
       if(searchstring[0]!=NULL)
       {
        int findings[256];
        int max=SearchItem(items,searchstring,findings);
        int place=0;
        bool oki=true;
        while(oki)
        {
         if(place==max){oki=false;MessageBox(hwnd,"String not found","Done",MB_OK|MB_ICONQUESTION|MB_TASKMODAL);break;}
         SendMessage(item_NameList,CB_SETCURSEL,findings[place],0);
         set_item_selected(findings[place]);
         if(MessageBox(hwnd,"Continue Search?","Search found",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL)==IDNO)oki=false;
         place++;
        }
       }
      }
      break;
     }

    }
   }
//done WM_COMMAND
    break;
   }

  case WM_CLOSE:
  {
    SetMenu(hwnd,NULL);
   EndDialog(hwnd,5);
   break;
  }

  default: return false;
 }
}

bool CALLBACK PSINameEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)butterfly_icon);

  SetMenu(hwnd,PSI_menu);

  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-295;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-(225+(GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU)/2));
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,590,450+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU),SWP_SHOWWINDOW);

    //toolbar stuff
     window_toolbar=CreateWindow(TOOLBARCLASSNAME,"Tools",TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CCS_TOP,0,0,100,100,hwnd,NULL,hInst,NULL);
     CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,33,640,2,hwnd,NULL,hInst,NULL);
     makenofindwindowtoolbar(window_toolbar);

  SetWindowText(hwnd,"PSI Name Editing");

  tabs=CreateWindowEx(0,WC_TABCONTROL,"Tabs",WS_CHILD|WS_VISIBLE,0,40,585,405,hwnd,NULL,hInst,NULL);
  SendMessage(tabs,TCM_SETIMAGELIST,0,(LPARAM)iconlist);
    TC_ITEM tc;
    tc.mask=TCIF_TEXT|TCIF_IMAGE;
    tc.pszText="Names";
    tc.iImage=8;
    TabCtrl_InsertItem(tabs,3,&tc);

//HWND temp;
HWND temp;
  tele_header=CreateWindow(WC_HEADER,"Header",WS_CHILD|HDS_BUTTONS|WS_VISIBLE|HDS_HORZ,105,28+72,399,18,hwnd,NULL,hInst,NULL);
  HD_ITEM item;
  item.mask=HDI_TEXT|HDI_WIDTH|HDI_FORMAT;
  item.cxy=200;
  item.pszText="PSI Name";
  item.fmt=HDF_LEFT;
  Header_InsertItem(tele_header,0,&item);
  SendMessage(tele_header,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));

 psi_name_box=CreateWindow("BUTTON","Names:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,5,32,575,370,tabs,NULL,hInst,NULL);

  for(int n=0;n<16;n++)
  {
   if(n<8){psi_name[n]=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,100,45+(n*19),200,20,psi_name_box,NULL,hInst,NULL);}
   else{psi_name[n]=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,299,45+((n-8)*19),200,20,psi_name_box,NULL,hInst,NULL);}
   SendMessage(psi_name[n],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));
  }
   applybuttn=CreateWindow("BUTTON","Apply Changes",WS_CHILD|WS_VISIBLE|BS_TEXT,465,385,100,30,hwnd,NULL,hInst,NULL);
   SendMessage(applybuttn,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);
   SendMessage(tabs,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(tabs);
   SendMessage(psi_name_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(psi_name_box);

  set_psi_selected(selected_psi);
  return false;
  }

  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;
       if(hdr->code==TTN_NEEDTEXT)
       {
            TOOLTIPTEXT *tooltip=(TOOLTIPTEXT*)lParam;
            tooltip->hinst = hInst;

            switch (hdr->idFrom){
                case WINDOWMENU_CLOSEROM:{
                    tooltip->lpszText="Close ROM";
                    break;}
                case WINDOWMENU_HELP:{
                    tooltip->lpszText="Help";
                    break;}
            }
        return lParam;
       }
   break;
  }

  case WM_COMMAND:
  {
   if((HIWORD(wParam)==BN_CLICKED)&&(lParam==(LPARAM)applybuttn))
     {
      apply_psi_selected(0);
     }

   if(HIWORD(wParam)==0)
   {
    switch(LOWORD(wParam))
    {
     case WINDOWMENU_CLOSEROM:
     {
     SetMenu(hwnd,NULL);
     PlaySound(MAKEINTRESOURCE(1002),hInst,SND_ASYNC|SND_RESOURCE);
     EndDialog(hwnd,5);
     fclose(rom);
     fileloaded=false;
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
     return false;
     }

     case WINDOWMENU_HELP:
     {
          {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
         char *temp=new char[strlen(line)+15];
         strcpy(temp,line);
//         strcat(temp,"pkfaq.txt");
         WinHelp(hWnd,temp,HELP_FINDER,0);
         ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
         delete temp;
         }
     break;
     }
    }
   }
//done WM_COMMAND
  break;
  }

  case WM_CLOSE:
  {
   SetMenu(hwnd,NULL);
   EndDialog(hwnd,5);
   break;
  }

  default: return false;
 }
}

bool CALLBACK TeleEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)diamond_icon);

  SetMenu(hwnd,tele_menu);

  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-295;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-(225+(GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU)/2));
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,590,450+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU),SWP_SHOWWINDOW);

    //toolbar stuff
     window_toolbar=CreateWindow(TOOLBARCLASSNAME,"Tools",TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CCS_TOP,0,0,100,100,hwnd,NULL,hInst,NULL);
     CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,33,640,2,hwnd,NULL,hInst,NULL);
     makenofindwindowtoolbar(window_toolbar);

  SetWindowText(hwnd,"Teleport Editing");

  tabs=CreateWindowEx(0,WC_TABCONTROL,"Tabs",WS_CHILD|WS_VISIBLE,0,40,585,405,hwnd,NULL,hInst,NULL);
  SendMessage(tabs,TCM_SETIMAGELIST,0,(LPARAM)iconlist);
    TC_ITEM tc;
    tc.mask=TCIF_TEXT|TCIF_IMAGE;
    tc.pszText="Teleport";
    tc.iImage=9;
    TabCtrl_InsertItem(tabs,3,&tc);

//HWND temp;
HWND temp;

 tele_main_box=CreateWindow("BUTTON","Teleports:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,5,32,575,370,tabs,NULL,hInst,NULL);
  tele_header=CreateWindow(WC_HEADER,"Header",WS_CHILD|HDS_BUTTONS|WS_VISIBLE|HDS_HORZ,55,23+72,326,18,hwnd,NULL,hInst,NULL);
  HD_ITEM item;
  item.mask=HDI_TEXT|HDI_WIDTH|HDI_FORMAT;
  item.cxy=176;
  item.pszText="Title";
  item.fmt=HDF_LEFT;
  Header_InsertItem(tele_header,0,&item);
  item.cxy=75;
  item.pszText="X";
  Header_InsertItem(tele_header,1,&item);
  item.pszText="Y";
  Header_InsertItem(tele_header,2,&item);
  SendMessage(tele_header,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));

 for(int n=0;n<16;n++){
  tele_name[n]=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,55,72+40+(n*19),176,20,hwnd,NULL,hInst,NULL);
  tele_x[n]=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,230,72+40+(n*19),76,20,hwnd,NULL,hInst,NULL);
  tele_y[n]=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,305,72+40+(n*19),76,20,hwnd,NULL,hInst,NULL);
  SendMessage(tele_name[n],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));
  SendMessage(tele_x[n],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));
  SendMessage(tele_y[n],WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0));
 }

   applybuttn=CreateWindow("BUTTON","Apply Changes",WS_CHILD|WS_VISIBLE|BS_TEXT,465,385,100,30,hwnd,NULL,hInst,NULL);
   SendMessage(applybuttn,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);

   SendMessage(tabs,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(tabs);
   SendMessage(tele_main_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(tele_main_box);

   set_tele_selected(0);
  return false;
  }

  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;
       if(hdr->code==HDN_BEGINTRACK)
       {
        return true;
       }
       if(hdr->code==TTN_NEEDTEXT)
       {
            TOOLTIPTEXT *tooltip=(TOOLTIPTEXT*)lParam;
            tooltip->hinst = hInst;

            switch (hdr->idFrom){
                case WINDOWMENU_CLOSEROM:{
                    tooltip->lpszText="Close ROM";
                    break;}
                case WINDOWMENU_HELP:{
                    tooltip->lpszText="Help";
                    break;}
            }
        return lParam;
       }
   //return true;
   break;
  }

  case WM_COMMAND:
  {
   if((HIWORD(wParam)==BN_CLICKED)&&(lParam==(LPARAM)applybuttn))
     {
       apply_tele_selected(0);
     }

   if(HIWORD(wParam)==0)
   {
    switch(LOWORD(wParam))
    {
     case WINDOWMENU_CLOSEROM:
     {
     SetMenu(hwnd,NULL);
     PlaySound(MAKEINTRESOURCE(1002),hInst,SND_ASYNC|SND_RESOURCE);     
     EndDialog(hwnd,5);
     fclose(rom);
     fileloaded=false;
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
     return false;
     }

     case WINDOWMENU_HELP:
     {
          {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
         char *temp=new char[strlen(line)+15];
         strcpy(temp,line);
//         strcat(temp,"pkfaq.txt");
         WinHelp(hWnd,temp,HELP_FINDER,0);
         ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
         delete temp;
         }
     break;
     }
    }
   }
//done WM_COMMAND
  break;
  }

  case WM_CLOSE:
  {
   SetMenu(hwnd,NULL);
   EndDialog(hwnd,5);
   break;
  }

  default: return false;
 }
}


bool CALLBACK MiscTextEditProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
  SendMessage(hwnd,WM_SETICON,ICON_SMALL,(LPARAM)sign_icon);

  SetMenu(hwnd,tele_menu);

  int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-295;
  int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-(225+(GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU)/2));
  SetWindowPos(hwnd,HWND_TOP,xpos,ypos,590,450+GetSystemMetrics(SM_CYCAPTION)+GetSystemMetrics(SM_CYMENU),SWP_SHOWWINDOW);

    //toolbar stuff
     window_toolbar=CreateWindow(TOOLBARCLASSNAME,"Tools",TBSTYLE_TOOLTIPS|WS_CHILD|WS_VISIBLE|CCS_TOP,0,0,100,100,hwnd,NULL,hInst,NULL);
     CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE|SS_ETCHEDFRAME,-5,33,640,2,hwnd,NULL,hInst,NULL);
     makenofindwindowtoolbar(window_toolbar);

  SetWindowText(hwnd,"Misc. Text Editing");

  tabs=CreateWindowEx(0,WC_TABCONTROL,"Tabs",WS_CHILD|WS_VISIBLE,0,40,585,405,hwnd,NULL,hInst,NULL);
  SendMessage(tabs,TCM_SETIMAGELIST,0,(LPARAM)iconlist);
    TC_ITEM tc;
    tc.mask=TCIF_TEXT|TCIF_IMAGE;
    tc.pszText="Misc. Text";
    tc.iImage=11;
    TabCtrl_InsertItem(tabs,3,&tc);

//HWND temp;
HWND temp;

 misc_main_box=CreateWindow("BUTTON","Misc. Text:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,5,32,575,370,tabs,NULL,hInst,NULL);

 temp=CreateWindow("BUTTON","Phone List:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,75,95-40,475,90,tabs,NULL,hInst,NULL); SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
  misc_phone_list=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,94,114,210,150,hwnd,NULL,hInst,NULL);
   misc_phone_edit=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,320,115,160,20,hwnd,NULL,hInst,NULL);

  temp=CreateWindow("STATIC","Pointer:",WS_CHILD|WS_VISIBLE,200,141,90,19,hwnd,NULL,hInst,NULL);  SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
   misc_phone_pointer=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,320,141,160,20,hwnd,NULL,hInst,NULL);

 temp=CreateWindow("BUTTON","Ailment Names:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,75,190-40,475,65,tabs,NULL,hInst,NULL); SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
  misc_ail_list=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,94,209,210,150,hwnd,NULL,hInst,NULL);
   misc_ail_edit=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,320,210,160,20,hwnd,NULL,hInst,NULL);

 temp=CreateWindow("BUTTON","Battle Menu:",WS_CHILD|WS_VISIBLE|BS_GROUPBOX,75,260-40,475,65,tabs,NULL,hInst,NULL); SendMessage(temp,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true,0));
  misc_menu_list=CreateWindow("COMBOBOX","",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL,94,279,210,150,hwnd,NULL,hInst,NULL);
   misc_menu_edit=CreateWindow("EDIT","",WS_CHILD|WS_TABSTOP|ES_AUTOHSCROLL|WS_BORDER|WS_VISIBLE,320,280,160,20,hwnd,NULL,hInst,NULL);

   applybuttn=CreateWindow("BUTTON","Apply Changes",WS_CHILD|WS_VISIBLE|BS_TEXT,465,385,100,30,hwnd,NULL,hInst,NULL);
   SendMessage(applybuttn,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(applybuttn);

   SendMessage(tabs,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(tabs);
   SendMessage(misc_main_box,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_main_box);
   SendMessage(misc_phone_list,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_phone_list);
   SendMessage(misc_phone_edit,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_phone_edit);
   SendMessage(misc_phone_pointer,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_phone_pointer);
   SendMessage(misc_ail_list,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_ail_list);
   SendMessage(misc_ail_edit,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_ail_edit);
   SendMessage(misc_menu_list,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_menu_list);
   SendMessage(misc_menu_edit,WM_SETFONT,(WPARAM)fonty,MAKELPARAM(true, 0)); Ctl3dSubclassCtl(misc_menu_edit);

   refreshphonelist(misc_phone_list);
   refreshaillist(misc_ail_list);
   refreshmenulist(misc_menu_list);

   SendMessage(misc_phone_list,CB_SETCURSEL,0,0);
   SendMessage(misc_ail_list,CB_SETCURSEL,0,0);
   SendMessage(misc_menu_list,CB_SETCURSEL,0,0);
   set_misc_selected(0);
  return false;
  }

  case WM_NOTIFY:
  {
   NMHDR *hdr=(NMHDR*)lParam;
       if(hdr->code==TTN_NEEDTEXT)
       {
            TOOLTIPTEXT *tooltip=(TOOLTIPTEXT*)lParam;
            tooltip->hinst = hInst;

            switch (hdr->idFrom){
                case WINDOWMENU_CLOSEROM:{
                    tooltip->lpszText="Close ROM";
                    break;}
                case WINDOWMENU_HELP:{
                    tooltip->lpszText="Help";
                    break;}
            }
        return lParam;
       }
   break;
  }

  case WM_COMMAND:
  {
   if((HIWORD(wParam)==BN_CLICKED)&&(lParam==(LPARAM)applybuttn))
     {
       apply_misc_selected(0);      
     }
   if(HIWORD(wParam)==CBN_SELENDOK)
   {
    if((lParam==(LPARAM)misc_phone_list)||(lParam==(LPARAM)misc_ail_list)||(lParam==(LPARAM)misc_menu_list))
    set_misc_selected(0);
   }


   if(HIWORD(wParam)==0)
   {
    switch(LOWORD(wParam))
    {
     case WINDOWMENU_CLOSEROM:
     {
     SetMenu(hwnd,NULL);
     PlaySound(MAKEINTRESOURCE(1002),hInst,SND_ASYNC|SND_RESOURCE);     
     EndDialog(hwnd,5);
     fclose(rom);
     fileloaded=false;
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
     return false;
     }

     case WINDOWMENU_HELP:
     {
          {
          char *liney=GetCommandLine();
          char *line=new char[strlen(liney)+1];
          strcpy(line,liney);
          char commandy[MAX_PATH];
          while(line[strlen(line)-1]!='\\')
          {
          line[strlen(line)-1]=NULL;
          }
         line++;
         char *temp=new char[strlen(line)+15];
         strcpy(temp,line);
//         strcat(temp,"pkfaq.txt");
         WinHelp(hWnd,temp,HELP_FINDER,0);
         ShellExecute(hWnd,"open",temp,NULL,"C:\\",SW_SHOW);
         delete temp;
         }
     break;
     }
    }
   }
//done WM_COMMAND
  break;
  }

  case WM_CLOSE:
  {
   SetMenu(hwnd,NULL);
   EndDialog(hwnd,5);
   break;
  }

  default: return false;
 }
}



//---------------Setselected functions

void set_enemy_selected(int select)
{

// DrawSprite(50,200,sprConv[enemies[select].outsidePic],enemy_graphics_box);

   SetWindowText(enemy_Name,enemies[select].name);
   SetWinCap(enemy_Hp,enemies[select].hp);
   SetWinCap(enemy_Pp,enemies[select].pp);
   SetWinCap(enemy_Exp,enemies[select].exp);
   SetWinCap(enemy_Money,enemies[select].money);
   SetWinCap(enemy_Speed,enemies[select].speed);
   SetWinCap(enemy_Palette,enemies[select].palette);
   SetWinCap(enemy_Offense,enemies[select].offense);
   SetWinCap(enemy_Defense,enemies[select].defense);
   for(int n=0;n<256;n++)
   {
    if(items[n].number==enemies[select].item)SendMessage(enemy_Item,CB_SETCURSEL,n,0);
   }
   SendMessage(enemy_Freq,CB_SETCURSEL,enemies[select].freq,0);
   SetWinCap(enemy_Status,enemies[select].status);
   SetWinCap(enemy_Level,enemies[select].level);
   SetWinCap(enemy_Guts,enemies[select].guts);
   SetWinCap(enemy_Iq,enemies[select].iq);
   SetWinCap(enemy_InsidePic,enemies[select].insidePic);
//   SetWinCap(enemy_OutsidePic,enemies[select].outsidePic);

    SendMessage(enemy_selector,CB_SETCURSEL,enemies[select].outsidePic,0);

   if(!enemies[select].theFlag){SendMessage(enemy_TheFlag,BM_SETCHECK,BST_UNCHECKED,0);}
   else{SendMessage(enemy_TheFlag,BM_SETCHECK,BST_CHECKED,0);}
   SetWinCap(enemy_Movement,enemies[select].movement);
   SetWinCap(enemy_MissRate,enemies[select].missRate);
   SendMessage(enemy_DieSound,CB_SETCURSEL,enemies[select].dieSound,0);
   SendMessage(enemy_Type,CB_SETCURSEL,enemies[select].type,0);
   SendMessage(enemy_Music,CB_SETCURSEL,enemies[select].music,0);

//actions and friends
 for(int n=0;n<4;n++)
 {
  int thingy=-1;
  SendMessage(enemy_Action[n],CB_SETCURSEL,enemies[select].action[n],0);
  if(actionType[enemies[select].action[n]]==TYPE_NORMAL){SendMessage(enemy_Friend[n],CB_RESETCONTENT,0,0);}
  if(actionType[enemies[select].action[n]]==TYPE_PSI){refreshpsipowerlist(enemy_Friend[n]);SendMessage(enemy_Friend[n],CB_SETCURSEL,enemies[select].amigo[n]-1,0);}
   if(actionType[enemies[select].action[n]]==TYPE_CALL){
     refreshenemylist(enemy_Friend[n]);
     for(int i=0;i<230;i++){
     if(enemies[select].amigo[n]==0)break;
     if(enemies[i].number==enemies[select].amigo[n]-1){thingy=i;break;}}
     SendMessage(enemy_Friend[n],CB_SETCURSEL,thingy,0);
   }
   if(actionType[enemies[select].action[n]]==TYPE_ITEM){
   refreshitemlist(enemy_Friend[n]);
     for(int i=0;i<256;i++){
     if(enemies[select].amigo[n]==0)break;
     if(items[i].number==enemies[select].amigo[n]){thingy=i;break;}}  //no -1 here, guess the 0 is fine with items...
     SendMessage(enemy_Friend[n],CB_SETCURSEL,thingy,0);
   }
 }

 SendMessage(enemy_FinalAction,CB_SETCURSEL,enemies[select].finalAction,0);
 SendMessage(enemy_Order,CB_SETCURSEL,enemies[select].order,0);
 SetWinCap(enemy_MaxCall,enemies[select].maxCall);
 SetWinHexCap(enemy_StartAction,enemies[select].startAddress);
 SetWinHexCap(enemy_DieAction,enemies[select].dieAddress);
}

void set_item_selected(int select)
{
 SetWindowText(item_Name,items[select].name);
 SetWinCap(item_Type,items[select].type);
 SetWinCap(item_Cost,items[select].cost);
 SendMessage(item_Effect,CB_SETCURSEL,items[select].effect,0);
 SetWinSignedCap(item_Strength,items[select].strength);
 SetWinCap(item_Increase,items[select].increase);
 SetWinSignedCap(item_ExtraPower,items[select].extraPower);
 SetWinCap(item_SpecialProperties,items[select].specialProperties);
 SetWinHexCap(item_DescAddress,items[select].descAddress);

 if(items[select].ownership&128){SendMessage(item_Ownership[0],BM_SETCHECK,BST_UNCHECKED,0);}else{SendMessage(item_Ownership[0],BM_SETCHECK,BST_CHECKED,0);}
 if(items[select].ownership&96){SendMessage(item_Ownership[1],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_Ownership[1],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&16){SendMessage(item_Ownership[2],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_Ownership[2],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&8){SendMessage(item_Ownership[6],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_Ownership[6],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&4){SendMessage(item_Ownership[5],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_Ownership[5],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&2){SendMessage(item_Ownership[4],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_Ownership[4],BM_SETCHECK,BST_UNCHECKED,0);}
 if(items[select].ownership&1){SendMessage(item_Ownership[3],BM_SETCHECK,BST_CHECKED,0);}else{SendMessage(item_Ownership[3],BM_SETCHECK,BST_UNCHECKED,0);}
}

void set_psi_selected(int select)
{
 for(int n=0;n<16;n++)
 {
 SetWindowText(psi_name[n],psis[n].name);
 }
}

void set_tele_selected(int select)
{
 for(int n=0;n<16;n++)
 {
  SetWindowText(tele_name[n],teles[n].name);
  SetWinCap(tele_x[n],teles[n].x);
  SetWinCap(tele_y[n],teles[n].y);
 }
}

void set_misc_selected(int select)
{
 select=SendMessage(misc_phone_list,CB_GETCURSEL,0,0);
  SetWindowText(misc_phone_edit,phones[select].name);
  SetWinHexCap(misc_phone_pointer,phones[select].pointer);
 select=SendMessage(misc_ail_list,CB_GETCURSEL,0,0);
  SetWindowText(misc_ail_edit,ails[select].name);
 select=SendMessage(misc_menu_list,CB_GETCURSEL,0,0);
  SetWindowText(misc_menu_edit,bmenus[select].name);
}
//-------------------

//Applyselected functions
void apply_enemy_selected(int select)
{
 enemies[select].ChangeHp(rom,junkstuff(enemy_Hp,65535));
 enemies[select].ChangePp(rom,junkstuff(enemy_Pp,65535));
 enemies[select].ChangeExp(rom,junkstuff(enemy_Exp,4294967295));
 enemies[select].ChangeMoney(rom,junkstuff(enemy_Money,65535));
 enemies[select].ChangeSpeed(rom,junkstuff(enemy_Speed,255));
 enemies[select].ChangeOffense(rom,junkstuff(enemy_Offense,255));
 enemies[select].ChangeDefense(rom,junkstuff(enemy_Defense,255));
 enemies[select].ChangeStatus(rom,junkstuff(enemy_Status,255));
 enemies[select].ChangeLevel(rom,junkstuff(enemy_Level,255));
 enemies[select].ChangeGuts(rom,junkstuff(enemy_Guts,255));
 enemies[select].ChangeIq(rom,junkstuff(enemy_Iq,255));
 enemies[select].ChangeMovement(rom,junkstuff(enemy_Movement,255));
 enemies[select].ChangeItem(rom,items[SendMessage(enemy_Item,CB_GETCURSEL,0,0)].number);
 enemies[select].ChangeFreq(rom,SendMessage(enemy_Freq,CB_GETCURSEL,0,0));
  int theflag=SendMessage(enemy_TheFlag,BM_GETCHECK,0,0);
  if(theflag==BST_CHECKED){enemies[select].ChangeTheFlag(rom,1);}else{enemies[select].ChangeTheFlag(rom,0);}
 enemies[select].ChangePalette(rom,junkstuff(enemy_Palette,255));
 enemies[select].ChangeInsidePic(rom,junkstuff(enemy_InsidePic,255));
// enemies[select].ChangeOutsidePic(rom,junkstuff(enemy_OutsidePic,65535));
  enemies[select].ChangeOutsidePic(rom,SendMessage(enemy_selector,CB_GETCURSEL,0,0));
 enemies[select].ChangeMissRate(rom,junkstuff(enemy_MissRate,255));
 enemies[select].ChangeDieSound(rom,SendMessage(enemy_DieSound,CB_GETCURSEL,0,0));
 enemies[select].ChangeType(rom,SendMessage(enemy_Type,CB_GETCURSEL,0,0));
 enemies[select].ChangeMusic(rom,SendMessage(enemy_Music,CB_GETCURSEL,0,0));

//act stuff
 for(int i=0;i<4;i++)
 {
  enemies[select].ChangeAction(rom,SendMessage(enemy_Action[i],CB_GETCURSEL,0,0),i);

   int joe=SendMessage(enemy_Action[i],CB_GETCURSEL,0,0);
   int chosen=SendMessage(enemy_Friend[i],CB_GETCURSEL,0,0);
    if(actionType[joe]==TYPE_NORMAL){}
    if(actionType[joe]==TYPE_PSI){if(chosen==-1){enemies[select].ChangeFriend(rom,i,0);}else{enemies[select].ChangeFriend(rom,i,chosen+1);}}
     if(actionType[joe]==TYPE_CALL){
       int thingy=SendMessage(enemy_Friend[i],CB_GETCURSEL,0,0);
       enemies[select].ChangeFriend(rom,i,enemies[thingy].number+1);
     }
     if(actionType[joe]==TYPE_ITEM){
       int thingy=SendMessage(enemy_Friend[i],CB_GETCURSEL,0,0);
       if(thingy==-1){enemies[select].ChangeFriend(rom,i,0);}else
       {enemies[select].ChangeFriend(rom,i,items[thingy].number);}
     }
 }

 enemies[select].ChangeFinalAction(rom,SendMessage(enemy_FinalAction,CB_GETCURSEL,0,0));
 enemies[select].ChangeOrder(rom,SendMessage(enemy_Order,CB_GETCURSEL,0,0));
 enemies[select].ChangeMaxCall(rom,junkstuff(enemy_MaxCall,255));
  //silly address stuff, hex/dec
 char *thing=new char[25];
 unsigned long result=0;
  GetWindowText(enemy_StartAction,thing,25);
  if(thing[0]=='$')
  {
   thing++;
   result=strtoul(thing,NULL,16);
   thing--;
  }
  else
  {
   result=strtoul(thing,NULL,10);
  }
 enemies[select].ChangeStartAction(rom,result);
   GetWindowText(enemy_DieAction,thing,25);
   if(thing[0]=='$')
   {
    thing++;
    result=strtoul(thing,NULL,16);
    thing--;
   }
   else
   {
    result=strtoul(thing,NULL,10);
   }
 enemies[select].ChangeDieAction(rom,result);
 delete thing;

//if they changed the name, reset namelist and find place again
 char name[25];
 GetWindowText(enemy_Name,name,25);
 enemies[select].ChangeName(rom,name);

int something=select;
int numba=enemies[select].number;
if(enemy_sorting==VIEW_ALPHA){SortEnemiesAlpha(enemies);for(int n=0;n<230;n++){if(enemies[n].number==numba){something=n;break;}}}
if(enemy_sorting==VIEW_LEVEL){SortEnemiesGameOrder(enemies);for(int n=0;n<230;n++){if(enemies[n].number==numba){something=n;break;}}}
if(enemy_sorting==VIEW_NONE){SortEnemiesOriginal(enemies);}

 refreshenemylist(enemy_NameList);
 SendMessage(enemy_NameList,CB_SETCURSEL,something,0);
 set_enemy_selected(something);
}

void apply_item_selected(int select)
{
 items[select].ChangeCost(rom,junkstuff(item_Cost,65525));
 items[select].ChangeType(rom,junkstuff(item_Type,255));
 items[select].ChangeEffect(rom,SendMessage(item_Effect,CB_GETCURSEL,0,0));
 items[select].ChangeStrength(rom,junkstuffsigned(item_Strength,127));
 items[select].ChangeIncrease(rom,junkstuff(item_Increase,255));
 items[select].ChangeExtraPower(rom,junkstuffsigned(item_ExtraPower,255));
 items[select].ChangeSpecialProperties(rom,junkstuff(item_SpecialProperties,255));

 short owner=0;
 if(SendMessage(item_Ownership[0],BM_GETCHECK,0,0)==BST_UNCHECKED)owner|=128;
 if(SendMessage(item_Ownership[1],BM_GETCHECK,0,0)==BST_CHECKED)owner|=96;
 if(SendMessage(item_Ownership[2],BM_GETCHECK,0,0)==BST_CHECKED)owner|=16;
 if(SendMessage(item_Ownership[6],BM_GETCHECK,0,0)==BST_CHECKED)owner|=8;
 if(SendMessage(item_Ownership[5],BM_GETCHECK,0,0)==BST_CHECKED)owner|=4;
 if(SendMessage(item_Ownership[4],BM_GETCHECK,0,0)==BST_CHECKED)owner|=2;
 if(SendMessage(item_Ownership[3],BM_GETCHECK,0,0)==BST_CHECKED)owner|=1;
 items[select].ChangeOwnership(rom,owner);

 char *thing=new char[25];
 unsigned long result=0;
  GetWindowText(item_DescAddress,thing,25);
  if(thing[0]=='$')
  {
   thing++;
   result=strtoul(thing,NULL,16);
   thing--;
  }
  else
  {
   result=strtoul(thing,NULL,10);
  }
 delete thing;

 items[select].ChangeDescAddress(rom,result);


 int something=select;
 char name[25];
 GetWindowText(item_Name,name,25);
 items[select].ChangeName(rom,name);

 int numba=items[select].number;
 if(item_sorting==VIEW_ALPHA){SortItemsAlpha(items);for(int n=0;n<256;n++){if(items[n].number==numba){something=n;break;}}}
 if(item_sorting==VIEW_LEVEL){SortItemsType(items);for(int n=0;n<256;n++){if(items[n].number==numba){something=n;break;}}}
 if(item_sorting==VIEW_NONE){SortItemsOriginal(items);}
 refreshitemlist(item_NameList);
 SendMessage(item_NameList,CB_SETCURSEL,something,0);
 set_item_selected(something);
}

void apply_psi_selected(int select)
{
 for(int n=0;n<16;n++)
 {
 char name[25];
 GetWindowText(psi_name[n],name,25);
 psis[n].ChangeName(rom,name);
 }
 set_psi_selected(select);
}

void apply_tele_selected(int select)
{
 for(int n=0;n<16;n++)
 {
  char name[25];
  GetWindowText(tele_name[n],name,25);
  teles[n].ChangeName(rom,name);
  teles[n].ChangeX(rom,junkstuff(tele_x[n],65535));
  teles[n].ChangeY(rom,junkstuff(tele_y[n],65535));
 }
 set_tele_selected(0);
}

void apply_misc_selected(int select)
{
 select=SendMessage(misc_phone_list,CB_GETCURSEL,0,0);
  char *thing=new char[25];
  GetWindowText(misc_phone_edit,thing,25);
  phones[select].ChangeName(rom,thing);

  unsigned long result=0;
  GetWindowText(misc_phone_pointer,thing,25);
  if(thing[0]=='$')
  {
   thing++;
   result=strtoul(thing,NULL,16);
   thing--;
  }
  else
  {
   result=strtoul(thing,NULL,10);
  }
  phones[select].ChangePointer(rom,result);

 select=SendMessage(misc_ail_list,CB_GETCURSEL,0,0);
  GetWindowText(misc_ail_edit,thing,25);
  ails[select].ChangeName(rom,thing);

 select=SendMessage(misc_menu_list,CB_GETCURSEL,0,0);
  GetWindowText(misc_menu_edit,thing,25);
  bmenus[select].ChangeName(rom,thing);
  delete thing;

 int phonei=SendMessage(misc_phone_list,CB_GETCURSEL,0,0);
 int aili=SendMessage(misc_ail_list,CB_GETCURSEL,0,0);
 int menui=SendMessage(misc_menu_list,CB_GETCURSEL,0,0);
 refreshphonelist(misc_phone_list);
 refreshaillist(misc_ail_list);
 refreshmenulist(misc_menu_list);
 SendMessage(misc_phone_list,CB_SETCURSEL,phonei,0);
 SendMessage(misc_ail_list,CB_SETCURSEL,aili,0);
 SendMessage(misc_menu_list,CB_SETCURSEL,menui,0);
 set_misc_selected(0);
}
//------------

void refreshenemylist(HWND window)
{
  SendMessage(window,CB_RESETCONTENT,0,0);
   for(int n=0;n<230;n++)
   {
    char joe[50];
    char number[10];
    strcpy(joe, enemies[n].name);
    strcat(joe," - [");
    itoa(enemies[n].number,number,10);
    strcat(joe,number);
    strcat(joe,"]");

    SendMessage(window,CB_ADDSTRING,0,(LPARAM)joe);
   }
}

void refreshitemlist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
   for(int n=0;n<256;n++)
   {
    char joe[50];
    char number[10];
    strcpy(joe, items[n].name);
    strcat(joe," - [");
    itoa(items[n].number,number,10);
    strcat(joe,number);
    strcat(joe,"]");
    SendMessage(window,CB_ADDSTRING,0,(LPARAM)joe);
   }
}

void refresheffectlist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
 for(int n=0;n<319;n++)
 {
  SendMessage(window,CB_ADDSTRING,0,(LPARAM)action[n]);
 }
}

void refreshpsipowerlist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
 for(int n=0;n<52;n++)
 {
  SendMessage(window,CB_ADDSTRING,0,(LPARAM)psinames[n]);
 }
}

void refreshpsinamelist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
   for(int n=0;n<16;n++)
   {
    char joe[50];
    char number[10];
    strcpy(joe, psis[n].name);
    strcat(joe," - [");
    itoa(n,number,10);
    strcat(joe,number);
    strcat(joe,"]");
    SendMessage(window,CB_ADDSTRING,0,(LPARAM)joe);
   }
}

void refreshphonelist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
 for(int n=0;n<5;n++)
 {
  SendMessage(window,CB_ADDSTRING,0,(LPARAM)phones[n].name);
 }
}

void refreshaillist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
 for(int n=0;n<10;n++)
 {
  SendMessage(window,CB_ADDSTRING,0,(LPARAM)ails[n].name);
 }
}

void refreshmenulist(HWND window)
{
SendMessage(window,CB_RESETCONTENT,0,0);
 for(int n=0;n<11;n++)
 {
  SendMessage(window,CB_ADDSTRING,0,(LPARAM)bmenus[n].name);
 }
}


void refreshrecent(void)
{
 for(int n=0;n<5;n++)
 {
  char buf[10];
  itoa(n+1,buf,10);
  char thing[1045];
  strcpy(thing,"&");
  strcat(thing,buf);
  strcat(thing," - ");
  strcat(thing,&recent[n][0]);
  ModifyMenu(filemenu,3+n,MF_BYPOSITION|MF_STRING,100+n,thing);
 }
}


void tryopenfile(char *openfile)
{
      if(fileloaded){fclose(rom);fileloaded=false;}
      rom=fopen(openfile,"r+b");
      if(!rom){MessageBox(hWnd,"Error reading file.", "Warning",MB_OK|MB_ICONWARNING|MB_TASKMODAL);return;}
      else
      {
       PlaySound(MAKEINTRESOURCE(1001),hInst,SND_ASYNC|SND_RESOURCE);
       int type=IsCorrectRom(rom);
       if(type==0)
        {
         romtype=0;
         char *temp=new char[strlen(openfile)+150];
         strcpy(temp,openfile);
         strcat(temp,"\n\n");
         strcat(temp," This is an unrecognized ROM image.  Please be aware that editing this ROM will most likely make it unplayable.  Make sure to create a backup before editing.");
         MessageBox(hWnd,temp, "Warning",MB_OK|MB_ICONWARNING|MB_TASKMODAL);
         delete temp;
        }

       if(type==1){romtype=1;}
       if(type==2){MessageBox(hWnd,"You have loaded a Mother 2 ROM.  However, the current version of PK Hack is incompatible with it. Any editing you do to this ROM will caused unexpected results.  Make sure to create a backup before editing.\n\nあいにくですが、現在、MOTHER2のROM\nは無理です。将来、編集できるように\nなるかもしれないから、楽しみにして下さい!", "Warning",MB_OK|MB_ICONWARNING|MB_TASKMODAL); romtype=2;}
       fileloaded=true;
       PullEnemies(rom,enemies);
       if(enemy_sorting==VIEW_ALPHA)SortEnemiesAlpha(enemies);
       if(enemy_sorting==VIEW_LEVEL)SortEnemiesGameOrder(enemies);
       if(enemy_sorting==VIEW_NONE)SortEnemiesOriginal(enemies);

       PullItems(rom,items);
       if(item_sorting==VIEW_ALPHA)SortItemsAlpha(items);
       if(item_sorting==VIEW_LEVEL)SortItemsType(items);
       if(item_sorting==VIEW_NONE)SortItemsOriginal(items);


       PullPsi(rom,psis);

       PullTeleport(rom,teles);

       PullPhone(rom,phones);
       PullAilments(rom,ails);
       PullBattleMenu(rom,bmenus);

       romsize=FileSize(rom);

       bool inlist=false;
       int inlistpos=0;
       for(int n=0;n<5;n++)
       {
        if(strcmp(openfile,recent[n])==0)
        {
         inlist=true;
         inlistpos=n;
         break;
        }
       }

       if(!inlist)
       {
        char *temp=recent[4];
        recent[4]=recent[3];
        recent[3]=recent[2];
        recent[2]=recent[1];
        recent[1]=recent[0];
        recent[0]=temp;
        strcpy(recent[0],openfile);
       }
       else
       {
        char *temp=recent[inlistpos];
        for(int n=inlistpos;n>0;n--)
        {
         recent[n]=recent[n-1];
        }
        recent[0]=temp;
       }

       refreshrecent();
      }
    //refresh shown stats
     NMHDR hdr;
     hdr.code=TVN_SELCHANGED;
     hdr.hwndFrom=tree;
     SendMessage(hWnd,WM_NOTIFY,0,(LPARAM)&hdr);
}

RECT makerect(int a, int b, int x, int y)
{
 RECT rect;
 rect.left=a;
 rect.top=b;
 rect.right=x+a;
 rect.bottom=y+b;
 return rect;
}

void makewindowtoolbar(HWND window)
{
   SendMessage(window,TB_SETBITMAPSIZE,0,MAKELONG(20,20));
   SendMessage(window,TB_SETBUTTONSIZE,0,MAKELONG(26,27));
   SendMessage(window,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);

   TBADDBITMAP addbmp;
    addbmp.hInst=hInst;
    addbmp.nID=101;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=102;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=103;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=104;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=105;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=109;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=110;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=111;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);

   TBBUTTON button[8];

    button[0].iBitmap=1;
    button[0].idCommand=WINDOWMENU_CLOSEROM;
    button[0].fsState=TBSTATE_ENABLED;
    button[0].fsStyle=TBSTYLE_BUTTON;
    button[0].iString=0;

    button[1].iBitmap=2;
    button[1].idCommand=WINDOWMENU_FIND;
    button[1].fsState=TBSTATE_ENABLED;
    button[1].fsStyle=TBSTYLE_BUTTON;
    button[1].iString=0;

    button[2].iBitmap=0;
    button[2].idCommand=0;
    button[2].fsState=TBSTATE_ENABLED;
    button[2].fsStyle=TBSTYLE_SEP;
    button[2].iString=0;

    button[3].iBitmap=6;
    button[3].idCommand=VIEW_NONE;
    button[3].fsState=TBSTATE_ENABLED;
    button[3].fsStyle=TBSTYLE_BUTTON;
    button[3].iString=0;
    button[4].iBitmap=5;
    button[4].idCommand=VIEW_ALPHA;
    button[4].fsState=TBSTATE_ENABLED;
    button[4].fsStyle=TBSTYLE_BUTTON;
    button[4].iString=0;
    button[5].iBitmap=7;
    button[5].idCommand=VIEW_LEVEL;
    button[5].fsState=TBSTATE_ENABLED;
    button[5].fsStyle=TBSTYLE_BUTTON;
    button[5].iString=0;

    button[6].iBitmap=0;
    button[6].idCommand=0;
    button[6].fsState=TBSTATE_ENABLED;
    button[6].fsStyle=TBSTYLE_SEP;
    button[6].iString=0;

    button[7].iBitmap=4;
    button[7].idCommand=WINDOWMENU_HELP;
    button[7].fsState=TBSTATE_ENABLED;
    button[7].fsStyle=TBSTYLE_BUTTON;
    button[7].iString=0;

    SendMessage(window,TB_ADDBUTTONS,8,(LPARAM)button);

    SendMessage(window,WM_SIZE,0,0);
}

void makenofindwindowtoolbar(HWND window)
{
   SendMessage(window,TB_SETBITMAPSIZE,0,MAKELONG(20,20));
   SendMessage(window,TB_SETBUTTONSIZE,0,MAKELONG(26,27));
   SendMessage(window,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);

   TBADDBITMAP addbmp;
    addbmp.hInst=hInst;
    addbmp.nID=101;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=102;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=103;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=104;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);
    addbmp.nID=105;
    SendMessage(window,TB_ADDBITMAP,1,(LPARAM)&addbmp);

   TBBUTTON button[3];

    button[0].iBitmap=1;
    button[0].idCommand=WINDOWMENU_CLOSEROM;
    button[0].fsState=TBSTATE_ENABLED;
    button[0].fsStyle=TBSTYLE_BUTTON;
    button[0].iString=0;

    button[1].iBitmap=0;
    button[1].idCommand=0;
    button[1].fsState=TBSTATE_ENABLED;
    button[1].fsStyle=TBSTYLE_SEP;
    button[1].iString=0;

    button[2].iBitmap=4;
    button[2].idCommand=WINDOWMENU_HELP;
    button[2].fsState=TBSTATE_ENABLED;
    button[2].fsStyle=TBSTYLE_BUTTON;
    button[2].iString=0;

    SendMessage(window,TB_ADDBUTTONS,3,(LPARAM)button);

    SendMessage(window,WM_SIZE,0,0);
}


bool CALLBACK SearchProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
   return false;
  }

  case WM_COMMAND:
  {
   if(HIWORD(wParam)==BN_CLICKED)
   {
    if(LOWORD(wParam)==2)
    {
     GetDlgItemText(hwnd,1,searchstring,25);
     EndDialog(hwnd,1);
    }
    if(LOWORD(wParam)==3)
    {
     EndDialog(hwnd,0);
    }
   }
   return false;
  }

  case WM_CLOSE:
  {
   EndDialog(hwnd,0);
   return false;
  }

  default: return false;
 }
}

bool CALLBACK MakeIPSProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
   return false;
  }

  case WM_COMMAND:
  {
   if(HIWORD(wParam)==BN_CLICKED)
   {
    if(LOWORD(wParam)==2)
    {
     if(GetOpenFileName(&ofn)){SetDlgItemText(hwnd,1,filename);}
    }

    if(LOWORD(wParam)==4)
    {
     if(GetSaveFileName(&ipsofn)){SetDlgItemText(hwnd,3,ipsofn.lpstrFile);}
    }

    if(LOWORD(wParam)==5)
    {
     GetDlgItemText(hwnd,1,romname,MAX_PATH);
     GetDlgItemText(hwnd,3,ipsname,MAX_PATH);
     if((ipsname[0]!=NULL)&&(romname[0]!=0))
     {
      DWORD id;
      HANDLE thing=CreateThread(NULL,4096,(LPTHREAD_START_ROUTINE)IPSMakingThread,NULL,NULL,&id);
      DialogBox(hInst,MAKEINTRESOURCE(10005),hwnd,(DLGPROC)WaitingProc);
     }
    }

    if(LOWORD(wParam)==6)
    {
     EndDialog(hwnd,0);
    }
   }
   return false;
  }

  case WM_CLOSE:
  {
   EndDialog(hwnd,0);
   return false;
  }

  default: return false;
 }
}

bool CALLBACK WaitingProc(HWND hwnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
 switch(wMsg)
 {
  case WM_INITDIALOG:
  {
    int xpos=GetSystemMetrics(SM_CXFULLSCREEN)/2-150;
    int ypos=GetSystemMetrics(SM_CYFULLSCREEN)/2-100;

   SetWindowPos(hwnd,HWND_TOP,xpos,ypos,150,100,SWP_SHOWWINDOW);
   HBITMAP stand=LoadBitmap(hInst,MAKEINTRESOURCE(106));
   HBITMAP walk=LoadBitmap(hInst,MAKEINTRESOURCE(107));
   HDC joe=GetDC(hwnd);
    spaanr=CreateCompatibleBitmap(joe,100,24);
    spaanstepr=CreateCompatibleBitmap(joe,100,24);
    HDC spaanrdc=CreateCompatibleDC(joe);
    HDC spaansteprdc=CreateCompatibleDC(joe);
    HDC standdc=CreateCompatibleDC(joe);
    HDC walkdc=CreateCompatibleDC(joe);
     SelectObject(spaanrdc,spaanr);
     SelectObject(spaansteprdc,spaanstepr);
     SelectObject(standdc,stand);
     SelectObject(walkdc,walk);
      for(int n=-12;n<100;n+=16)
      {
       BitBlt(spaanrdc,n,0,16,24,standdc,0,0,SRCCOPY);
       BitBlt(spaansteprdc,n,0,16,24,walkdc,0,0,SRCCOPY);
      }
    DeleteObject(spaanrdc);
    DeleteObject(spaansteprdc);
    DeleteObject(standdc);
    DeleteObject(walkdc);
   ReleaseDC(hwnd,joe);
   DeleteObject(stand);
   DeleteObject(walk);
   SetTimer(hwnd,150,100,NULL);
   return false;
  }

  case WM_TIMER:
  {
   if(doneworking){doneworking=false;DeleteObject(spaanr);DeleteObject(spaanstepr);EndDialog(hwnd,0);}
   if(!stepping){stepping=1;}else{stepping=0;}
   char buf[35];
   itoa(waiting,buf,10);
   strcat(buf," bytes done");
   SetDlgItemText(hwnd,1,buf);

   int percdone=0;
   if(ipsmax!=0){percdone=(waiting*100/ipsmax);}else{percdone=0;}

   HDC joe=GetDC(hwnd);
    HDC thingy=CreateCompatibleDC(joe);;
    HBITMAP junk;
     if(!stepping)junk=SelectObject(thingy,spaanr);
     if(stepping)junk=SelectObject(thingy,spaanstepr);
      HBRUSH brushy=CreateSolidBrush(RGB(255,255,255));
      SelectObject(joe,brushy);
      Rectangle(joe,14,34,116,60);
      DeleteObject(brushy);
     BitBlt(joe,15,35,percdone,24,thingy,100-percdone,0,SRCCOPY);
     MoveToEx(joe,15+percdone,35,NULL);
     LineTo(joe,15+percdone,35+24);
      itoa(percdone,buf,10);
      strcat(buf,"%");
      SelectObject(joe,fonty);
      SetBkMode(joe,TRANSPARENT);
      TextOut(joe,17+percdone,36,buf,strlen(buf));
     SelectObject(thingy,junk);
    DeleteDC(thingy);
   ReleaseDC(hwnd,joe);
   return false;
  }

  case WM_CLOSE:
  {
   EndDialog(hwnd,0);
   return false;
  }

  default: return false;
 }
}

DWORD WINAPI IPSMakingThread(LPVOID)
{
 waiting=0;
 FILE *result=fopen(romname,"rb");
  if(result){
   if(IsCorrectRom(rom)==IsCorrectRom(result))
   {
    if(FileSize(rom)==FileSize(result)){
     ipsmax=FileSize(rom);
     CreateIPS(rom,result,ipsname,&waiting);
     PlaySound(MAKEINTRESOURCE(1003),hInst,SND_ASYNC|SND_RESOURCE);
    }else{MessageBox(hWnd,"The two ROM are of different size", "Warning",MB_OK|MB_ICONWARNING|MB_TASKMODAL);}
   }else{MessageBox(hWnd,"The two ROM are not compatible.", "Warning",MB_OK|MB_ICONWARNING|MB_TASKMODAL);}
  }else{MessageBox(hWnd,"Error Opening ROM for Reading", "Warning",MB_OK|MB_ICONWARNING|MB_TASKMODAL);}
 doneworking=true;
 ExitThread(0);
}

void DrawSprite(int x, int y, int select, HDC hdc)
{
   int width;
   int height;
   unsigned char spritez[80][64];
   unsigned char sprite[80*64];
   GetSprite(rom,select,spritez,&width,&height);
   for(int i=0;i<80;i++)
   {for(int j=0;j<64;j++){sprite[j+(i*64)]=spritez[79-i][j];}}

   width=width*8;
   height=height*8;
    BITMAPINFOHEADER infohead={sizeof(BITMAPINFOHEADER),64,80,1,8,BI_RGB,80*64,0,0,256,0};
    BITMAPINFO *info=(BITMAPINFO*) new unsigned char[sizeof(BITMAPINFOHEADER)+(256 * sizeof(RGBQUAD))];
    info->bmiHeader=infohead;
    for(int n=0;n<256;n++){info->bmiColors[n].rgbBlue=globalPal[n].b;info->bmiColors[n].rgbRed=globalPal[n].r;info->bmiColors[n].rgbGreen=globalPal[n].g;info->bmiColors[n].rgbReserved=0;}
   HBITMAP pic=CreateDIBitmap(hdc,&infohead,CBM_INIT,sprite,info,DIB_RGB_COLORS);

   HDC joe=CreateCompatibleDC(hdc);
    SelectObject(joe,pic);
    COLORREF color=RGB(0,0,0);
    HBRUSH brush=CreateSolidBrush(color);
    HBRUSH temp=SelectObject(hdc,brush);
    Rectangle(hdc,x,y,x+64,y+80);
    BitBlt(hdc,x+((64/2)-(width/2)),y+((80/2)-(height/2)),width,height,joe,0,0,SRCCOPY);
//    StretchBlt(hdc,x,y,64*2,80*2,joe,0,0,64,80,SRCCOPY);
   DeleteDC(joe);
  SelectObject(hdc,temp);
  DeleteObject(brush);
  DeleteObject(pic);
}
