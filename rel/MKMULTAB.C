#include<stdio.h>
#include<dos.h>

typedef unsigned char byte;

int c_break() {
 return(1);
}

void main() {
unsigned m0,m,n,p,limlin;
long i,j;
char *bs,*ms;
 ctrlbrk(c_break);
 printf("\nmultab 0.00á by me (but i'm definitely not proud of it)\
\n - creates the table of first m muls of n\n");
 fflush(stdin);
 printf("\nenter n : ");
 scanf("%u",&n);
 fflush(stdin);
 printf("enter m : ");
 scanf("%u",&m);
 i=n*m;
 if (i<=0x0ffff) {
  if (i<=0xff){
   bs="\n db %03Xh";
   ms=",%03Xh";
   p=limlin=14;
  } else {
   bs="\ndw %05Xh";
   ms=",%05Xh";
   p=limlin=10;
  }
  m++;
  for (j=0,m0=0;m0<m;m0++,j+=n) {
   if (p==limlin) {
    printf(bs,j);
    p=0;
   }
   else {
    printf(ms,j);
    p++;
   }
  }
 }
}

