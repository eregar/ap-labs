package main

import (
	"fmt"
	"time"
	"os"
	"flag"
)

var ( REPEATS =1000000)

func racket(reciever <- chan struct{},sender chan <- struct{},output chan <- struct{}){

	for t:=0 ; t<=REPEATS ; t++{
		<- reciever
		sender <- struct{}{}
	}
	output<-struct{}{}
}

func main() {
	of:=flag.String("file","out.txt","the output file")
	flag.Parse()
	file, err := os.Create(*of)
    	if err != nil {
        	fmt.Println(err)
        	return
    	}
	ping:=make(chan struct{})
	pong:=make(chan struct{})
	done:=make(chan struct{},2)
	t0 := time.Now()
	go racket(ping,pong,done)
	go racket(pong,ping,done)
	ping<- struct{}{}
	<-done
	t1 := time.Now()
	dur:=int64(time.Second*time.Duration(REPEATS*2)/t1.Sub(t0))
	_ ,err = file.WriteString(fmt.Sprintf("Program can sustain %d communications per second.\n",dur))
	if err != nil {
        	fmt.Println(err)
        	file.Close()
        	return
    	}
	close(ping)
	close(pong)
	close(done)
	file.Close()
	fmt.Println("results generated at",*of)
}

