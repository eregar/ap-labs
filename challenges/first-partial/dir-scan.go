package main

import (
	"fmt"
	"os"
	"path/filepath"
)

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
	dirs,links,devices,sockets,others := 0,0,0,0,0
	res:= filepath.Walk(dir,func(path string, info os.FileInfo, err  error) error{
	    if info.IsDir(){
		dirs++
	    }else if info.Mode()&os.ModeSymlink != 0 {
		links++
	    }else if info.Mode()&os.ModeDevice!=0 {
		devices++
	    }else if info.Mode()&os.ModeSocket!=0 {
		sockets++
	    }else {
		others++
		}
	    return nil
	})
	if res != nil {
	    fmt.Println(res)
	}
	fmt.Println("Directory Scanner Tool")
	fmt.Println("+-------------------------+------+")
	fmt.Println("| Path                    |",dir,"|")
	fmt.Println("+-------------------------+------+")
	fmt.Println("| Directories             |",dirs,"   |")
	fmt.Println("| Symbolic Links          |",links,"   |")
	fmt.Println("| Devices                 |",devices,"   |")
	fmt.Println("| Sockets                 |",sockets,"   |")
	fmt.Println("| Other files             |",others,"   |")
	fmt.Println("+-------------------------+------+")

	return res
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	scanDir(os.Args[1])
}
