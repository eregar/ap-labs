#define _XOPEN_SOURCE 500
#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

#include <ftw.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include "logger.h"
int fd=-1;

static int display_info(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {
  if(tflag == FTW_D){
    int wd = inotify_add_watch(fd, fpath, IN_CREATE | IN_DELETE | IN_MOVE);
    if (wd == -1) {
      errorf("Error while adding watch to inotify");
    }
  }
  return 0; 
}

int main(int argc, char **argv) {
  if(argc!=2){
    errorf("incorrect number of arguments");
    return 0;
  }
  int flag=0;
  fd=inotify_init();
  if(fd<0){
    errorf("inotify cannot initiate");
    return 0;
  }
  if (nftw(argv[1], display_info, 20,flag)== -1) {
    errorf("algo salio mal");
    exit(EXIT_FAILURE);
  }


  while(1){
    char buffer[EVENT_BUF_LEN];
    int length = read(fd, buffer, EVENT_BUF_LEN); 
    if (length<0) {
      errorf( "read cannot be anymore xd" );
      break;
    }  

    int i=0;
    while (i < length ) {
      struct inotify_event *event = ( struct inotify_event * ) &buffer[i];  
      if ( event->len ) {
        if ( event->mask & IN_CREATE ) {
          infof("%s was created\n", event->name);
        }
        else if ( event->mask & IN_DELETE ) {
          infof("%s was deleted\n", event->name);
        }
        else if (event->cookie!=0){
	  if(event->mask & IN_MOVED_FROM)
          infof("%s was renamed to ",event->name);
	  else infof("%s\n",event->name);
        }
        else{
          infof("unknown action");
        }
      }
      i += EVENT_SIZE + event->len;
    }
  }
  //ooif
  exit(EXIT_SUCCESS);
}
