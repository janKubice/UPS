import pygame
pygame.init()

class grid(object):
    """
    Třída reprezentujícíc grid
    """
    def __init__(self, win, width, height, cols, rows, showGrid=False, startx = 0, starty = 0, bg=(255,255,255)):
        self.width = width
        self.height = height
        self.cols = cols
        self.rows = rows
        self.bg = bg
        self.startx = startx
        self.starty = starty
        self.lineThick = 1
        self.showGrid = showGrid
        self.isSelected = None
        self.grid = None

        self.screen = win
        pygame.display.update()
                    
    def getGrid(self):
        return self.grid

    def drawGrid(self, lineColor=(0,0,0)):
        x = self.startx
        y = self.starty
        
        for i in range(self.cols):
            y = self.starty + self.height
            if i > 0:
                x += (self.width / self.cols)
            for j in range(self.rows):
                y -= self.height / self.rows 
                pygame.draw.rect(self.screen, (0,0,0),(x, y, self.width / self.cols, self.height/ self.rows), 1)

    def clicked(self, pos): 
        """
        Vrátí pozici kliknutí v gridu
        """
        try:
            t = pos[0]
            w = pos[1]
            g1 = int((t - self.startx) / self.grid[0][0].w)
            g2 = int((w - self.starty) / self.grid[0][0].h)

            self.selected = self.grid[g1][g2]

            return self.grid[g1][g2]
        
        except IndexError:
            return False
            
    def get_position(self, pos):
        t = pos[0]
        w = pos[1]
        g1 = int((t - self.startx) / self.grid[0][0].w)
        g2 = int((w - self.starty) / self.grid[0][0].h)
        return [g1,g2]

    def isSelected(self):
        """
        Vrátí vybraný objekt
        """
        return self.selected



class pixelArt(grid):  
    """
    Vykresluje pixeli na grid
    """
    def drawGrid(self):
        self.grid = []
        for i in range(self.cols):
            self.grid.append([])
            for j in range(self.rows):
                self.grid[i].append(pixel(i, j, self.width, self.height, self.cols, self.rows, self.startx, self.starty, self.showGrid))
                self.grid[i][j].show(self.screen, (255,255,255), self.lineThick)
                if self.showGrid:
                    self.grid[i][j].show(self.screen, (0,0,0), 1,False,True)
                    
       
        for c in range(self.cols):
            for r in range(self.rows):
                self.grid[c][r].getNeighbors(self.grid)
        
        self.selected = self.grid[self.cols - 1][self.rows - 1]

        
    def clearGrid(self): 
        """
        Nastaví pixeli na stejnou barvu jako pozadí
        """
        for pixels in self.grid:
            for p in pixels:
                if self.showGrid: 
                    p.show(self.screen, self.bg, 0)
                    p.show(self.screen, (0,0,0), 1)
                else:
                    p.show(self.screen, self.bg, 0)



class colorPallet(pixelArt):
    def setColor(self, colorList): 
        colourCount = 0

        for pixels in self.getGrid():
            for p in pixels:
                p.show(self.screen, colorList[colourCount],0)
                colourCount += 1


class menu(grid):
    """
    Třída vytvoří jednoduché menu které může obsahovat text
    """
    def setText(self, textList):
        
        self.grid = []
        for i in range(self.cols):
            self.grid.append([])
            for j in range(self.rows):
                self.grid[i].append(textObject(i, j, self.width, self.height, self.cols, self.rows, self.startx, self.starty))
        c = 0
        for spots in self.getGrid():
            for s in spots:
                s.showText(self.screen, textList[c])
                c += 1


class textObject():
    def __init__(self, i, j, width, height, cols, rows, startx=0, starty=0):
        self.col = i #The column of the current instance in the grid
        self.row = j #The row of the current instance in the grid
        self.rows = rows #Total amount of rows
        self.cols = cols #Total amount of columns
        self.w = width / cols
        self.h = height / rows
        self.x = self.col * self.w + startx
        self.y = self.row * self.h + starty
        self.text = ''
    
    def showText(self, win, txt): 
        """
        Vykreslí text na obrazovku
        """
        self.text = txt
        myFont = pygame.font.SysFont('comicsansms', 15)
        text = myFont.render(self.text, 1, (0,0,0))
        win.blit(text, (self.x + (self.w /2 - text.get_width() / 2), self.y + (self.h/2 - text.get_height() / 2))) 

    def show(self, screen, color, st, outline=False): 
        pygame.draw.rect(screen, color, (self.x, self.y, self.w, self.h), st)


class pixel():
    """
    Třída reprezentující jeden pixel
    """
    def __init__(self, i,j, width, height, cols, rows, startx=0, starty=0, showGrid=False):
        self.col = i 
        self.row = j 
        self.color = (255,255,255)
        self.rows = rows 
        self.cols = cols 
        self.showGrid = showGrid
        self.w = width / cols
        self.h = height / rows
        self.x = self.col * self.w + startx
        self.y = self.row * self.h + starty
        self.neighbors = [] 
        
    def show(self, screen, color, st, outline=False, first=False):
        """
        Zobrazí pixel
        """
        if not(first):
            self.color = color
            
        pygame.draw.rect(screen, color, (self.x, self.y, self.w, self.h), st)
        if self.showGrid and not(outline):
            pygame.draw.rect(screen, (0,0,0), (self.x, self.y, self.w, self.h), 1)
        
    def getPos(self):
        """Získá pozici pixelu
        Returns:
            tuple: pozice ve formátu (x,y) od horního levého kraje
        """
        return (self.col * self.w, self.row * self.h)

    def click(self, screen, color):
        self.show(screen, color, 0)
        self.color = color

    def getColor(self):
        return self.color

    def getNeighbors(self, grid):
        i = self.col 
        j = self.row 
        rows = self.rows
        cols = self.cols

        if i < cols-1: #pravá
            self.neighbors.append(grid[i + 1][j])
        if i > 0: #levá
            self.neighbors.append(grid[i - 1][j])
        if j < rows-1: #nahoru
            self.neighbors.append(grid[i][j + 1])
        if j > 0 : #dolů
            self.neighbors.append(grid[i][j - 1])

            
        if j > 0 and i > 0: #horní levá
            self.neighbors.append(grid[i - 1][j - 1])

        if j + 1 < rows and i > -1 and i - 1 > 0: #dolní levá
            self.neighbors.append(grid[i - 1][j + 1])

        if j - 1 < rows and i < cols - 1 and j - 1 > 0: #horní pravá
            self.neighbors.append(grid[i + 1][j - 1])
            
        if j < rows - 1 and i < cols - 1: #dolní pravá
            self.neighbors.append(grid[i + 1][j + 1])
        
    def neighborsReturn(self):
        """
        Vrátí list sousedů
        """
        return self.neighbors #Return a list of the neighbours of the current pixel
