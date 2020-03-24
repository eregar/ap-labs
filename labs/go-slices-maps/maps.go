package main

import (
        "golang.org/x/tour/wc"
        "strings"
)

func WordCount(s string) map[string]int {
        var m map[string]int
        m=make(map[string]int)
        a:= strings.Split(s," ")

        for _,word := range a{
                m[word]++
        }
        return m
}

func main() {
        wc.Test(WordCount)
}

