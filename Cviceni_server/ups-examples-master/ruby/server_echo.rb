require 'socket'

server = TCPServer.open(2000)

loop do
   Thread.start(server.accept) do |client|
     line = client.gets
     puts line
     client.puts(line)
     client.close
   end
end
