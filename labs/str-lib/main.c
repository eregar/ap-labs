#include <stdio.h>

int mystrlen(char*);
char *mystradd(char*, char*);
int mystrfind(char*, char*);

int main(int argc, char **argv) {
    if(argc<3){
	printf("se necesitan 3 argumentos");
    }
    else{
        printf("Initial Length	    : %d\n",mystrlen(argv[1]));
        char * temp=mystradd(argv[1],argv[2]);
	printf("New String	    : %s\n",temp);
	printf("Substring was found : %s\n",(mystrfind(temp,argv[3]))>=0? "yes":"no");
    }
    return 0;
}
