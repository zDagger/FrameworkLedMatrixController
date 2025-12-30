import sys
from PIL import Image, ImageDraw
import random

def drawGame():
    im = Image.new('L', (9, 34), 5)  # 255 = white (max brightness)
    apple = (random.randint(0,8), random.randint(0,33))
    draw=ImageDraw.Draw(im)
    draw.point(apple, 255)
    return im

