import serial
#0=left, 1=right
ports = ['com4', 'com3']
def send_command(command_id, parameters, port, with_response=False):
  with serial.Serial(port , 115200) as s:
      s.write([0x32, 0xAC, command_id] + parameters)

      if with_response:
          res = s.read(32)
          return res

# Go to sleep and check the status
send_command(0x10, [0], ports[0])
send_command(0x11, [1], ports[0])
send_command(0x11, [2], ports[0])
send_command(0x11, [0], ports[0])
send_command(0x11, [2], ports[0])