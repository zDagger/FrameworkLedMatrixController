import sys
from PIL import Image, ImageDraw
import random
from enum import Enum

class directions(Enum):
    up=0
    down=1
    left=2
    right=3




class Game:
    game = 'snake'
    def __init__(self,width,height):
        self.snake = [[1,1]]
        self.apple=[random.randint(0,width-1), random.randint(0,height-1)]
        self.width = width
        self.height = height
        self.death = False
        self.toggle = False
        while self.apple in self.snake:
            self.apple=[random.randint(0,width-1), random.randint(0,height-1)]
        self.im = Image.new('L', (width, height))
    def drawGame(self):
        if not self.death:
            self.im = Image.new('L', (self.width, self.height))
            draw=ImageDraw.Draw(self.im)
            draw.point(self.apple, 255)
            for i in range(len(self.snake)):
                    draw.point(self.snake[i], int(150 *((len(self.snake)-i)/(len(self.snake)))))
        else:
            if self.toggle:
                self.im = Image.new('L', (self.width, self.height),255)
                self.toggle = False
            else:
                self.im = Image.new('L', (self.width, self.height))
                self.toggle = True
        return self.im
    def newApple(self):
        self.apple=[random.randint(0,self.width-1), random.randint(0,self.height-1)]
        while self.apple in self.snake:
            self.apple=[random.randint(0,self.width-1), random.randint(0,self.height-1)]
    def move(self, direction):
        head = [self.snake[0][0], self.snake[0][1]]
        if direction == directions.up.value:
            head[1] -= 1
        elif direction == directions.down.value:
            head[1] += 1
        elif direction == directions.left.value:
            head[0] -=1
        elif direction == directions.right.value:
            head[0] += 1
        if head in self.snake or  head[0] == self.width or head[0] < 0 or head[1] == self.height or head[1] < 0:
            self.death = True
        else:
            self.snake = [head] + self.snake
            if head == self.apple:
                self.newApple()
            else:
                self.snake.pop(len(self.snake)-1)
            



