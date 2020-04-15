
#include <stdio.h>
#include <stdarg.h>

#define RED 1
#define GREEN 2
#define YELLOW 3
#define	WHITE 7
#define STOUT 2

void textcolor(int fg){	
  char command[6];
	sprintf(command, "\e[%dm",fg + 30);
	printf("%s",command);
}

int infof(const char *format, ...){
    va_list ap;
    va_start(ap,format);
    textcolor(WHITE);
    vprintf(format,ap);
    va_end(ap);
    return 0;
}

int warnf(const char *format, ...){
    va_list ap;
    va_start(ap,format);
    textcolor(YELLOW);
    vprintf(format,ap);
    va_end(ap);
    textcolor(WHITE);
    return 0;
}
int errorf(const char *format, ...){
    va_list ap;
    va_start(ap,format);
    textcolor(RED);
    vprintf(format,ap);
    va_end(ap);
    textcolor(WHITE);
    return 0;
}
int panicf(const char *format, ...){
    va_list ap;
    va_start(ap,format);
    textcolor(GREEN);
    vprintf(format,ap);
    va_end(ap);
    textcolor(WHITE);
    return 0;
}


