// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
	"flag"
)

//!+
func main() {
	username := flag.String("user", "", "your username")
	server := flag.String("server", "localhost:8000","the server address to connect to 'ipaddress:port'")
	flag.Parse()
	if *username=="" {
		log.Println("a username is required to connect")
		return
	}
	conn, err := net.Dial("tcp", *server)
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	io.WriteString(conn, *username+"\n")
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
