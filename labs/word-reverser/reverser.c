#include <stdio.h>
#include <stdlib.h>

int main()
{
    int length=20;
    int *buff=malloc(sizeof(int)*length);
    while(1){
        int uwu;
        int i=0;
        uwu=getchar();
        while(uwu!='\n') {
            if(uwu==-1) return 0;
            buff[i]=uwu;
            uwu=getchar();
            i++;
            if(length==i){
                int *temp=realloc(buff,sizeof(int)*length*2);
                if(temp){
                    buff=temp;
                    length*=2;
                }
                else{
                    printf("memory issues, plz retry");
                    return 0;
                } 
            }
        }
        i--;
        while(i>=0){
            putchar(buff[i]);
            i--;
        }
        printf("\n");
    }
    free(buff);
}
