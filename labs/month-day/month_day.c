#include <stdio.h>
#include <stdlib.h>
void month_day(int year, int yearday, int *pmonth, int *pday){
    int monthDay[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    int i;
    for(i=0;i<12;i++){
        if(yearday-monthDay[i]>0){
	    yearday-=monthDay[i];
        }else break;
    }
    *pmonth=i;
    *pday=yearday;
}

int main(int argc, char **argv) {
    if(argc==3){
        char *month[12]=
	{"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    	int year=atoi(argv[1]);
	int day=atoi(argv[2]);
	if(year <=0 ||(day<=0||day>365)){
            printf("arguments must be positive numbers\n");
	    printf("day must be between 1 and 365\n");
	    return 0;
    	}
    int *monthNumber;
    int *dayNumber;
    int monthnum=0;
    int daynum=0;
    monthNumber=&daynum;
    dayNumber=&monthnum;
    month_day(year,day,monthNumber,dayNumber);
    printf("%s %d, %d\n",month[*monthNumber],*dayNumber,year);
    }
    else{ printf("ther must be 2 arguments (year, day)\n");
    }
    return 0;
}
