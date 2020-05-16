#include <stdio.h>
#include "logger.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

//necessary for decoding proc
#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66
#define ENCODE 1
#define DECODE 2

//necesary for decoding proc
static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int progress = 0;
int total =0;

//64 encode algorithm from wikibooks.org
int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3) 
   {
	progress=x;
      /* these three 8-bit (ASCII) characters become one 24-bit number */
	/*if(x>=20){
	   int cont=0;
	    while(cont<999)cont++;
	}*/
      n = ((uint32_t)data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+2) < dataLength)
         n += data[x+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
            
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }

   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      } 
      progress=dataLength;
   }
   if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}


//decode algorithm from wikibooks.org
int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;

    total=inLen;
    while (in < end) {
	progress++;
	/*if(progress>=20){
           int cont=0;
            while(cont<99999)cont++;
        }*/
        unsigned char c = d[*in++];
        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            /* If the buffer is full, split it into bytes */
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}

void handler(int signal){
    if(signal==SIGINT || signal==SIGUSR1){
	infof("%d%%\n",(progress*100)/total);
    }
    else{
	infof("unexpected signal!\n");
    }
}


int main(int argc, char **argv){
    initLogger("");
    signal(SIGUSR1,handler);
    signal(SIGINT,handler);
    int action;
    if(argc==3){
	if(strcmp(argv[1],"--encode")==0){
	    //infof("quiere hacer encode\n");
	    action=ENCODE;
	}
	else if(strcmp(argv[1],"--decode")==0){
	    //infof("quiere hacer decode\n");
	    action=DECODE;
	}
	else {
	    errorf("opcion no valida\n");
	    return 0;
	}
	FILE *arch = fopen(argv[2],"r");
	if(arch==NULL){
	    errorf("no se pudo abrir %s\n",argv[2]);
	    return 0;
	}
	fseek(arch, 0L, SEEK_END);
	int max = ftell(arch);
	rewind(arch);
	FILE *writer;
	if(action==ENCODE){
	    writer=fopen("encoded.txt","w");
	}else{
	    writer=fopen("decoded.txt","w");
	}
	if(writer==NULL){
	    errorf("no se pudo abrir el archivo de escritura\n");
	    fclose(arch);
	    return 0;
	}
	char *contents=malloc(max);
	fread(contents, 1, max, arch);
	char *output=malloc(max*4);
	total=max;
	if(action==ENCODE){
            int err=base64encode(contents,max,output,max*4);
	    if(err!=0){
		errorf("algo salio mal\n");
		return 1;
	    }
	    fprintf(writer,"%s",output);
	}else{
	    size_t buf = max * 4;
	    int err=base64decode(contents,max,output,&buf);
	    if(err!=0){
		errorf("algo salio mal\n");
		return 1;
	    }
	    fprintf(writer,"%s",output);
	}
	fclose(arch);
	fclose(writer);
	free(contents);
	free(output);
	infof("completed succesfully\n");
    }
    return 0;
}

