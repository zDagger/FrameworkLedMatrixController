import serial
import time
import pathGen
import snakeGame
from PIL import Image
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
     time.sleep(0.44)
     send_command(0x11, [snakePath[n]], ports[0])
     n += 1
def run_snake():
   im = snakeGame.drawGame()

   # For each column x, collect brightness bytes for y=0..8 and send them
   for x in range(im.width):  # 0..33
      col_bytes = [im.getpixel((x, y)) for y in range(im.height)]  # 9 values, each 0..255
      # Stage this column (one StageCol call per column)
   #   print(x)
      send_command(0x07, [x]+col_bytes, ports[0])

   # After staging all 34 columns, flush to show the frame
   send_command(0x08, [], ports[0])

run_snake()

    
#send_command(0x10, [0], ports[0])
#send_command(0x11, [3], ports[0])