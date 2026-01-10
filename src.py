import serial
import time
import pathGen
import snakeGame

from PIL import Image
from enum import Enum
#0=left, 1=right
ports = ['com3', 'com4']

class directions(Enum):
    up=0
    down=1
    left=2
    right=3

def send_command(command_id, parameters, port, with_response=False):
  with serial.Serial(port , 115200) as s:
      s.write([0x32, 0xAC, command_id] + parameters)

      if with_response:
          res = s.read(32)
          return res
      
def hamilton_snake():
  path=pathGen.Hamiltonian(9, 34)
  path.generate(2500)
  snakePath=path.getSnakePath()
  path.print_path()
  return snakePath
def run_snake():
   Game = snakeGame.Game(9,34)
   n=0
   path = hamilton_snake()
   while True:
      im = Game.drawGame()
      # For each column x, collect brightness bytes for y=0..8 and send them
      for x in range(im.width):  # 0..33
         col_bytes = [im.getpixel((x, y)) for y in range(im.height)]  # 9 values, each 0..255
         # Stage this column (one StageCol call per column)
      #   print(x)
         send_command(0x07, [x]+col_bytes, ports[1])

      # After staging all 34 columns, flush to show the frame
      send_command(0x08, [], ports[1])
      if n == len(path):
         n=0
      Game.move(path[n])
      n += 1
      #time.sleep(0.01)

run_snake()

    
#send_command(0x10, [0], ports[0])
#send_command(0x11, [3], ports[0])