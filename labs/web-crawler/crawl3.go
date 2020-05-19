// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"fmt"
	"log"
	"flag"
	"gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

func crawl(url string) []string {
	//fmt.Println(url)
	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(url)
	<-tokens // release the token

	if err != nil {
		log.Print(err)
	}
	return list
}

//!-sema

type linkdepth struct{
	links []string
	depth int
}


//!+
func main() {
	limit:=flag.Int("depth",1,"depth of the webcrawler")
	flag.Parse()
	args := flag.Args()
	if len(args)<1{
		fmt.Println("at least 1 URL needed to execute")
		return
	}
	if *limit<1{
		fmt.Println("depth must be greater than 0")
		return
	}
	worklist := make(chan linkdepth)
	var n int // number of pending sends to worklist

	// Start with the command-line arguments.
	n++
	go func() { worklist <- linkdepth{args[0:],*limit} }()

	// Crawl the web concurrently.
	seen := make(map[string]bool)
	for ; n > 0; n-- {
		list := <-worklist
		for _, link := range list.links {
			if !seen[link] {
				seen[link] = true
				fmt.Println(link)
				if list.depth>0{
					n++
					//fmt.Println("crawling",link,"depth:",list.depth)
					go func(link string) {
						worklist <- linkdepth{crawl(link),list.depth-1}
					}(link)
				}
			}
		}
	}
}

//!-
