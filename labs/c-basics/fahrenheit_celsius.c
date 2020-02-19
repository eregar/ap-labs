#include <stdio.h>
#include <stdlib.h>

#define   LOWER  0       /* lower limit of table */
#define   UPPER  300     /* upper limit */
#define   STEP   20      /* step size */

/* print Fahrenheit-Celsius table */

int main(int argc, char **argv)
{
    if(argc==2){
        int convert=strtol(argv[1],NULL,10);
        printf("Fahrenheit: %3s, Celcius: %6.1f\n",argv[1],(5.0/9.0)*(atoi(argv[1])-32));
    }
    else if(argc==4){
        int fahr;
        for (fahr = atoi(argv[1]); fahr <=atoi(argv[2]); fahr = fahr + atoi(argv[3])){
	    printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
	}
    }
    printf("numero de args: %d",argc);
    //for (fahr = LOWER; fahr <= UPPER; fahr = fahr + STEP)
//	printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));

    return 0;
}
