from tokenize import Triple
import pygame
from tkinter import *
import grid
from grid import colorPallet, textObject
from grid import pixelArt
from grid import menu
from grid import grid
import sys
import client
import threading

class Gui:
    """
    Třída pracující s GUI celé aplikace
    Není to žádný zázrak a je to docela ošklivé GUI ale funguje a to stačí!
    """
    def __init__(self) -> None:
        sys.setrecursionlimit(1000000)
        pygame.init()

        #self.paintBrush = pygame.image.load("sp/game/art/Paintbrush.png")
        #self.paintBrush = pygame.image.load("art/Paintbrush.png")

        #Defaultní hodnoty
        self.rows = 50
        self.cols = 50
        self.width = 600
        self.height = 600

        self.checked = []
        self.client:client.Client = None
        self.clicked = None
        self.points = "0"

        self.players = []
        self.points = 0
        self.word = ''

    def set_client(self, client):
        self.client = client

    def close_application(self):
        self.client.run_menu = False
        self.client.run_connection = False
        self.client.run_lobby = False
        self.client.run_game = False

        self.client.run_ping = False
        self.client.run_receive = False

        self.client.thread_ping.join()
        self.client.thread_receive.join()

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

    def draw_quess_response(self, text):
        print(text)
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
        
        #pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height) + 100))
        pygame.display.set_caption('Untitled')
        win.fill((255,255,255))

        grid = pixelArt(win, int(self.width), int(self.height), cols, rows, showGrid)
        grid.drawGrid()

        pallet = colorPallet(win, 90, 90, 3, 3, True, 10, grid.height + 2)
        pallet.drawGrid()

        colorList = [(0,0,0), (255,255,255), (255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,168,0), (244, 66, 173), (65, 244, 226)]
        pallet.setColor(colorList)

        pygame.display.update()

    def draw_menu(self):
        """
        První obrazovka kterou uživatel vidí
        """
        self.client.run_menu = True
        self.client.run_connection = False
        self.client.run_lobby = False
        self.client.run_game = False
        self.client.run_wait = False
        self.client.run_final_screen = False

        #pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Menu')
        win.fill((255,255,255))

        base_font = pygame.font.Font(None, 32)
        color_white = (255,255,255)
        color_light = (170,170,170)
        color_dark = (100,100,100)

        btn_w = 140
        btn_h = 40
        
        user_text_name = 'your name'
        user_text_ip = '127.0.0.1'
        user_text_port = '10000'
        text_font = pygame.font.SysFont('Corbel',35)
        text = text_font.render('Connect' , True , color_white)

        color_active = pygame.Color('lightskyblue3')
        color_passive = pygame.Color('chartreuse4')

        color_name = color_passive
        color_ip = color_passive
        color_port = color_passive

        input_rect_name = pygame.Rect(self.width/2 - 50, 120, 200, 32)
        input_rect_ip = pygame.Rect(self.width/2 - 50, 170, 200, 32)
        input_rect_port = pygame.Rect(self.width/2 - 50,220, 200, 32)

        
        active_name = False
        active_ip = False
        active_port = False
        while True:
            if self.client.run_menu == False:
                continue
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    self.client.send_leave()
                    self.close_application()
                    pygame.quit()
                    sys.exit()
                    
                #ověření jestli se klikla myš
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    
                    #je kliknutí v pozici tlačítka?
                    if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                        self.client.connect(user_text_name, user_text_ip, user_text_port)

                    if input_rect_name.collidepoint(ev.pos):
                        active_name = True
                    else:
                        active_name = False

                    if input_rect_ip.collidepoint(ev.pos):
                        active_ip = True
                    else:
                        active_ip = False

                    if input_rect_port.collidepoint(ev.pos):
                        active_port = True
                    else:
                        active_port = False

                if ev.type == pygame.KEYDOWN:
                    if active_name:
                        if ev.key == pygame.K_BACKSPACE:
                            user_text_name = user_text_name[:-1]
                        else:
                            if len(user_text_name) < 7:
                                user_text_name += ev.unicode
                    elif active_ip:
                        if ev.key == pygame.K_BACKSPACE:
                            user_text_ip = user_text_ip[:-1]
                        else:
                            if len(user_text_ip) < 15:
                                user_text_ip += ev.unicode
                    elif active_port:
                        if ev.key == pygame.K_BACKSPACE:
                            user_text_port = user_text_port[:-1]
                        else:
                            if len(user_text_port) < 5:
                                user_text_port += ev.unicode
                        
            win.fill((60,25,60))

            if active_name:
                color_name = color_active
            else:
                color_name = color_passive

            if active_ip:
                color_ip = color_active
            else:
                color_ip = color_passive

            if active_port:
                color_port = color_active
            else:
                color_port = color_passive


            mouse = pygame.mouse.get_pos()

            pygame.draw.rect(win, color_name, input_rect_name)
            text_surface_name = base_font.render(user_text_name, True, (255, 255, 255))  
            input_rect_name.w = max(100, text_surface_name.get_width()+10)

            pygame.draw.rect(win, color_ip, input_rect_ip)
            text_surface_ip = base_font.render(user_text_ip, True, (255, 255, 255))  
            input_rect_ip.w = max(100, text_surface_ip.get_width()+10)

            pygame.draw.rect(win, color_port, input_rect_port)
            text_surface_port = base_font.render(user_text_port, True, (255, 255, 255))  
            input_rect_port.w = max(100, text_surface_port.get_width()+10)
            
            #hover přes tlačítko
            if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                pygame.draw.rect(win,color_light,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2),btn_w,btn_h])
            else:
                pygame.draw.rect(win,color_dark,[(self.width/2)-(btn_w/2),self.height/2-(btn_h/2),btn_w,btn_h])
            

            win.blit(text , ((self.width/2)-50,self.height/2-(btn_h/2) + 17/2))
            win.blit(text_surface_name, (input_rect_name.x+5, input_rect_name.y+5))
            win.blit(text_surface_ip, (input_rect_ip.x+5, input_rect_ip.y+5))
            win.blit(text_surface_port, (input_rect_port.x+5, input_rect_port.y+5))
            pygame.display.update()

    def draw_connection(self, succ:bool = True):
        """
        Obrazovka s textovým inputem pro zadání ID místnosti
        """
        print('Vykresluju connection')
        self.client.run_menu = False
        self.client.run_connection = True
        self.client.run_lobby = False
        self.client.run_game = False
        self.client.run_wait = False
        self.client.run_final_screen = False

        self.run_menu = False
        #pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Menu')
        win.fill((255,255,255))
        pygame.display.update()

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
            if self.client.run_connection == False:
                break
            for ev in pygame.event.get():            
                if ev.type == pygame.QUIT:
                    self.client.send_leave()
                    self.close_application()
                    pygame.quit()
                    sys.exit()
                    
                #ověření jestli se klikla myš
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    succ = True
                    #je kliknutí v pozici tlačítka?
                    if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2) <= mouse[1] <= self.height/2+(btn_h/2):
                        if self.client.send_connect_to_game(user_text):
                            wrong_format_id = False
                        else:
                            wrong_format_id = True
                            print('ID místnosti není ve správném formátu')

                    if self.width/2-(btn_w/2) <= mouse[0] <= self.width/2+(btn_w/2) and self.height/2-(btn_h/2)+btn_h*1.5 <= mouse[1] <= self.height/2+(btn_h/2)+btn_h*1.5:
                        self.client.send_create_new_game()

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
            

            win.blit(text_join , ((self.width/2) - 25,self.height/2-(btn_h/2) + 17/2))
            win.blit(text_create , ((self.width/2) - 40,(self.height/2-(btn_h/2))+btn_h*1.5 + 17/2))
            win.blit(text_id , ((self.width/2)-250,50))
            win.blit(text_name , ((self.width/2)-50,50))
            win.blit(text_surface, (input_rect.x+5, input_rect.y+5))

            if wrong_format_id:
                win.blit(text_wrong_id , ((self.width/2)-50,100))

            if succ == False:
                win.blit(text_connection_fail , ((self.width/2)-50,100))

            pygame.display.update()


    def draw_lobby(self, admin:bool, game_id:int = 0):
        print('Vykresluju lobby')
        self.client.run_menu = False
        self.client.run_connection = False
        self.client.run_lobby = True
        self.client.run_game = False
        self.client.run_wait = False
        self.client.run_final_screen = False

        #pygame.display.set_icon(self.paintBrush)   
        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Menu')
        win.fill((255,255,255))
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

        if admin:
            print(f'game id {game_id}')
            text_lobby_id = text_font.render(f'lobby id: {game_id}', True, color_white)

        
        while True:
            if self.client.run_lobby == False:
                break
            
            text_players_numbers = [text_font.render(f'Player {str(i)}: ', True, color_white) for i in range(len(self.players))]
            text_players = [text_font.render(p, True, color_white) for p in self.players]
            
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    self.client.send_leave()
                    self.close_application()
                    pygame.quit()
                    sys.exit()
                    
                #ověření jestli se klikla myš
                if ev.type == pygame.MOUSEBUTTONDOWN:
                    
                    #je kliknutí v pozici tlačítka?
                    if btn_play_pos_x-(btn_w/2) <= mouse[0] <= btn_play_pos_x+(btn_w/2) and btn_play_pos_y-(btn_h/2) <= mouse[1] <= btn_play_pos_y+(btn_h/2):
                        self.client.send_start_game()

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

            for idx in range(len(self.players)):
                win.blit(text_players_numbers[idx] , ((self.width/2)-200,100+(50*idx)))
                win.blit(text_players[idx] , ((self.width/2)-50,100+(50*idx)))

            win.blit(text_lobby , ((self.width/2)-50,50))
            win.blit(text_leave , (btn_back_pos_x-(btn_w/2),btn_back_pos_y-(btn_h/2)))
            if admin:
                win.blit(text_lobby_id , ((self.width/2)+120,50))
                win.blit(text_start , (btn_play_pos_x-(btn_w/2),btn_play_pos_y-(btn_h/2)))

            pygame.display.update()

    def draw_pixel(self, x, y, color):
        self.clicked = grid.clicked((x,y))
        self.clicked.click(grid.screen,color)
        pygame.display.update()

    def draw_wait_for_game(self):
        """
        Cekaci obrazovka nez zacne dalsi kolo
        """
        self.client.run_menu = False
        self.client.run_connection = False
        self.client.run_lobby = False
        self.client.run_game = False
        self.client.run_wait = True
        self.client.run_final_screen = False

        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Waiting')
        win.fill((255,255,255))

        color_dark = (100,100,100)
        
        text_font = pygame.font.SysFont('Corbel',35)
        text = text_font.render('Wait for next round. THANK YOU' , True , color_dark)

        while True:
            if self.client.run_wait == False:
                break
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    self.client.send_leave()
                    self.close_application()
                    pygame.quit()
                    sys.exit()
                
            win.fill((60,25,60))
            win.blit(text , (self.width/2 - text.get_width()/2,self.height/2))
            pygame.display.update()

    
    def draw_final_screen(self, winner):
        self.client.run_menu = False
        self.client.run_connection = False
        self.client.run_lobby = False
        self.client.run_game = False
        self.client.run_wait = False
        self.client.run_final_screen = True

        win = pygame.display.set_mode((int(self.width), int(self.height)))
        pygame.display.set_caption('Waiting')
        win.fill((255,255,255))

        color_dark = (100,100,100)
        
        text_font = pygame.font.SysFont('Corbel',35)
        text = text_font.render(f'Winner: {winner}!' , True , color_dark)

        while True:
            if self.client.run_final_screen == False:
                break
            for ev in pygame.event.get():
            
                if ev.type == pygame.QUIT:
                    self.client.send_leave()
                    self.close_application()
                    pygame.quit()
                    sys.exit()
                
            win.fill((60,25,60))
            win.blit(text , (self.width/2,self.height/2))
            pygame.display.update()


    def gameloop(self, on_turn:bool, word:str, points: str):
        """
        Hlavní cyklus celé herní části
        """
        self.client.run_menu = False
        self.client.run_connection = False
        self.client.run_lobby = False
        self.client.run_game = True
        self.client.run_wait = False
        self.client.run_final_screen = False

        global pallet

        self.initalize_game(self.cols, self.rows, False)
        pygame.display.update()

        color = (0,0,0) 
        color_rec = (0,0,0)
        color_light = (170,170,170)
        color_dark = (100,100,100)
        thickness = 1
        btn_w = 140
        btn_h = 40

        base_font = pygame.font.Font(None, 32)
        color_white = (255,255,255)
        color_black = (0,0,0)
        text_font = pygame.font.SysFont('Corbel',35)
        text_word = text_font.render(word, True, color_rec)
        text_points = text_font.render(f'Points: {points}' , True , color_black)


        input_rect = pygame.Rect(300, 500, 140, 32)
        color_active = pygame.Color('lightskyblue3')
        color_passive = pygame.Color('chartreuse4')
        
        user_text = ''
        active = False
        run = True
        while run:
            pygame.display.update()
            if self.client.run_game == False:
                break
            ev = pygame.event.get()

            for event in ev:
                if event.type == pygame.QUIT:
                    self.client.send_leave()
                    self.close_application()
                    window = Tk()
                    window.withdraw()
                    
                    run = False
                
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_RETURN:
                        self.client.send_quess(user_text)

                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_BACKSPACE:
                        user_text = user_text[:-1]
                    else:
                        user_text += event.unicode

                if event.type == pygame.MOUSEBUTTONDOWN:
                    if input_rect.collidepoint(event.pos):
                        active = True
                    else:
                        active = False


                if pygame.mouse.get_pressed()[0]: 
                    try:
                        pos = pygame.mouse.get_pos()
                        if pos[1] >= grid.height: 
                            if pos[0] >= pallet.startx and pos[0] <= pallet.startx + pallet.width and pos[1] >= pallet.starty and pos[1] <= pallet.starty + pallet.height:
                                self.clicked = pallet.clicked(pos)
                                color = self.clicked.getColor() 

                                if on_turn == False:
                                    pallet = colorPallet(win, 90, 90, 3, 3, True, 10, grid.height + 2)
                                    pallet.drawGrid()

                                    colorList = [(0,0,0), (255,255,255), (255,0,0), (0,255,0), (0,0,255), (255,255,0), (255,168,0), (244, 66, 173), (65, 244, 226)]
                                    pallet.setColor(colorList)
                                    self.clicked.show(grid.screen, (255,0,0), 3, True)
                        
                        else:
                            name = pygame.display.get_caption()[0]
                            if name.find("*") < 1:
                                self.changeCaption(name + '*')

                            if on_turn:
                                self.client.send_pixel(pos[0],pos[1],color)
                            

                        pygame.display.update()
                    except AttributeError:
                        pass
            if active:
                color_rec = color_active
            else:
                color_rec = color_passive


            win.blit(text_points , ((self.width/2)+100,50))

            if on_turn == False:
                pygame.draw.rect(win, color_rec, input_rect)
                text_surface = base_font.render(user_text, True, (255, 255, 255))  
                input_rect.w = max(100, text_surface.get_width()+10)
                win.blit(text_surface , (300,500))
            else:
                win.blit(text_word , ((self.width/2)-50,50))
            
