#include<stdio.h>
#include<dos.h>
#include<fcntl.h>
#include<io.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<dir.h>

#define MAX_ROW_LENGHT 255

#define name256 "RGB8BIT.INC"
#define name64  "RGB6BIT.INC"
#define name16  "RGB4BIT.INC"
#define namefkm "RGB4FKM.INC"
#define enterstr "\n"

#define equstr  "EQU"

typedef unsigned char byte;

char sign[]="\nrgb_text_list -> assembler_equ  formatter\
\nsyntax : rgblist <rgblist file>\n";

char ERR_O[]="\nError : can't open file!";
char ERR_C[]="\nError : can't create file!";
char ERR_R[]="\nError : reading file failed!";
char ERR_W[]="\nError : writing file failed!";

byte ctrlbreaksignal,fileselect=0;
static union REGS r;
static struct REGPACK reg;
int handle,handle256,handle64,handle16,handlefkm;

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

void askfile (char *filename,byte fileflag,byte allflag) {
byte c;
 printf("\nFile %s ",filename);
 if (access(filename,0)==0) printf("exists.");
 else printf("doesn't exist.");
 if (fileselect&allflag==allflag) return;
 printf(" Add new colors (Yes/All/no)?");
 for(c=getch();!c;c=getch());
 c=toupper(c);
 if (c=='Y') fileselect|=fileflag;
 if (c=='A') fileselect=allflag;
}

void addtofile (char *filename,int *handle) {
 if (access(filename,0)==0) {
   if ((*handle=open(filename,O_RDWR))==-1) quit(ERR_O,1);
   lseek(*handle,0,SEEK_END);
 } else {
   if ((*handle=creat(filename,S_IWRITE))==-1) quit(ERR_C,1);
   lseek(*handle,0,SEEK_SET);
 }
}

char *strupper (char *str) {
byte i,j;
 j=strlen(str);
 for (i=0;i<j;i++) str[i]=toupper(str[i]);
 return(str);
}

char *strlower (char *str) {
byte i,j;
 j=strlen(str);
 for (i=0;i<j;i++) str[i]=tolower(str[i]);
 return(str);
}

void entertofiles() {
 if (fileselect&1) if (write(handle256,enterstr,strlen(enterstr))!=strlen(enterstr)) quit(ERR_W,1);
 if (fileselect&2) if (write(handle64,enterstr,strlen(enterstr))!=strlen(enterstr)) quit(ERR_W,1);
 if (fileselect&4) if (write(handle16,enterstr,strlen(enterstr))!=strlen(enterstr)) quit(ERR_W,1);
 if (fileselect&8) if (write(handlefkm,enterstr,strlen(enterstr))!=strlen(enterstr)) quit(ERR_W,1);
}



void main() {

byte prebuf[MAX_ROW_LENGHT+3],outbuf[MAX_ROW_LENGHT+2],
     fn1[MAX_ROW_LENGHT],fn2[MAX_ROW_LENGHT],fn3[MAX_ROW_LENGHT],fn4[MAX_ROW_LENGHT],
     colorname[MAX_ROW_LENGHT],*pbuf,*sbuf,*buf,*s,lastline=0,linedone,lastmark=0;
unsigned ln=0,ln_ne=0,ln_marked=0,ln_colors=0,ibuf,i,j,n1,n2,n3;

 printf(sign);

 /* ctrlbreaksignal=0;
 ctrlbrk(c_break);*/
 _fmode=O_TEXT;

 if (_argc<2) quit("\nrgblist filename parameter needed!",1);

 if ((handle=open(_argv[1],O_RDONLY))==-1) quit(ERR_O,1);
 lseek(handle,0,SEEK_SET);

 askfile(name256,1,15);
 askfile(name64,2,14);
 askfile(name16,4,12);
 askfile(namefkm,8,8);
 if (fileselect==0) quit("\No update requested!",1);

 if (fileselect&1) addtofile(name256,&handle256);
 if (fileselect&2) addtofile(name64,&handle64);
 if (fileselect&4) addtofile(name16,&handle16);
 if (fileselect&8) addtofile(namefkm,&handlefkm);

 entertofiles();

 buf=prebuf+2;
 *prebuf=';';
 *(prebuf+1)=' ';

 printf("\n");
 do {
   linedone=0;
   ln_ne++;
   do {
     for(pbuf=buf,ibuf=0;ibuf<MAX_ROW_LENGHT;*pbuf=0,pbuf++,ibuf++);
     *(buf+MAX_ROW_LENGHT)='A';
     ln++;
     printf("%cprocessing line %4u",13,ln);
     pbuf=buf-1;
     ibuf=0;
     do {
       pbuf++;
       ibuf++;
       if (read(handle,pbuf,1)==0) {
	 printf("; lastline");
	 lastline=1;
       }
     } while ((*pbuf!='\n')&&(*pbuf!=0)&&(ibuf<MAX_ROW_LENGHT-1)&&(lastline==0));
     for (sbuf=buf;isspace(*sbuf);sbuf++);
     printf ("; ibuf=%3u; sbuf=%3u   ",ibuf,sbuf-buf);
   } while (sbuf>=buf+MAX_ROW_LENGHT);

   *fn1=0;*fn2=0;*fn3=0;*fn4=0;
   if ((sscanf(sbuf,"%s %hu %hu %hu",fn1,&n1,&n2,&n3)==4)) {
     sprintf(colorname,"%s",fn1);
   } else if (sscanf(sbuf,"%s %s %hu %hu %hu",fn1,fn2,&n1,&n2,&n3)==5) {
     sprintf(colorname,"%s_%s",fn1,fn2);
   } else if (sscanf(sbuf,"%s %s %s %hu %hu %hu",fn1,fn2,fn3,&n1,&n2,&n3)==6) {
     sprintf(colorname,"%s_%s_%s",fn1,fn2,fn3);
   } else if (sscanf(sbuf,"%s %s %s %s %hu %hu %hu",fn1,fn2,fn3,fn4,&n1,&n2,&n3)==7) {
     sprintf(colorname,"%s_%s_%s_%s",fn1,fn2,fn3,fn4);
   } else {
if (lastmark==0) entertofiles();
ln_marked++;
linedone=1;
if (fileselect&1) if (write(handle256,prebuf,strlen(prebuf))!=strlen(prebuf)) quit(ERR_W,1);
if (fileselect&2) if (write(handle64,prebuf,strlen(prebuf))!=strlen(prebuf)) quit(ERR_W,1);
if (fileselect&4) if (write(handle16,prebuf,strlen(prebuf))!=strlen(prebuf)) quit(ERR_W,1);
if (fileselect&8) if (write(handlefkm,prebuf,strlen(prebuf))!=strlen(prebuf)) quit(ERR_W,1);
lastmark=1;
		 }

   if (!(linedone)) {
if (lastmark==1) entertofiles();
ln_colors++;
strlower(colorname);
for(s=colorname;*s!=0;s++);
i=s-colorname;
for(;i<30;*s=' ',i++,s++);
*s=0;
if (fileselect&1) {
  sprintf(outbuf,"_R8_%s EQU %3u ; = %03Xh\n",colorname,n1,n1);
  if (write(handle256,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
  sprintf(outbuf,"_G8_%s EQU %3u ; = %03Xh\n",colorname,n2,n2);
  if (write(handle256,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
  sprintf(outbuf,"_B8_%s EQU %3u ; = %03Xh\n",colorname,n3,n3);
  if (write(handle256,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
}
if (fileselect&2) {
  i=n1/4; /*+(n1%4)/2;*/
  sprintf(outbuf,"_R6_%s EQU %2u ; = %02Xh , %3u = %03Xh\n",colorname,i,i,n1,n1);
  if (write(handle64,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
  i=n2/4;
  sprintf(outbuf,"_G6_%s EQU %2u ; = %02Xh , %3u = %03Xh\n",colorname,i,i,n2,n2);
  if (write(handle64,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
  i=n3/4;
  sprintf(outbuf,"_B6_%s EQU %2u ; = %02Xh , %3u = %03Xh\n",colorname,i,i,n3,n3);
  if (write(handle64,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
}
if (fileselect&4) {
  i=n1/16; /*+(n1%16)/8;*/
  sprintf(outbuf,"_R4_%s EQU %2u ; = %02Xh , %3u = %03Xh\n",colorname,i,i,n1,n1);
  if (write(handle16,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
  i=n2/16;
  sprintf(outbuf,"_G4_%s EQU %2u ; = %02Xh , %3u = %03Xh\n",colorname,i,i,n2,n2);
  if (write(handle16,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
  i=n3/16;
  sprintf(outbuf,"_B4_%s EQU %2u ; = %02Xh , %3u = %03Xh\n",colorname,i,i,n3,n3);
  if (write(handle16,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
}
if (fileselect&4) {
  i=(n1/16)+(n3&240);
  if (i<16) i+=16;
  sprintf(outbuf,"_BR4_%s EQU %3u ; = %03Xh , %u = %Xh , %u = %Xh\n",colorname,i,i,n3,n3,n1,n1);
  if (write(handlefkm,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);

  i=n2/16;
  sprintf(outbuf,"_0G4_%s EQU %3u ; = %03Xh , %3u = %03Xh\n",colorname,i,i,n2,n2);
  if (write(handlefkm,outbuf,strlen(outbuf))!=strlen(outbuf)) quit(ERR_W,1);
}
lastmark=0;
}

 } while (lastline==0);
 printf("\nlines: %u; ln_marked: %u; ln_colors: %u; ln_e: %u",ln,ln_marked,ln_colors,ln-ln_ne);
 quit("\nEverything went ok..",0);


/*	    for (i=0;!(isspace(fn[i]));i++);fn[i]='_';
	    for (;!(isspace(fn[i]));i++);fn[i]='_';
*/

}