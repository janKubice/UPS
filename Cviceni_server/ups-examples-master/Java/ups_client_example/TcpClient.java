package a;
import java.io.*;
import java.net.*;


public class TcpClient implements NetworkInterface {

    Socket s;
    BufferedReader reader;
    PrintWriter writer;
    int port;
    String host;

    public TcpClient ( ) {
    }

    public TcpClient( String host, int port ) {
        open(host, port);
    }

    public void open( String ihost, int iport ) {
        host = ihost;
        port = iport;
        // create a socket to communicate to the specified host and port
        try {
            s = new Socket( host, port);
        }
        catch (IOException e) {
             System.out.println("Connection to " + host + ":" + port + " refused");
         }
        catch (IllegalArgumentException e){
            System.out.println("Illegal port - not in allowed range 0 - 65535");
        }
        catch (NullPointerException e){
            System.out.println("Hostname not supplied");
        }

        try {
            // create streams for reading and writing
            reader = new BufferedReader(new InputStreamReader(s.getInputStream()));
            writer = new PrintWriter(new OutputStreamWriter(s.getOutputStream()));
            // tell the user that we've connected
            System.out.println("Connected to " + s.getInetAddress() +
                    ":" + s.getPort());
        }
        // pouze java 1.7 funkcni
        //catch( NullPointerException | IOException e ) { }
        catch (Exception e) {}
    }

    public void close(  ) {
        try {
            reader.close();
            writer.close();
        }
        // pouze java 1.7 funkcni
        //catch (IOException | NullPointerException e) {
        catch (Exception e) {
            System.err.println("Close error");
        }
        //always be sure to close the socket
        finally {
            try {
                if (s != null) s.close();
            }
            catch (IOException e) { }
        }
    }

    public void putMessage( TcpMessage msg) {
        try {
    	    writer.println(msg.getMessage());
            writer.flush();
        }
        catch (Exception e) {
            System.err.println("Write error");
        }
    }

    public TcpMessage getMessage(  ) {
        String line;
        // read the response (a line) from the server
        try {
            line = reader.readLine();
            // write the line to console
            return new TcpMessage(line);
        }
        catch (IOException e) {
            System.err.println("Read error");
        }
        return null;
    }
    protected void finalize() { }
}
