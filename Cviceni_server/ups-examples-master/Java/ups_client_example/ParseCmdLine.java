package a;

class ParseCmdLine {
    int port = 7654;
    String hostname = "localhost";

    public ParseCmdLine(String[] args) {

        int i = 0;
        String arg;

        while (i < args.length && args[i].startsWith("-")) {
            arg = args[i++];
            if (arg.equals("-host")) {
               if (i < args.length)
                    hostname = args[i++];
                else
                    System.err.println("-host requires hostname");
            }
            else if (arg.equals("-port")){
                if (i < args.length)
                    try {
                        port = Integer.parseInt(args[i++]);
                    } catch (NumberFormatException e) {
                        System.err.println("-port not a number, setting to 7654");
                        port = 7654;
                    } finally {
                        if (port >= 65535) System.err.println("-port not in range <65535");
                    }
                else
                    System.err.println("-port requires port (<65535)");
            }
            else {
                System.err.println("TcpApplication: illegal option " + arg);
            }
        }
    }
    void usage() {
        System.err.println("Usage: TcpApplication [-port port] [-host hostname]");
    }
}
