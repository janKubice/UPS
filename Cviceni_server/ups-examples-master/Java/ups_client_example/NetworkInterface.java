package a;

public interface NetworkInterface {
	public void open( String ihost, int iport );
	public void close(  );
	public void putMessage( TcpMessage msg );
	public TcpMessage getMessage(  );
}
