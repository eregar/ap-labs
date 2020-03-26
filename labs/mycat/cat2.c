#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define tocons 1
#define toerror 2
#define readcons 0

/* filecopy:  copy file ifp to file ofp */
void filecopy(int ifp, int ofp)
{
    char c[1];
    int r;
    while ((r = read(ifp,c,1)) != 0){
        int err=write(ofp,c,1);
        if(err==0) write(toerror,"error al escribir\n",18);
    }
}

int mystrlen(char *str){
    int i=0;
    while(str[i]!='\0'){
        i++;
    }
    return i;
}

/* cat:  concatenate files, version 2 */
int main(int argc, char *argv[])
{
    int fp;
    void filecopy(int, int);
    char *prog = argv[0];   /* program name for errors */

    if (argc == 1)  /* no args; copy standard input */
        filecopy(readcons, tocons);
    else
        while (--argc > 0)
            if ((fp = open(*++argv,O_RDONLY)) < 0) {
                write(toerror,prog,mystrlen(prog));
                write(toerror,": can′t open ",13);
                write(toerror,*argv,mystrlen(*argv));
                write(toerror,"\n",1);
                return 1;
            } else {
                filecopy(fp, tocons);
                close(fp);
            }

    if (ferror(stdout)) {
        write(toerror,prog,mystrlen(prog));
        write(toerror,": error writing stdout\n",23);
        return 2;
    }

    return 0;
}
