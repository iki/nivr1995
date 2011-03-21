#include<stdio.h>
#include<dos.h>
#include<fcntl.h>
#include<io.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<dir.h>

#define fatfilename "fatfile"
#define megafilename "megafile"

#define DF_MAXFATSIZE    65536
#define DF_MAXFATENTRIES  4096
#define DF_BLOCKSIZE      4096
#define DF_BLOCKENTRIES    256
#define DF_VERSION        0x10

#define STPOSADDBUF   17


typedef unsigned char byte;
typedef unsigned long fatentry[4];

char sign[]="\n\datafile maker\
\n - syntax : mkdf <filename0> <filename1> [..filenameN]\
\n - version: %X, maxfatsize: %lu, blocksize: %u\n";

char ERR_FO[]="\nError : can't open fatfile!";
char ERR_FC[]="\nError : can't create fatfile!";
char ERR_MO[]="\nError : can't open megafile!";
char ERR_MC[]="\nError : can't create megafile!";

char ERR_FR[]="\nError : reading fatfile failed!";
char ERR_FW[]="\nError : writing fatfile failed!";
char ERR_MW[]="\nError : writing megafile failed!";
char ERR_R[]="\nError : reading file failed!";
char ERR_W[]="\nError : writing file failed!";

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

fatentry *dffat,dffatx;
unsigned long *pdff,*pbuf;
int handle,handle_mf,handle_ff;
struct ffblk ffblk;
byte c,c0,c1,buf[DF_BLOCKSIZE];
unsigned b0,b1,b2,b3;
unsigned u0,u1,u2,u3;
unsigned nfiles0,nfiles,fatentries,fatsize;
unsigned long l0,l1,l2,l3,foffset;
struct date d;
struct time t;


 printf(sign,DF_VERSION,DF_MAXFATSIZE,DF_BLOCKSIZE);

 if ((dffat=malloc(4095*16+4))==NULL) quit("\nError : not enough memory to allocate buffer..",1);

 ctrlbreaksignal=0;
 ctrlbrk(c_break);
 _fmode=O_BINARY;

 if (_argc<2) quit("\nAt least one parameter needed!",1);

 if (access(fatfilename,0)==0) {
   printf("\nWarning : %s exists. Append entered files (Y,y/any key)?",fatfilename);
   for(c=getch();!c;c=getch());
   if (toupper(c)!='Y') quit("\nAbort on user request!",1);
   if ((findfirst(fatfilename,&ffblk,FA_ARCH))==-1) quit(ERR_FO,1);
   if ((handle_ff=open(fatfilename,O_RDWR))==-1) quit(ERR_FO,1);
   if (read(handle_ff,dffat,ffblk.ff_fsize-12)!=ffblk.ff_fsize-12) quit(ERR_FR,1);
   l0=dffat[0][3];
   l0>>=16;
   nfiles0=l0;
   fatsize=dffat[1][0]&0x0FFFFFF00;
   fatentries=fatsize>>4;
   foffset=dffat[fatentries-1][0]-fatsize;
   printf("\n[OLD] nfiles=%u ; fatentries=%u ; fatsize=%u ; foffset=%08lX",nfiles0,fatentries,fatsize,foffset);
   for(b0=1;b0<=nfiles0;b0++) dffat[b0][0]-=fatsize;
   if (access(megafilename,0)==0) {
     if ((handle_mf=open(megafilename,O_RDWR))==-1) quit(ERR_MO,1);
     lseek(handle_mf,0,SEEK_END);
   } else {
     printf("\nWarning : %s doesn't exist. Continue anyway (Y,y/any key)?",megafilename);
     for(c=getch();!c;c=getch());
     if (toupper(c)!='Y') quit("\nAbort on user request!",1);
     if ((handle_mf=creat(megafilename,S_IWRITE))==-1) quit(ERR_MC,1);
     lseek(handle_mf,0,SEEK_SET);
   }
 } else {
  if (_argc<3) quit("\nAt least two parameters needed when fatfile doesn't alredy exist!",1);
  if ((handle_ff=creat(fatfilename,S_IWRITE))==-1) quit(ERR_FC,1);
  if (access(megafilename,0)==0) {
   printf("\nWarning : %s exists. Overwrite (Y,y/any key)?",megafilename);
   for(c=getch();!c;c=getch());
   if (toupper(c)!='Y') quit("\nAbort on user request!",1);
  }
  if ((handle_mf=creat(megafilename,S_IWRITE))==-1) quit(ERR_MC,1);
  lseek(handle_mf,0,SEEK_SET);
  nfiles0=0;
  foffset=0;
 }

 randomize();
 for (u0=0;u0<DF_BLOCKSIZE;u0++) buf[u0]=random(256);

 nfiles=_argc-1+nfiles0;
 fatsize=(nfiles+2)<<4;
 u1=fatsize % DF_BLOCKSIZE;
 if (u1) fatsize=fatsize+(DF_BLOCKSIZE-u1);
 fatentries=fatsize>>4;
 printf("\n[EST] nfiles=%u ; fatentries=%u ; fatsize=%u",nfiles,fatentries,fatsize);
 dffat[0][0]=0;

 getdate(&d);
 u0=d.da_year-1980;
 u0&=127;
 u0<<=9;
 u1=d.da_mon&15;
 u1<<=5;
 u2=d.da_day&31;
 u3=u0+u1+u2;

 gettime(&t);
 u0=t.ti_hour&31;
 u0<<=11;
 u1=t.ti_min&63;
 u1<<=5;

 u2=t.ti_sec&63;
 u2>>=1;
 l0=u0+u1+u2;
 l0<<=16;
 l0+=u3;
 dffat[0][1]=dffat[0][2]=l0;
 printf("\n # date=%u.%u.%u ; time=%u:%u:%u ; %08lX",d.da_day,d.da_mon,d.da_year,t.ti_hour,t.ti_min,t.ti_sec,l0);

 for(b0=1,b1=nfiles0+1;b1<=nfiles;b0++,b1++){

   if ((findfirst(_argv[b0],&ffblk,FA_ARCH))==-1) {
    printf("\n # warning : couldn't find %s",_argv[b0]);
    nfiles--;b1--;
    continue;
   }

   for(;;) {
     fatentries=nfiles+2;
     u1=fatentries % DF_BLOCKENTRIES;
     if (u1) fatentries=fatentries+(DF_BLOCKENTRIES-u1);

     printf("\n # adding %s as file %u/%u/%u",ffblk.ff_name,b1,nfiles,fatentries-2);
     for(l3=0,b2=0,b3=28;b2<8;b2++,b3-=4) {
       l2=recoghex(ffblk.ff_name[b2]);
       if (l2>15) {
	 printf(" * enter id (lX) : ");
	 scanf("%lX",&l3);
	 break;
       }
       l2<<=b3;
       l3+=l2;
     }
     dffat[b1][3]=l3;
     if (b2==8) printf("\n");
     printf("   id=%08lX ; ",l3);

     dffat[b1][0]=foffset;
     l0=ffblk.ff_fsize;
     l1=l0|(DF_BLOCKSIZE-1);
     l1-=(DF_BLOCKSIZE-1);
     if (l1<l0) l1+=DF_BLOCKSIZE;
     l2=l1-l0;
     l3=(l0|3)-3;
     printf("%lu+%lu=%lu ; ",l0,l2,l1,l3);
     dffat[b1][1]=l0;
     dffat[b1][2]=l0;
     foffset+=l1;

     if ((handle=open(ffblk.ff_name,O_RDWR))==-1) quit("\nError : can't open file!",1);
     /* if ((handle=open(_argv[b0],O_RDWR))==-1) quit("\nError : can't open file!",1);*/
     /* lseek(handle,0,SEEK_SET);*/

     c0=c1=0;u0=STPOSADDBUF;
     for(l1=0;l1<l0;l1++) {
       if (read(handle,&c,1)!=1) quit(ERR_R,1);
       if (write(handle_mf,&c,1)!=1) quit(ERR_MW,1);
       buf[u0]+=c;
       c0+=c;
       c1^=c;
       u0++;
       if (u0>=DF_BLOCKSIZE) u0=0;
     }
     c=c0^c1;dffat[b1][0]+=c;
     printf("ac=%02X ; xc=%02X ; foffset=%08lX ",c0,c1,dffat[b1][0]);

     lseek(handle,0,SEEK_END);
     if (l2) if (write(handle,buf,l2)!=l2) quit(ERR_W,1);
     close(handle);
     if (l2) if (write(handle_mf,buf,l2)!=l2) quit(ERR_MW,1);

     if (findnext(&ffblk)) break;
     if (ctrlbreaksignal) {
      nfiles=b1;
      break;
     }
     nfiles++;b1++;
   }
   if (ctrlbreaksignal) {
      nfiles=b1;
      break;
   }
 }

 l0=nfiles;
 l0<<=16;
 l0+=DF_VERSION+DF_BLOCKSIZE;
 dffat[0][3]=l0;
 fatentries=nfiles+2;
 u1=fatentries % DF_BLOCKENTRIES;
 if (u1) fatentries=fatentries+(DF_BLOCKENTRIES-u1);
 fatsize=fatentries<<4;
 foffset+=fatsize;
 dffatx[0]=foffset;
 printf("\n[NEW] nfiles=%u ; fatentries=%u ; fatsize=%u ; foffset+(fs)=%08lX",nfiles,fatentries,fatsize,foffset);
 printf("\n      infodd=%08lX ; ",l0);

 /* if (nfiles>fatentries-2) printf("\n\b # WARNING: too much files due to wildcards (%u allowed)",fatentries-2);*/

 for(b0=1;b0<=nfiles;b0++) dffat[b0][0]+=fatsize;

 for(b0=0,b1=1,b2=nfiles+1;b2<fatentries-1;b0++,b1++,b2++) {
  dffat[b2][0]=dffat[b0][0]^dffat[b1][0];
  dffat[b2][1]=dffat[b0][1]^dffat[b1][1];
  dffat[b2][2]=dffat[b0][2]^dffat[b1][2];
  dffat[b2][3]=dffat[b0][3]^dffat[b1][3];
 }
/* l0=dffat[0][3];l1=dffat[fatentries-2][3];l2=l0^l1;
 printf("\n # updating info: %08lX xor %08lX = %08lX ; ",l0,l1,l2);
 dffat[0][3]=l2;*/

 l0=l1=0;
 for(b0=0;b0<fatentries-1;b0++) for(b1=0;b1<4;b1++) {
  l0^=dffat[b0][b1];
  l1+=dffat[b0][b1];
 }
 printf("fataddcrc0 : %08lX ; fatxorcrc0 : %08lX",l1,l0);
 dffatx[1]=dffatx[2]=l1;
 dffatx[3]=l0;

 lseek(handle_ff,0,SEEK_SET);
 if (write(handle_ff,dffat,fatsize-16)!=(fatsize-16)) quit("\nError : writing fatfile failed!",1);
 if (write(handle_ff,dffatx,16)!=16) quit("\nError : writing datafile failed!",1);
 close(handle_ff);
 close(handle_mf);
}