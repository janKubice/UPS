require 'socket'
require 'securerandom'


server = TCPServer.open(2000)

fds = [server]

loop do
   Thread.start(server.accept) do |client|
     s = SecureRandom.hex
     client.puts(s)
     line = client.gets
     if line.strip == s.reverse
       client.puts("OK")
     else
       client.puts("ERROR")
     end
     client.close
   end
end
