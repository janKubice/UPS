package main

import (
    "fmt"
    "bufio"
    "log"
    "net"
    "os"
)

const (
    serverHost = "localhost"
    serverPort = "10000"
    serverType = "tcp"
)

func main() {
    fmt.Println("Connecting to " + serverType + " server " + serverHost + ":" + serverPort)

    server, err := net.Dial(serverType, serverHost+":"+serverPort)
    if err != nil {
        fmt.Println("Error serverecting:", err.Error())
        os.Exit(1)
    }
    reader := bufio.NewReader(os.Stdin)

    fmt.Print("Text to send: ")

    input, _ := reader.ReadString('\n')

    server.Write([]byte(input))

    message, _  := bufio.NewReader(server).ReadString('\n')

    log.Print("Server reply: ", message)
}
