import pygame
from tkinter import *
from tkinter import messagebox
from tkinter.filedialog import askopenfilename, asksaveasfilename
import grid
from grid import colorPallet
from grid import pixelArt
from grid import menu
from grid import grid
import sys
import time

sys.setrecursionlimit(1000000)

pygame.init()
paintBrush = pygame.image.load(r"sp\art\Paintbrush.png")
currentVersion = 1.1

#Defaultní hodnoty
rows = 50
cols = 50
wid = 600
heigh = 600

checked = []
def fill(spot, grid, color, c):
   if spot.color != c:
      pass
   else:
      spot.click(grid.screen, color)
      pygame.display.update()
      
      i = spot.col 
      j = spot.row 

      if i < cols-1: #pravá
         fill(grid.getGrid()[i + 1][j], grid, color, c)
      if i > 0: #levá
         fill(grid.getGrid()[i - 1][j], grid, color, c)
      if j < rows-1: #nahoru
         fill(grid.getGrid()[i][j + 1], grid, color, c)
      if j > 0 : #dolů
         fill(grid.getGrid()[i][j - 1], grid, color, c)
   



def changeCaption(txt):
   pygame.display.set_caption(txt)


def onsubmit(x=0):
    global cols, rows, wid, heigh
    
    st = rowsCols.get().split(',') 
    window.quit()
    window.destroy()
    try: 
        if st[0].isdigit(): 
            cols = int(st[0])
            while 600//cols != 600/cols:
               if cols < 300:
                  cols += 1
               else:
                  cols -= 1
        if st[1].isdigit():
            rows = int(st[1])
            while 600//rows != 600/rows:
               if rows < 300:
                  rows += 1
               else:
                  rows -= 1
        if cols > 300:
          cols = 300
        if rows > 300:
          rows = 300

    except:
        pass


def updateLabel(a, b, c):
   sizePixel = rowsCols.get().split(',')
   l = 12
   w = 12
   
   try:
      l = 600/int(sizePixel[0])
   except:
      pass

   try:
      w = 600/(int(sizePixel[1]))
   except:
      pass

   label1.config(text='Pixel Size: ' + str(l) + ', ' + str(w))



def initalize(cols, rows, showGrid=False):
   """
   Vytvoří obrazovu
   """
   global pallet, grid, win, tools, lineThickness, saveMenu

   try:
      del grid
   except:
      pass
   
   pygame.display.set_icon(paintBrush)   
   win = pygame.display.set_mode((int(wid), int(heigh) + 100))
   pygame.display.set_caption('Untitled')
   win.fill((255,255,255))

   grid = pixelArt(win, int(wid), int(heigh), cols, rows, showGrid)
   grid.drawGrid()

   pallet = colorPallet(win, 90, 90, 3, 3, True, 10, grid.height + 2)
   pallet.drawGrid()

   colorList = [(0,0,0), (255,255,255), (255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,168,0), (244, 66, 173), (65, 244, 226)]
   pallet.setColor(colorList)

   tools = menu(win, 200, 40, 5, 1, True, grid.width - 210, grid.height + 50)
   tools.drawGrid()

   buttons = ['D', 'E', 'F', 'R', 'C']
   tools.setText(buttons)
   tools.drawGrid()

   l = tools.getGrid()
   l[0][0].show(grid.screen, (255,0,0),1, True)

   lineThickness = menu(win, 180, 40, 4, 1, True, grid.width - 200, grid.height + 10)
   lineThickness.drawGrid()

   buttons = ['1', '2', '3', '4']
   lineThickness.setText(buttons)

   pygame.display.update()

#-----------------------------------------------------------------------#

window = Tk()
window.title('Paint Program')

t_var = StringVar()
t_var.trace('w', updateLabel)

label = Label(window, text='# Of Rows and Columns (25,50): ')
rowsCols = Entry(window, textvariable=t_var)

label1 = Label(window, text="Pixel Size: 12.0, 12.0")
var = IntVar()
c = Checkbutton(window, text="View Grid", variable=var)
submit = Button(window, text='Submit', command=onsubmit)
window.bind('<Return>', onsubmit)

submit.grid(columnspan=1, row=3, column=1,pady=2)
c.grid(column=0, row=3)
label1.grid(row=2)
rowsCols.grid(row=0, column=1, pady=3, padx=8)
label.grid(row=0, pady=3)

window.update()
mainloop()

#------------------------------------------------------------------------#


#Hlavní smička
initalize(cols, rows, var.get())
pygame.display.update()
color = (0,0,0) 
thickness = 1
replace = False
doFill = False

run = True
while run:
    ev = pygame.event.get()

    for event in ev:
        if event.type == pygame.QUIT:
            window = Tk()
            window.withdraw()
            
            run = False
         
        if pygame.mouse.get_pressed()[0]: 
            try:
                pos = pygame.mouse.get_pos()
                if pos[1] >= grid.height: 
                    if pos[0] >= tools.startx and pos[0] <= tools.startx + tools.width and pos[1] >= tools.starty and pos[1] <+ tools.starty + tools.height: #If the mouse ic clicking on the tools grid
                        replace = False
                        doFill = False
                        tools.drawGrid() 
                        buttons = ['D', 'E', 'F', 'R', 'C']
                        tools.setText(buttons)
                        
                        clicked = tools.clicked(pos)
                        clicked.show(grid.screen, (255,0,0), 1, True)

                        if clicked.text == 'D': #Draw  
                            color = (0,0,0)
                        elif clicked.text == 'E': #Erase
                            color = (255,255,255)
                        elif clicked.text == 'F':# Fill
                            doFill = True
                        elif clicked.text == 'R':# Replace
                            replace = True
                        elif clicked.text == 'C':# Clear 
                            grid.clearGrid()
                            tools.drawGrid() 
                            buttons = ['D', 'E', 'F', 'R', 'C']
                            tools.setText(buttons)
                            l = tools.getGrid()
                            l[0][0].show(grid.screen, (255,0,0),1, True)
                            
                    elif pos[0] >= pallet.startx and pos[0] <= pallet.startx + pallet.width and pos[1] >= pallet.starty and pos[1] <= pallet.starty + pallet.height:
                        clicked = pallet.clicked(pos)
                        color = clicked.getColor() 

                        pallet = colorPallet(win, 90, 90, 3, 3, True, 10, grid.height + 2)
                        pallet.drawGrid()

                        colorList = [(0,0,0), (255,255,255), (255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,168,0), (244, 66, 173), (65, 244, 226)]
                        pallet.setColor(colorList)
                        clicked.show(grid.screen, (255,0,0), 3, True)
                        
                    elif pos[0] >= lineThickness.startx and pos[0] <= lineThickness.startx + lineThickness.width and pos[1] >= lineThickness.starty and pos[1] <= lineThickness.starty + lineThickness.height:
                        lineThickness.drawGrid()
                        buttons = ['1', '2', '3', '4']
                        lineThickness.setText(buttons)
                        
                        clicked = lineThickness.clicked(pos)
                        clicked.show(grid.screen, (255,0,0), 1, True)

                        thickness = int(clicked.text) 

                            
                else:
                    if replace:
                        tools.drawGrid()
                        buttons = ['D', 'E', 'F', 'R', 'C']
                        tools.setText(buttons)
                        
                        tools.getGrid()[0][0].show(grid.screen, (255,0,0), 1, True)

                        clicked = grid.clicked(pos)
                        c = clicked.color
                        replace = False

                        for x in grid.getGrid():
                            for y in x:
                                if y.color == c:
                                    y.click(grid.screen, color)
                    elif doFill:
                       clicked = grid.clicked(pos)
                       if clicked.color != color:
                          fill(clicked, grid, color, clicked.color)
                          pygame.display.update()
                                    
                    else:
                        name = pygame.display.get_caption()[0]
                        if name.find("*") < 1:
                           changeCaption(name + '*')

                        clicked = grid.clicked(pos)
                        clicked.click(grid.screen,color)
                        if thickness == 2:
                            for pixel in clicked.neighbors:
                                pixel.click(grid.screen, color)
                        elif thickness == 3:
                            for pixel in clicked.neighbors:
                                pixel.click(grid.screen, color)
                                for p in pixel.neighbors:
                                    p.click(grid.screen, color)
                        elif thickness == 4:
                            for pixel in clicked.neighbors:
                                pixel.click(grid.screen, color)
                                for p in pixel.neighbors:
                                    p.click(grid.screen, color)
                                    for x in p.neighbors:
                                        x.click(grid.screen, color)
                                         
                pygame.display.update()
            except AttributeError:
                pass

pygame.quit()
