
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void merge(void *array[],int left,int mid, int right, int (*comp)(void *,void *)){
    int l,r,n1,n2;
    l=left;
    r=mid+1;
    while (r<=right && l<r){
        if((*comp)(array[l],array[r])<=0)l++;
        else{
            void *temp=array[r];
            for(int i=r-1;i>=l;i--){
                array[i+1]=array[i];
            }
            array[l]=temp;
            l++;
            r++;
        }
    }
}

int numcmp(char * int1,char * int2){
    float conv1=atof(int1);
    float conv2=atof(int2);
    if(conv1<conv2) return -1;
    else return conv1>conv2;
}
void mergeSort(void *array[], int left, int right, int (*comp)(void *,void *)){
    if (left < right){
        int mid = left+(right-left)/2;
        mergeSort(array, left, mid,comp);
        mergeSort(array, mid+1, right,comp);
        merge(array, left, mid, right, comp);
    }
}

int main(int argc, char *argv[])
{
    int numeric=0;
    if(argc>1){
        if(strcmp(argv[1],"-n")==0){
            numeric=1;
        }
        int lenarr=argc-1-numeric;
        char *arr[lenarr];
        for(int i=1+numeric;i<argc;i++){
            arr[i-1-numeric]=argv[i];
        }
        mergeSort(arr,0,lenarr-1,numeric?numcmp:strcmp);
        for(int i=0;i<lenarr;i++){
            printf("%s ",arr[i]);
        }
	printf("\n");
    }
    return 0;
}
