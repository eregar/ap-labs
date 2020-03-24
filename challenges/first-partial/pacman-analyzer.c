#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define REPORT_FILE "packages_report.txt"
#define NAMESIZE 50
#define DATESIZE 17

void analizeLog(char *logFile, char *report);

struct pack{
    char *nombre;
    int namesize;
    char *installed;
    char *lastUpdate;
    int updateCount;
    char *removed;
};

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage:./pacman-analizer.o pacman.log\n");
        return 1;
    }
    analizeLog(argv[1], REPORT_FILE);
    return 0;
}

char* readLine(int file){
    char c;
    char line[200];
    int pos=0;
    int r;
    while((r=read(file,&c,1))!=0 && c!='\n' && pos<200){
        line[pos]=c;
        pos++;
    }
    if(pos==0){
      return "\0";
    }
    char *res=malloc(sizeof(char) * (pos+1));
    for(int i=0;i<pos+1;i++){
        res[i]=line[i];
    }
    res[pos]='\0';
    return res;
}

int intlen(int n){
    if(n>0){
        int c=5;
        for(int i=10000;i>1;i=i/10){
            if((n/i)>=1){
               return c;
              }
              c--;
        }
    }
    return 1;
}


void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    int file = open(logFile,O_RDONLY);
    int output= open(report,O_WRONLY | O_CREAT,0666);

    char *tokens[3]={"installed","upgraded","removed"};

    int f=0;
    printf("%d %d\n",file,output);
    if(file==0 || output==0){
	printf("no se pudieron acceder a los archivos");
	return;
    }
    int maxPackSize=1;
    int maxNameSize=NAMESIZE;

    struct pack *paquetes=malloc(sizeof(struct pack)*maxPackSize);//limpiar memoria
    if(paquetes==NULL){
        printf("problema de memoria");
        return;
    }
    paquetes[0].updateCount=0;
    paquetes[0].removed="-";
    paquetes[0].installed="-";
    paquetes[0].lastUpdate="-";
    int paquetesSize=0;

    int upgradedpkg=0;
    int installedpkg=0;
    int removedpkg=0;
    int currentInstalledpkg=0;

    char *string;
    while((string=readLine(file))[0]!='\0' && string!=NULL){
       // printf("%s\n",linea);//las lee todas, sin \n
        char *timeStamp=malloc(sizeof(char) * DATESIZE);

        int uselessLine=0;
        int i=1;
        //for timestamp
        for(i=1;i<DATESIZE;i++){
            if(string[i]=='\0'){
                uselessLine=1;
                break;
            }
            timeStamp[i-1]=string[i];
        }
        if(uselessLine || string[i++]!=']'){
            free(string);
            continue;
           }
        timeStamp[DATESIZE-1]='\0';
        i++;
        //for ALPM //salto//
        while(string[i]!=']'){i++;}
        i++;
        i++;
        int checkC=0;
        int word=-1;
        //printf("string[i]: %c\n",string[i]);
        if(string[i]=='i'){word=0;}
        else if(string[i]=='u'){word=1;}
        else if(string[i]=='r'){word=2;}
        else { uselessLine=1;  free(string); continue;}

        for(i;string[i]!=' ';i++){
            if(string[i]!=tokens[word][checkC]){
                uselessLine=1;
                break;
            }
            checkC++;
        }
        if(uselessLine){  free(string); continue;}
        i++;
        char *namae=malloc(sizeof(char)*NAMESIZE);
        if(namae==NULL){
            printf("problema de memoria");
            return;
        }

        int namaeSize=0;
        for (int j=i;string[j]!=' ';j++){
            namae[namaeSize]=string[j];

            namaeSize++;
            if(namaeSize==maxNameSize){
                maxNameSize++;
                namae=realloc(namae,sizeof(char)*(maxNameSize));
                if(namae==NULL){
                    printf("problema de memoria");
                    return;
                }
            }
        }
        namae[namaeSize]='\0';
        namaeSize++;
        int found=-1;
        for(int j=0;j<paquetesSize;j++){
            int k=0;
            for(k;k<namaeSize;k++){
                if(namae[k]!=paquetes[j].nombre[k]){
                    break;
                }
            }
            if(k==namaeSize){
                //este si es
                found=j;
                break;
            }
        }
        switch(word){
            case 0://install
            installedpkg++;
            currentInstalledpkg++;
            if(found!=-1){
                paquetes[found].installed = timeStamp;
            }
            else{
                paquetes[paquetesSize].nombre=namae;
                paquetes[paquetesSize].namesize=namaeSize;
                paquetes[paquetesSize].installed=timeStamp;
                paquetesSize++;

            }
            break;
            case 1://update
            
            if(found!=-1){
                if(paquetes[found].updateCount==0){
                  upgradedpkg++;
                }
                paquetes[found].lastUpdate=timeStamp;
                paquetes[found].updateCount++;
            }
            else{
                upgradedpkg++;
                printf("package upgraded w/ install log not found\n");
                paquetes[paquetesSize].nombre=namae;
                paquetes[paquetesSize].namesize=namaeSize;
                paquetes[found].lastUpdate=timeStamp;
                paquetes[found].updateCount++;
                paquetesSize++;
            }
            break;
            case 2://remove
            removedpkg++;
            if(found!=-1){
                currentInstalledpkg--;
                paquetes[found].removed=timeStamp;
            }
            else{
                printf("package removed w/ install log not found\n");
                paquetes[paquetesSize].nombre=namae;
                paquetes[paquetesSize].namesize=namaeSize;
                paquetes[found].removed=timeStamp;
                paquetesSize++;
            }
            break;
            default:
                uselessLine=1;
                printf("????");
            break;
        }
        if(paquetesSize==maxPackSize){
            maxPackSize++;
            paquetes=realloc(paquetes,sizeof(struct pack)* maxPackSize);
            if(paquetes==NULL){

                printf("problema de memoria");
                return;
            }
            paquetes[paquetesSize].updateCount=0;
            paquetes[paquetesSize].removed="-";
            paquetes[paquetesSize].installed="-";
            paquetes[paquetesSize].lastUpdate="-";
        }
        free(string);
    }
    char buffer[100];
    write(output, "Pacman PAckages report\n", 23);
    write(output, "----------------------\n", 23);
    write(output,"- Installed packages : ", 23);
    sprintf(buffer, "%d",installedpkg);
    write(output, buffer, intlen(installedpkg));
    write(output, "\n", 1);
    write(output,"- Removed packages   : ",23);
    sprintf(buffer, "%d",removedpkg);
    write(output, buffer, intlen(removedpkg));
    write(output, "\n", 1);
    write(output, "- Upgraded packages  : ", 23);
    sprintf(buffer, "%d",upgradedpkg);
    write(output, buffer, intlen(upgradedpkg));
    write(output, "\n", 1);
    write(output, "- Current installed  : ", 23);
    sprintf(buffer, "%d",currentInstalledpkg);
    write(output, buffer, intlen(currentInstalledpkg));
    write(output, "\n", 1);
    write(output, "\nList of packages:\n----------------\n", 36);

    for(int j=0;j<paquetesSize;j++){
        int datsize;
        write(output, "- Package Name        : ", 24);
        write(output, paquetes[j].nombre, paquetes[j].namesize-1);
        write(output, "\n", 1);
        write(output, "  - Install date      : ", 24);
        if(paquetes[j].installed[0]=='-') datsize=1;
        else datsize=DATESIZE-1;
        write(output, paquetes[j].installed, datsize);
        write(output, "\n", 1);
        write(output, "  - Last update date  : ", 24);
        if(paquetes[j].lastUpdate[0]=='-') datsize=1;
        else datsize=DATESIZE-1;
        write(output, paquetes[j].lastUpdate, datsize);
        write(output, "\n", 1);
        write(output, "  - How many updates  : ", 24);
        sprintf(buffer, "%d",paquetes[j].updateCount);
        write(output, buffer, intlen(paquetes[j].updateCount));
        write(output, "\n", 1);
        write(output, "  - Removal date      : ", 24);
        if(paquetes[j].removed[0]=='-') datsize=1;
        else datsize=DATESIZE-1;
        write(output, paquetes[j].removed, datsize);
        write(output, "\n", 1);
        free(paquetes[j].installed);
	if(paquetes[j].lastUpdate[0]!='-')
	    free(paquetes[j].lastUpdate);
	if(paquetes[j].removed[0]!='-')
	    free(paquetes[j].removed);
        free(paquetes[j].nombre);
    }
    free(paquetes);
    printf("Report is generated at: [%s]\n", report);
    close(file);
    close(output);
}



