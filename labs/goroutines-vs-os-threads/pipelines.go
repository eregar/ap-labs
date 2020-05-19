package main

import (
	"fmt"
	"time"
	"os"
	"flag"
)


func onarrival(tail chan time.Time, stage int, lock chan struct{},f *os.File) {
	totalTime:=time.Now().Sub(<-tail)
	<- lock
	_,err:=f.WriteString(fmt.Sprintf("pipe: %d     time to get to pipe: %v\n", stage, totalTime))
	if err!=nil {
		fmt.Println("couldnt write string")
	}
	lock <- struct{}{}
}

func connect(source, destiny chan time.Time) {
	for msg := range source {
		destiny <- msg
	}
}

//routine->routine->routine->routine->routine->
func main() {
	of:=flag.String("file","out.txt","output file to record results")
	dur:=flag.Int("max",-1,"max goroutine number to let the program create")
	flag.Parse()
	file, err := os.Create(*of)
        if err != nil {
                fmt.Println(err)
                return
        }
	start:= make(chan time.Time)
	tail := start
	lock:= make(chan struct{},1)
	lock <- struct{}{}
	abort:=*dur
	for stage:= 0; stage!=abort ; stage++ {
		go onarrival(tail, stage,lock,file)
		start <- time.Now()
		temp := make(chan time.Time)
		go connect(tail, temp)
		tail = temp
	}
	<-lock
	fmt.Println("progress recorded at",*of)
}


