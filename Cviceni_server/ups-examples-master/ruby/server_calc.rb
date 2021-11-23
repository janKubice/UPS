require 'socket'
require 'securerandom'

def calc(oper, o1, o2)
  if oper == "plus"
    o1 + o2
  elsif oper == "minus"
    o1 - o2
  elsif oper == "division"
    o1 / o2
  elsif oper == "multiply"
    o1 * o2
  end
end


server = TCPServer.open(2000)

ops= %w[ plus minus division multiply ]

loop do
   Thread.start(server.accept) do |client|
     client.puts("Calculator")
     client.puts("Meesage formet: 'operation|operand|operand'")
     client.puts("Opertion: #{ops}")
     line = client.gets
     oper, o1, o2 = line.split('|')
     puts ":#{oper} :#{o1} :#{o2}"
     if ops.include?(oper) && !o1.nil? && !o2.nil?
       res = "OK|#{calc(oper, o1.to_i, o2.to_i)}"
     else
       res = "ERROR|0"
     end
     client.puts(res)
     client.close
   end
end
