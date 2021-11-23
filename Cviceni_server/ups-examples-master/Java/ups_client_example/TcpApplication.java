package a;

public class TcpApplication
{

    public static void main(String[] args) {
        ParseCmdLine params = new ParseCmdLine(args);

        TcpClient client = new TcpClient( params.hostname, params.port );
        TcpMessage msgSend = new TcpMessage("Hello server");
        client.putMessage( msgSend );
        TcpMessage msgRecv = client.getMessage( );
        System.out.println(msgRecv.getMessage());
        client.close( );
    }
}
