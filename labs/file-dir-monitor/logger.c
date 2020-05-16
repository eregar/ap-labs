#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>
#include "logger.h"

#define RED 1
#define GREEN 2
#define YELLOW 3
#define WHITE 7
#define STOUT 2

int toSys=0;
FILE *doc=NULL;

int initLogger(char *logType) {
    char log[6]="syslog";
    int equal=1;
    printf("Initializing Logger on: %s\n", logType);
    for(int i=0;i<5;i++){
	if(log[i]!=logType[i]){
	    equal=0;
	    break;
	}
    }
    if(equal){
	if(doc==NULL){
	    toSys=1;
	    doc=fopen("log.txt","a");
	    if(doc==NULL){
		(printf("algo salio mal al abrir el archivo"));
		toSys=0;
	    }
	}
    }
    return 0;
}

void textcolor(int fg){
    if(toSys==0){
        char command[6];
        sprintf(command, "\e[%dm",fg + 30);
        printf("%s",command);
    }
}

int infof(const char *format, ...) {
    va_list ap;
    va_start(ap,format);
    textcolor(WHITE);
    if(toSys){
	fprintf(doc,"[INFO] ");
	vfprintf(doc,format,ap);
    }
    else vprintf(format,ap);
    va_end(ap);
    return 0;
}

int warnf(const char *format, ...) {
    va_list ap;
    va_start(ap,format);
    textcolor(YELLOW);
    if(toSys){
	fprintf(doc,"[WARNING] ");
	vfprintf(doc,format,ap);
    }
    else vprintf(format,ap);
    va_end(ap);
    textcolor(WHITE);
    return 0;
}

int errorf(const char *format, ...) {
    va_list ap;
    va_start(ap,format);
    textcolor(RED);
    if(toSys){
	fprintf(doc,"[ERROR] ");
	vfprintf(doc,format,ap);
    }
    else vprintf(format,ap);
    va_end(ap);
    textcolor(WHITE);
    return 0;
}
int panicf(const char *format, ...) {
    va_list ap;
    va_start(ap,format);
    textcolor(GREEN);
    if(toSys){
	fprintf(doc,"[PANIC] ");
	vfprintf(doc,format,ap);
    }
    else vprintf(format,ap);
    va_end(ap);
    textcolor(WHITE);
    printf("\n");
    raise(SIGABRT);
    return 0;
}
