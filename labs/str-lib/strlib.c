

int mystrlen(char *str){
    int i=0;
    while(str[i]!='\0'){
        i++;
    }
    return i;
}

char *mystradd(char *origin, char *addition){
    int lenOrigin=mystrlen(origin);
    int lenAddition=mystrlen(addition);
    //char temp[lenOrigin+lenAddition];
    int i=lenOrigin;
    while(i<lenOrigin+lenAddition){
        origin[i]=addition[i-lenOrigin];
        i++;
    }
    return origin;
}

int mystrfind(char *origin, char *substr){
    int lenOrigin=mystrlen(origin);
    int lenSubstr=mystrlen(substr);
    if(lenOrigin<lenSubstr || lenSubstr==0 || lenOrigin==0) return-1;
    for(int i=0;i<lenOrigin-lenSubstr+1;i++){
        int j=i+lenSubstr;
        int found=1;
        int f=0;
        for(int k=i;k<j;k++){
            if(origin[k]!=substr[f]){
                found=0;
                break;
            }
            f++;
        }
        if(found){
            return i;
        }
    }
    return -1;
}
