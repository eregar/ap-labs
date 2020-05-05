// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"strings"
	"time"
	"flag"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type user_client struct { // makes possible pass the username within channels
	channel client
	username string
	connection net.Conn
}
type userData struct{
	username string
	connection net.Conn
}

type cmd struct{
	command int
	channel client
	destiny string
	message string
}

const USERS,MSG,USER,KICK int =0,1,2,3

var (
	entering = make(chan user_client)
	leaving  = make(chan client,1)
	messages = make(chan string,1) // all incoming client messages
	rights = make(chan bool,2)
	command = make(chan cmd)
)

func broadcaster() {
	clients := make(map[client]userData) // all connected clients
	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range clients {
				cli <- msg
			}

		case cli := <-entering:
			if len(clients)==0 {
				clients[cli.channel] = userData{cli.username,cli.connection}
				rights <- true
				log.Println("irc-server > New connected user "+cli.username)
				log.Println("irc-server > "+cli.username+" was promoted to channel ADMIN")
			} else {
				duplicate:=false
				for _,users:= range clients {
					if cli.username==users.username {
						duplicate=true
						break
					}
				}
				if duplicate==false {
					clients[cli.channel] = userData{cli.username,cli.connection}
					rights <- false
					log.Println("irc-server > New connected user "+cli.username)
				} else {
					cli.channel <- "irc-server > someone is already using that username!"
					rights <- false
					close(cli.channel)
					cli.connection.Close()
					messages <- "irc-server > " + cli.username + " was denied entrance, duplicate usernames"
				}
			}
		case cli := <-leaving:
			if _,ok := clients[cli];ok {
				delete(clients, cli)
				close(cli)
			}
		case cli := <- command:
			switch cli.command {
			case USERS:
                        	tmp:=""
                        	for _,users:= range clients{
                                	tmp+= ", "+users.username
                        	}
                        	cli.channel <- "irc-server > " + tmp[2:]
			case MSG:
				found:=false
				for channel,users:= range clients{
					if cli.destiny==users.username{
						found=true
						channel <- "From->"+clients[cli.channel].username+": "+cli.message
						cli.channel <-"To->"+users.username+": "+cli.message
						break
					}
				}
				if found==false{
					cli.channel <- "irc-server > username "+cli.destiny+" was not found"
				}
			case USER:
				found:=false
				for _,users:=range clients{
					if cli.destiny==users.username {
						found=true
						cli.channel <-"irc-server > username: "+users.username+", ip: "+users.connection.RemoteAddr().String()
						break
					}
				}
                                if found==false{
                                        cli.channel <- "irc-server > username "+cli.destiny+" was not found"
                                }
			case KICK:
				found:=false
				for channel,users:=range clients{
					if cli.destiny==users.username{
						found=true
						channel <- "irc-server > you were kicked from the server"
						time.Sleep(10 *time.Millisecond)
						messages <- "irc-server > " +users.username+" was kicked from the server"
						leaving <- channel
						log.Println("irc-server > "+users.username+" was kicked")
						users.connection.Close()
						break
					}
				}
                                if found==false{
                                        cli.channel <- "irc-server > username "+cli.destiny+" was not found"
                                }
			}
		}
	}
}

//!-broadcaster

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	input := bufio.NewScanner(conn)
	input.Scan()
	who := strings.ReplaceAll(input.Text()," ","_")
	ch <- "irc-server > You are " + who
	messages <- "irc-server > " + who + " has arrived"
	entering <- user_client{ch,who,conn}
	admin := <- rights
	if admin {
		ch <- "irc-server > you are now an admin!"
	}
	for input.Scan() {
		text:=input.Text()
		if strings.HasPrefix(text,"/"){
			line := strings.Split(text," ")
			switch line[0]{
			case "/users":
				command <- cmd{USERS,ch,"",""}
			case "/user":
				if len(line)>1{
					command <- cmd{USER,ch,line[1],""}
				}else{
					ch <- "irc-server > incomplete command"
				}
			case "/msg":
				if len(line)>2{
					if line[1]!=who{
						command <- cmd{MSG,ch,line[1],text[6+len(line[1]):]}
					}else{
						ch<- "irc-server > you cant message yourself!"
					}
				}else{
					ch <- "irc-server > incomplete command"
				}
			case "/time":
				v,_:=time.Now().Zone()
				ch <- "irc-server > Local time: "+v+" "+time.Now().Format("15:04:05")
			case "/kick":
				if admin {
					if len(line)>1{
						if line[1]!=who{
							command <- cmd{KICK,ch,line[1],""}
						}else {
							ch <- "irc-server > you cant kick yourself!"
						}
					}else{
						ch <- "irc-server > incomplete command"
					}
				} else {
					ch <- "irc-server > you are not an admin!"
				}
			}
		} else {
			messages <- who + " > " + text
		}
	}
	// NOTE: ignoring potential errors from input.Err()

	leaving <- ch
	log.Println("irc-server > "+who+" left")
	messages <- "irc-server > " + who + " has left"
	conn.Close()
}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	host:=flag.String("host","localhost","the ip address")
	port:=flag.String("port","8000","the connection port")
	flag.Parse()
	listener, err := net.Listen("tcp", *host+":"+*port)
	if err != nil {
		log.Fatal(err)
	}
	log.Println("irc-server > Simple IRC Server started at "+*host+":"+*port)
	go broadcaster()
	log.Println("irc-server > Ready for receiving new clients")
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
