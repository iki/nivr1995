#include<stdio.h>
#include<dos.h>
#include<fcntl.h>
#include<io.h>
#include<stdlib.h>
#include<math.h>
#include<sys/stat.h>

/* #define costabfilename "F0000001.COS" */
#define sintabfilename "F0000000.SIN"

typedef unsigned char byte;

char sign[]="\n\sintab file maker\
\n - syntax : mksintab\n";

char ERR_FC[]="\nError : creating/overwriting file failed!";
char ERR_FW[]="\nError : writing file failed!";

byte ctrlbreaksignal;
static union REGS r;
static struct REGPACK reg;

/************************** KEYBOARD VARIABLES ****************************/
/**************************************************************************/

unsigned far *kb_stat =(unsigned far*)MK_FP(0,0x417);
byte     far *kb_alt  =(byte     far*)MK_FP(0,0x419);
unsigned far *kbb_head=(unsigned far*)MK_FP(0,0x41A);
unsigned far *kbb_tail=(unsigned far*)MK_FP(0,0x41C);
byte scancode=0,key=0;

/********************************* MACROS *********************************/
/**************************************************************************/

#define kbstr  (*kbb_tail!=*kbb_head)

/**************************** VIDEO ROUTINES ******************************/
/**************************************************************************/


unsigned getkey() {
 r.h.ah=0x10;
 int86(0x16,&r,&r);
 scancode=r.h.ah;
 key=r.h.al;
 return(r.x.ax);
}

/******************** kbb_nul - NULS KEYBOARD BUFFER *********************/
/*************************************************************************/

void kbb_nul() {
 while (bioskey(1)) bioskey(0);
}

/********************************* MAIN ***********************************/
/**************************************************************************/

byte recoghex(byte c) {
 if ((c>='0')&&(c<='9')) return(c-'0');
 if ((c>='A')&&(c<='F')) return(c-'A'+10);
 if ((c>='a')&&(c<='f')) return(c-'a'+10);
 return(16);
}

int c_break() {
 ctrlbreaksignal=1;
 return(1);
}

void quit (char *s,byte ec) {
 puts(s);
 exit(ec);
}

void main() {

byte c;
int handle_sf;        /*,handle_cf;*/
unsigned outval,i;
double alfa,sinval;

 puts(sign);

 ctrlbreaksignal=0;
 ctrlbrk(c_break);
 _fmode=O_BINARY;

 if (access(sintabfilename,0)==0) {
   printf("\nWarning : %s exists. Overwrite (Y,y/any key)?",sintabfilename);
   for(c=getch();!c;c=getch());
   if (toupper(c)!='Y') quit("\nAbort on user request!",1);
 }
 if ((handle_sf=creat(sintabfilename,S_IWRITE))==-1) quit(ERR_FC,1);
 lseek(handle_sf,0,SEEK_SET);

/* if (access(costabfilename,0)==0) {
   printf("\nWarning : %s exists. Overwrite (Y,y/any key)?",costabfilename);
   for(c=getch();!c;c=getch());
   if (toupper(c)!='Y') quit("\nAbort on user request!",1);
 }
 if ((handle_cf=creat(costabfilename,S_IWRITE))==-1) quit(ERR_FC,1);
 lseek(handle_cf,0,SEEK_SET);
 }
*/

 for(alfa=0,i=0;i<16384;alfa+=(2*M_PI)/(double)65536,i++) {
   sinval=sin(alfa)*32767+0.5;
   outval=sinval;
   if (write(handle_sf,&outval,2)!=2) quit(ERR_FW,1);
 }
 close(handle_sf);
/* close(handle_cf);*/
}