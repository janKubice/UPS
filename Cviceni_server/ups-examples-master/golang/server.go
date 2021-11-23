package main

import (
    "bufio"
    "fmt"
    "log"
    "net"
    "os"
)

const (
    connHost = "0.0.0.0"
    connPort = "10000"
    connType = "tcp"
)

func main() {
    socket, err := net.Listen(connType, connHost+":"+connPort)

    if err != nil {
        fmt.Println("Error listening:", err.Error())
        os.Exit(1)
    }
    defer socket.Close()

    for {
        client, err := socket.Accept()
        if err != nil {
            fmt.Println("Error on Accept", err.Error())
            return
        }

        fmt.Println("Client " + client.RemoteAddr().String() + " connected.")

        go handleConnection(client)
    }
}

func handleConnection(client net.Conn) {
    read_buffer, err := bufio.NewReader(client).ReadBytes('\n')

    if err != nil {
        fmt.Println("Client disconnected.")
        client.Close()
        return
    }

    log.Println("Msg:", string(read_buffer[:len(read_buffer)-1]))

    client.Write(read_buffer)

    client.Close()
}
