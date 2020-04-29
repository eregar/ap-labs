package main

import (
	"log"
	"net"
	"os"
	"io"
	"fmt"
	"strings"
)
func mustCopy(zone string,src net.Conn,closer chan int) {

   for infinite:=true;infinite;{
	buf := make([]byte, 256)
	n , err := src.Read(buf)
        //_ , err := io.Copy(dst, src)
        if err != nil {
	    if err != io.EOF{
                log.Fatal(err)
		infinite=false
	    } else {
	        infinite=false
	    }
        } else {
	    fmt.Print("["+zone+"] "+string(buf[:n]))
	}
    }
    closer<-1
}

func main(){
	chans:=make([]chan int,len(os.Args)-1)
	if len(os.Args)>1 {
		for i:=1;i<len(os.Args);i++ {
			l:=strings.Split(os.Args[i],":")[1]
			zone:=strings.Split(os.Args[i],"=")[0]
			conn,err:=net.Dial("tcp","localhost:"+l)
			if err != nil {
     				log.Fatal(err)
			}
			chans[i-1]=make(chan int)
			defer conn.Close()
			go mustCopy(zone,conn,chans[i-1])
		}
	}
	for i:=0;i<len(chans);i++{
		<-chans[i]
		close(chans[i])
	}

}
