/*** sample 1  内嵌汇编
#include <stdio.h>
void my_strcpy(const char *src, char *dest)
{
       char ch;
       __asm
       {
              loop:
              ldrb       ch, [src], #1
              strb       ch, [dest], #1
              cmp        ch, #0
              bne         loop
       }
}

int main()
{
       char *a = "forget it and move on!";
       char b[64];
       my_strcpy(a, b);
       printf("original: %s", a);
       printf("copyed:   %s", b);  
       return 0;
}
***/
 
 /*** sample 2 c调用汇编文件

 #include <stdio.h>
int gVar_1 = 12;
extern        asmDouble(void);
int main()
{
       printf("original value of gVar_1 is: %d", gVar_1);
       asmDouble();
       printf("       modified value of gVar_1 is: %d", gVar_1);
       return 0;
}


;called by main(in C),to double an integer, a global var defined in C is used.

       AREA asmfile, CODE, READONLY
       EXPORT       asmDouble
       IMPORT   gVar_1
asmDouble
       ldr r0, =gVar_1
       ldr          r1, [r0]
       mov        r2, #2      
       mul         r3, r1, r2
       str          r3, [r0]
       mov        pc, lr
       END
***/
 
 
 /*** sample 3 c 调用汇编函数

#include <stdio.h>
extern void asm_strcpy(const char *src, char *dest);
int main()
{
       const        char *s = "seasons in the sun";
       char        d[32];
       asm_strcpy(s, d);
       printf("source: %s", s);
       printf("       destination: %s",d);
       return 0;
}

;asm function implementation
       AREA asmfile, CODE, READONLY
       EXPORT asm_strcpy
asm_strcpy
loop
       ldrb          r4, [r0], #1       ;address increment after read
       cmp         r4, #0
       beq           over
       strb          r4, [r1], #1
       b               loop
over
       mov           pc, lr
       END
       
***/


/*** sample 4  汇编调用 c函数
;the details of parameters transfer comes from ATPCS
;if there are more than 4 args, stack will be used
       EXPORT asmfile
       AREA asmfile, CODE, READONLY
       IMPORT   cFun
       ENTRY
       mov        r0, #11
       mov        r1, #22
       mov        r2, #33
       BL       cFun
       END

int       cFun(int a, int b, int c)
{
       return a + b + c;
}

***/
