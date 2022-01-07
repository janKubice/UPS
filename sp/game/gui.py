import pygame
from tkinter import *
import grid
from grid import colorPallet
from grid import pixelArt
from grid import menu
from grid import grid
import sys
import client

class Gui:
    """
    Třída pracující s GUI celé aplikace
    Není to žádný zázrak a je to docela ošklivé GUI ale funguje a to stačí!
    """
    def __init__(self) -> None:
        sys.setrecursionlimit(1000000)
        pygame.init()

        #self.paintBrush = pygame.image.load("/home/jan/Documents/UPS/UPS/sp/game/art/Paintbrush.png")
        self.paintBrush = pygame.image.load("sp/game/art/Paintbrush.png")

        #Defaultní hodnoty
        self.rows = 50
        self.cols = 50
        self.width = 600
        self.height = 600

        self.checked = []
        self.client:client.Client = None
        self.clicked = None

    def set_client(self, client):
        self.client = client

    def fill(self, spot, grid, color, c):
        if spot.color != c:
            pass
        else:
            spot.click(grid.screen, color)
            pygame.display.update()
            
            i = spot.col 
            j = spot.row 

            if i < self.cols-1: #pravá
                self.fill(grid.getGrid()[i + 1][j], grid, color, c)
            if i > 0: #levá
                self.fill(grid.getGrid()[i - 1][j], grid, color, c)
            if j < self.rows-1: #nahoru
                self.fill(grid.getGrid()[i][j + 1], grid, color, c)
            if j > 0 : #dolů
                self.fill(grid.getGrid()[i][j - 1], grid, color, c)

    def changeCaption(self, txt):
        pygame.display.set_caption(txt)

    def draw_pixel(self, x, y, color):
        self.clicked = grid.clicked((x,y))
        self.clicked.click(grid.screen,color)

    def draw_item_to_draw(self, text):
        pass

    def draw_quess_response(self, text):
        pass

    def initalize_game(self, cols, rows, showGrid=False):
        """
        Vytvoří obrazovku hry
        """
        global pallet, grid, win, tools, lineThickness, saveMenu

        try:
            del grid
        except:
            pass
        
        pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height) + 100))
        pygame.display.set_caption('Untitled')
        win.fill((255,255,255))

        grid = pixelArt(win, int(self.width), int(self.height), cols, rows, showGrid)
        grid.drawGrid()

        pallet = colorPallet(win, 90, 90, 3, 3, True, 10, grid.heightt + 2)
        pallet.drawGrid()

        colorList = [(0,0,0), (255,255,255), (255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,168,0), (244, 66, 173), (65, 244, 226)]
        pallet.setColor(colorList)

        pygame.display.update()

    def draw_menu(self):
        """
        První obrazovka kterou uživatel vidí
        """
        pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Menu')
        win.fill((255,255,255))

        base_font = pygame.font.Font(None, 32)
        color_white = (255,255,255)
        color_light = (170,170,170)
        color_dark = (100,100,100)

        btn_w = 140
        btn_h = 40
        
        user_text = ''
        text_font = pygame.font.SysFont('Corbel',35)
        text = text_font.render('Connect' , True , color_white)
        input_rect = pygame.Rect(200, 200, 140, 32)
        color_active = pygame.Color('lightskyblue3')
        color_passive = pygame.Color('chartreuse4')

        color = color_passive
        active = False
        
        while True:
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    pygame.quit()
                    
                #ověření jestli se klikla myš
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    
                    #je kliknutí v pozici tlačítka?
                    if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                        #TODO pak odstranit
                        """ if self.client.connect():
                            print('Povedlo se připojit na server')
                            self.client.send_get_id(user_text)
                        else:
                            print('Nepovedlo se připojit na server') """
                        self.client.receive_id("1")

                    if input_rect.collidepoint(ev.pos):
                        active = True
                    else:
                        active = False

                if ev.type == pygame.KEYDOWN:
                    if ev.key == pygame.K_BACKSPACE:
                        user_text = user_text[:-1]
                    else:
                        if len(user_text) < 7:
                            user_text += ev.unicode
                        
            win.fill((60,25,60))

            if active:
                color = color_active
            else:
                color = color_passive
            mouse = pygame.mouse.get_pos()

            pygame.draw.rect(win, color, input_rect)
            text_surface = base_font.render(user_text, True, (255, 255, 255))  
            input_rect.w = max(100, text_surface.get_width()+10)
            
            #hover přes tlačítko
            if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                pygame.draw.rect(win,color_light,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2),btn_w,btn_h])
            else:
                pygame.draw.rect(win,color_dark,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2),btn_w,btn_h])
            

            win.blit(text , ((self.width/2)-50,self.height/2-(btn_h/2)))
            win.blit(text_surface, (input_rect.x+5, input_rect.y+5))
            pygame.display.update()

    def draw_connection(self, succ:bool = True):
        """
        Obrazovka s textovým inputem pro zadání ID místnosti
        """
        pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Menu')
        win.fill((255,255,255))

        color_white = (255,255,255)
        color_light = (170,170,170)
        color_dark = (100,100,100)
        color_red = (255,0,0)

        btn_w = 140
        btn_h = 40
        
        text_font = pygame.font.SysFont('Corbel',35)
        base_font = pygame.font.Font(None, 32)
        text_join = text_font.render('Join' , True , color_white)
        text_create = text_font.render('Create' , True , color_white)

        text_id = text_font.render(f'Your id: {self.client.id}', True, color_white)
        text_name = text_font.render(f'Player name: {self.client.name}', True, color_white)
        text_wrong_id = text_font.render('Wrong id format!', True, color_red)
        text_connection_fail = text_font.render('Connection fail', True, color_red)

        user_text = ''
        input_rect = pygame.Rect(200, 200, 140, 32)
        color_active = pygame.Color('lightskyblue3')
        color_passive = pygame.Color('chartreuse4')
        color = color_passive   

        active = False
        wrong_format_id = False
        
        while True:
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    #TODO odpojení hráče ze serveru -> uvolnění id na serveru
                    pygame.quit()
                    
                #ověření jestli se klikla myš
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    succ = True
                    #je kliknutí v pozici tlačítka?
                    if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                        #TODO pak odstranit
                        """ if self.client.send_connect_to_game(user_text):
                            wrong_format_id = False
                        else:
                            wrong_format_id = True
                            print('ID místnosti není ve správném formátu') """

                        self.client.receive_connect_response("pepa,adam")

                    if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2)+btn_h*1.5 <= mouse[1] <= self.height/2+(btn_h/2)+btn_h*1.5:
                        #TODO odstranit
                        #self.client.send_create_new_game()
                        self.client.receive_create_game_response('34')

                    if input_rect.collidepoint(ev.pos):
                        active = True
                    else:
                        active = False

                if ev.type == pygame.KEYDOWN:
                    succ = True
                    if ev.key == pygame.K_BACKSPACE:
                        user_text = user_text[:-1]
                    else:
                        if len(user_text) < 5:
                            user_text += ev.unicode
                                
            win.fill((60,25,60))
  
            if active:
                color = color_active
            else:
                color = color_passive
            mouse = pygame.mouse.get_pos()

            pygame.draw.rect(win, color, input_rect)
            text_surface = base_font.render(user_text, True, (255, 255, 255))  
            input_rect.w = max(100, text_surface.get_width()+10)
            
            #hover přes tlačítko
            if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                pygame.draw.rect(win,color_light,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2),btn_w,btn_h])
            else:
                pygame.draw.rect(win,color_dark,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2),btn_w,btn_h])

            #hover přes tlačítko
            if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2)+btn_h*1.5 <= mouse[1] <= self.height/2+(btn_h/2)+btn_h*1.5:
                pygame.draw.rect(win,color_light,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2)+btn_h*1.5,btn_w,btn_h])
            else:
                pygame.draw.rect(win,color_dark,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2)+btn_h*1.5,btn_w,btn_h])
            

            win.blit(text_join , ((self.width/2)-50,self.height/2-(btn_h/2)))
            win.blit(text_create , ((self.width/2)-50,(self.height/2-(btn_h/2))+btn_h*1.5))
            win.blit(text_id , ((self.width/2)-250,50))
            win.blit(text_name , ((self.width/2)-50,50))
            win.blit(text_surface, (input_rect.x+5, input_rect.y+5))

            if wrong_format_id:
                win.blit(text_wrong_id , ((self.width/2)-50,100))

            if succ == False:
                win.blit(text_connection_fail , ((self.width/2)-50,100))

            pygame.display.update()

    def draw_lobby(self, players, admin):
        pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Menu')
        win.fill((255,255,255))
        print(players)
        color_white = (255,255,255)
        color_light = (170,170,170)
        color_dark = (100,100,100)

        btn_w = 140
        btn_h = 40

        btn_play_pos_x = self.width-btn_w
        btn_play_pos_y = self.height-50
        btn_back_pos_x = btn_w
        btn_back_pos_y = self.height-50
        
        text_font = pygame.font.SysFont('Corbel',35)
        text_lobby = text_font.render('LOBBY', True, color_white)
        text_start = text_font.render('START', True, color_white)
        text_leave = text_font.render('LEAVE', True, color_white)
        text_lobby_id = text_font.render(f'lobby id: {self.client.lobby_id}', True, color_white)
        text_players_numbers = [text_font.render(f'Player {str(i)}: ', True, color_white) for i in range(len(players))]
        text_players = [text_font.render(p, True, color_white) for p in players]
        
        while True:
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    #TODO odpojení hráče z lobby
                    pygame.quit()
                    
                #ověření jestli se klikla myš
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    
                    #je kliknutí v pozici tlačítka?
                    if btn_play_pos_x-(btn_w/2) <= mouse[0] <= btn_play_pos_x+(btn_w/2) and btn_play_pos_y-(btn_h/2) <= mouse[1] <= btn_play_pos_y+(btn_h/2):
                        self.gameloop()

                    if btn_back_pos_x-(btn_w/2) <= mouse[0] <= btn_back_pos_x+(btn_w/2) and btn_back_pos_y-(btn_h/2) <= mouse[1] <= btn_back_pos_y+(btn_h/2):
                        self.client.send_cancel_lobby()

                if ev.type == pygame.KEYDOWN:
                    if ev.key == pygame.K_BACKSPACE:
                        user_text = user_text[:-1]
                    else:
                        if len(user_text) < 5:
                            user_text += ev.unicode

            win.fill((60,25,60))
            mouse = pygame.mouse.get_pos()

            if admin:
                if btn_play_pos_x-(btn_w/2) <= mouse[0] <= btn_play_pos_x+(btn_w/2) and btn_play_pos_y-(btn_h/2) <= mouse[1] <= btn_play_pos_y+(btn_h/2):
                    pygame.draw.rect(win,color_light,[(btn_play_pos_x)-(btn_w/2),btn_play_pos_y-(btn_h/2),btn_w,btn_h])
                else:
                    pygame.draw.rect(win,color_dark,[(btn_play_pos_x)-(btn_w/2),btn_play_pos_y-(btn_h/2),btn_w,btn_h])

            if btn_back_pos_x-(btn_w/2) <= mouse[0] <= btn_back_pos_x+(btn_w/2) and btn_back_pos_y-(btn_h/2) <= mouse[1] <= btn_back_pos_y+(btn_h/2):
                pygame.draw.rect(win,color_light,[(btn_back_pos_x)-(btn_w/2),btn_back_pos_y-(btn_h/2),btn_w,btn_h])
            else:
                pygame.draw.rect(win,color_dark,[(btn_back_pos_x)-(btn_w/2),btn_back_pos_y-(btn_h/2),btn_w,btn_h])

            for idx in range(len(players)):
                win.blit(text_players_numbers[idx] , ((self.width/2)-200,100+(50*idx)))
                win.blit(text_players[idx] , ((self.width/2)-50,100+(50*idx)))

            win.blit(text_lobby , ((self.width/2)-50,50))
            win.blit(text_lobby_id , ((self.width/2)+120,50))
            win.blit(text_leave , (btn_back_pos_x-(btn_w/2),btn_back_pos_y-(btn_h/2)))
            if admin:
                win.blit(text_start , (btn_play_pos_x-(btn_w/2),btn_play_pos_y-(btn_h/2)))

            pygame.display.update()

    def gameloop(self):
        """
        Hlavní cyklus celé herní části
        """
        global pallet

        self.initalize_game(self.cols, self.rows, False)
        pygame.display.update()

        color = (0,0,0) 
        thickness = 1

        color_white = (255,255,255)
        text_font = pygame.font.SysFont('Corbel',35)
        text_lobby = text_font.render('LOBBY', True, color)

        run = True
        while run:
            ev = pygame.event.get()

            for event in ev:
                if event.type == pygame.QUIT:
                    #TODO odpojení ze hry
                    #self.client.send_leave()
                    window = Tk()
                    window.withdraw()
                    
                    run = False

                if pygame.mouse.get_pressed()[0]: 
                    try:
                        pos = pygame.mouse.get_pos()
                        if pos[1] >= grid.heightt: 
                            if pos[0] >= pallet.startx and pos[0] <= pallet.startx + pallet.width and pos[1] >= pallet.starty and pos[1] <= pallet.starty + pallet.heightt:
                                self.clicked = pallet.clicked(pos)
                                color = self.clicked.getColor() 

                                pallet = colorPallet(win, 90, 90, 3, 3, True, 10, grid.heightt + 2)
                                pallet.drawGrid()

                                colorList = [(0,0,0), (255,255,255), (255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,168,0), (244, 66, 173), (65, 244, 226)]
                                pallet.setColor(colorList)
                                self.clicked.show(grid.screen, (255,0,0), 3, True)
                        
                        else:
                            name = pygame.display.get_caption()[0]
                            if name.find("*") < 1:
                                self.changeCaption(name + '*')

                            self.clicked = grid.clicked(pos)
                            self.clicked.click(grid.screen,color)
                            position = grid.get_position(pos)
                            #TODO odkomentovat
                            #self.client.send_pixel(pos[0],pos[1],color)
                            #self.client.receive_pixel('2;448,547,(0, 0, 0)')

                        pygame.display.update()
                    except AttributeError:
                        pass
            win.blit(text_lobby , ((self.width/2)-50,50))
