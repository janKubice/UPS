package a;
import java.io.*;
import java.net.*;


public class TcpMessage {

    String  message;
    int     bytes;
    public TcpMessage ( String msg) {
        message = msg;
        bytes = msg.length();
    }

    public TcpMessage( ) {
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String msg) {
        message = msg;
        bytes = msg.length();
    }
}
