import serial
import time
import pathGen
#0=left, 1=right
ports = ['com4', 'com3']
def send_command(command_id, parameters, port, with_response=False):
  with serial.Serial(port , 115200) as s:
      s.write([0x32, 0xAC, command_id] + parameters)

      if with_response:
          res = s.read(32)
          return res
      
def hamilton_snake():
  u=0
  d=1
  l=2
  r=3
  n=0
  path=pathGen.Hamiltonian(9, 34)
  snakePath=path.getSnakePath()
  print(snakePath)
  #print(snakePath)
  send_command(0x10, [0], ports[0])
  while True:
     if n == len(snakePath):
        n = 0
     print(snakePath[n])
     time.sleep(0.41)
     send_command(0x11, [snakePath[n]], ports[0])
     n += 1
hamilton_snake()

    
#send_command(0x10, [0], ports[0])
#send_command(0x11, [3], ports[0])