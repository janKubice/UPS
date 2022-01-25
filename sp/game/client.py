import socket
from time import sleep
from turtle import update
from urllib.parse import _NetlocResultMixinBytes
from pygame.draw import lines

from pygame.key import name

import msg_codes as codes
import gui
import threading
import ipaddress
from pygame import Color

class Client:
    def __init__(self) -> None:
        self.HOST = '127.0.0.1'  # The server's hostname or IP address
        self.PORT = 10000        # The port used by the server
        self.name = ''
        self.gui: gui.Gui = None
        self.id = -1
        self.lobby_id = -1
        self.points = "0"
        self.winner = ""

        self.run_receive = False
        self.run_ping = False

        self.thread_receive = None
        self.thread_ping = None

        self.thread_menu = None
        self.thread_connection = None
        self.thread_lobby = None
        self.thread_game = None
        self.thread_wait = None
        self.thread_final_screen = None

        self.run_menu = True
        self.run_menu_2 = True
        self.run_connection = False
        self.run_lobby = False
        self.run_game = False
        self.run_wait = False
        self.run_final_screen = False

        self.application = gui.Gui()
        self.application.set_client(self)
        
        self.application.draw_menu()
        #self.thread_menu = threading.Thread(target=self.application.draw_menu, args=())
        #self.thread_menu.start()

    def set_gui(self, gui:gui.Gui):
        self.gui = gui

    def connect(self, user_name, ip, port):
        self.HOST = ip
        self.PORT = int(port)

        try:
            self.run_receive = True
            self.run_ping = True
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.HOST, self.PORT))
            self.thread_receive = threading.Thread(target=self.receive_from_server, args=())
            self.thread_receive.start()
            self.send_get_id(user_name)

            self.thread_ping = threading.Thread(target=self.ping, args=())
            self.thread_ping.start()
            
        except:
            print('chyba, nepodarilo se pripojit. Server budto nebezi nebo jste zadali spatne udaje pro pripojeni')

    def ping(self):
        while(self.run_ping):
            if (self.send_msg_to_server(codes.PING) == False):
                print('Odpojeni od server, asi umrel')
                self.go_to_menu()
                self.end_threads()
            sleep(15)

    def end_threads(self):
        self.run_receive = False
        self.run_ping = False
        self.thread_connection.join()
        self.thread_ping.join()
        self.socket.close()

    def send_msg_to_server(self, code, msg = '-1', printing = True):
        """Odešle zprávu na server

        Args:
            code (int): kód zprávy
            msg (string): dodatečné informace ke zprávě
        """
        text = f'{self.id};{code};{msg}'
        if printing:
            print(f'Odesílám: {text}')
        try:
            self.socket.sendall(str.encode(text))
            return True
        except:
            return False

    def receive_from_server(self):
        """Poslouchá server a zjišťuje zda nebylo něco přijato
        """
        while(self.run_receive):
            try:
                data = self.socket.recv(1024)
                data = data.decode()
            except:
                print('Chyba pri prijimani dat')

            print(f'přijímám: {data}\n')

            if (len(data) == 0):
                print('Server se chova divne, uzaviram spojeni')
                self.go_to_menu()
                self.end_threads()
                break


            data = data.split(';')
            data = [s.rstrip('\x00') for s in data]
            print(f'splitnuta data: {data}')
            msg_code = int(data[0])
            msg_text = data[1]
            
            if msg_code == codes.GET_GAMES:
                pass
            elif msg_code == codes.SET_PIXEL:
                self.receive_pixel(msg_text)

            elif msg_code == codes.GET_ITEM:
                self.receive_item(msg_text)

            elif msg_code == codes.SEND_QUESS:
                self.receive_quess_response(msg_text)

            elif msg_code == codes.CREATE_GAME:
                self.receive_create_game_response(msg_text)

            elif msg_code == codes.CONNECT_TO_GAME:
                self.receive_connect_response(msg_text)

            elif msg_code == codes.RECONNECT:
                self.receive_reconnect_response(msg_text)

            elif msg_code == codes.LEAVE:
                self.receive_leave_response(msg_text)

            elif msg_code == codes.GET_POINTS:
                self.receive_get_points(msg_text)

            elif msg_code == codes.GET_ID:
                self.receive_id(msg_text)

            elif msg_code == codes.NEXT_ROUND:
                self.receive_next_round(msg_text)

            elif msg_code == codes.CANCEL_GAME:
                self.receive_cancel_game()

            elif msg_code == codes.ERR:
                self.show_error(msg_text)

            elif msg_code == codes.WAIT:
                self.receive_wait_for_next_round()

            elif msg_code == codes.FINAL_TABLE:
                self.receive_final_screen(msg_text)

    def receive_pixel(self, response: str):
        """Namaluje pixel na obrazovku

        Args:
            response (str): odpověď ze serveru ve formátu '448,547,0, 0, 0'
        """
        params = response.split(',')
        
        if len(params) != 5:
            return
        print(params)
        self.application.draw_pixel(int(params[0]), int(params[1]), Color(int(params[2]), 
                                    int(params[3]), int(params[4])))

    def receive_item(self, response: str):
        """Ukáže jaké slovo má hráč malovat

        Args:
            response (str): odpověď ze serveru co má hráč malovat
        """
        self.application.word = response

    def receive_quess_response(self, response: str):
        if response != codes.ERR:
            self.application.draw_quess_response('Správně')
            self.receive_get_points(response)
        else:
            self.application.draw_quess_response('Špatně')
            

    def receive_create_game_response(self, response: str):
        """Zjistí zda se povedla založit hra

        Args:
            # response (str): odpověď ze serveru ve formátu response_CODE,id_mistnosti
        """        
        if int(response) == codes.ERR:
            self.application.draw_connection(False)
        else:
            self.lobby_id = int(response)
            self.application.players = [self.name]
            self.thread_lobby = threading.Thread(target=self.application.draw_lobby, args=(True, response))
            self.thread_lobby.start()
            self.thread_connection.join()
            
    def receive_connect_response(self, response: str):
        """Připojí hráče do hry pokud může

        Args:
            response (str): odpověď se serveru, obsahuje zda se povedlo připojit, pokud ano obsahuje dále údaje o hře
        """
        params = response.split(',')

        self.application.players = params[1:]
        if self.run_lobby == False:
            self.thread_lobby = threading.Thread(target=self.application.draw_lobby, args=(int(params[0]), self.lobby_id))
            self.thread_lobby.start()


    def receive_reconnect_response(self, response: str):
        """Připojí hráče do hry, přepne obrazovku a zobrazí hru v jajím stavu

        Args:
            response (str): zpráva ze serveru
        """
        if response == None or response == '' or response == codes.ERR:
            return

        # TODO vymyslet co je potřeba přijmout

    def receive_leave_response(self, response: str):
        if response == None or response == '':
            return

        code = int(response)
        if code == codes.OK:
            # TODO exit a tak
            pass
        else:
            # TODO zkusit znovu a zatím neodpojovat
            pass

    def receive_get_points(self, response: str):
        points = int(response)
        self.points = points

    def receive_next_round(self, msg):
        msg = msg.split(',')
        on_turn = int(msg[0])


        self.thread_wait = threading.Thread(target=self.application.draw_wait_for_game, args=())
        self.thread_wait.start()
        
        if self.thread_lobby.is_alive():
            self.thread_lobby.join()

        if self.thread_game != None and self.thread_game.is_alive():
            self.thread_game.join()

        self.thread_game = threading.Thread(target=self.application.gameloop, args=(on_turn, msg[1], self.points))
        self.thread_game.start()
        self.thread_wait.join()

    def receive_cancel_game(self):
        if self.run_lobby:
            self.thread_connection = threading.Thread(target=self.application.draw_connection, args=())
            self.thread_connection.start()
            self.thread_lobby.join()
        elif self.run_game:
            self.thread_connection = threading.Thread(target=self.application.draw_connection, args=())
            self.thread_connection.start()
            self.thread_lobby.join()


    def receive_id(self, response: str):
        """Nastaví id hráče a změní obrazovku

        Args:
            response (str): přijatá zpráva ze serveru
        """

        id = int(response)
        self.id = id
        
        self.thread_connection = threading.Thread(target=self.application.draw_connection, args=())
        self.thread_connection.start()
        
        if self.thread_final_screen != None and self.thread_final_screen.is_alive():
            self.run_final_screen = False
            self.thread_final_screen.join()

        if self.thread_menu != None and self.thread_menu.is_alive():
            self.run_menu = False
            self.thread_menu.join()
        

    def send_get_games(self):
        """Zeptá se serveru na aktuální hry do kterých se lze připojit
        """
        pass

    def send_pixel(self, x: int, y: int, color:set):
        """Odešle na server požadavek na vyplnění pixelu

        Args:
            x (int): x pixelu
            y (int): y pixelu
            color (str): barva pixelu
        """
        color = str(color)
        color = color[1:-2]
        color = color.replace(" ", "")
        color_rgb = color.split(',')

        for idx, value in enumerate(color_rgb):
            if value == '':
                color_rgb[idx] = '0'
        
        msg = f'{x},{y},{color_rgb[0]},{color_rgb[1]},{color_rgb[2]}'
        self.send_msg_to_server(codes.SET_PIXEL, msg, False)

    def send_get_item_to_draw(self):
        """Dotáže se serveru na obrázek ke kreslení

        Returns:
            str: objekt který má hráč namalovat
        """
        self.send_msg_to_server(codes.GET_ITEM)

    def send_quess(self, quess: str):
        """Odešle na server tip toho co si hráč myslí, pokud je to správně vrací True jinak False

        Args:
            quess (str): odhad hráče
        """
        self.send_msg_to_server(codes.SEND_QUESS, quess)

    def send_create_new_game(self):
        """Odešle požadavek k vytvoření nové hry
        """
        self.send_msg_to_server(codes.CREATE_GAME)

    def send_connect_to_game(self, id:str) -> bool:
        """Žádost o připojení
        """
        if id.isnumeric() and len(id) >= 1:
            self.send_msg_to_server(codes.CONNECT_TO_GAME, int(id))
            return True
        
        return False

    def send_reconnect_to_game(self):
        """Žádost o znovu připojení
        """
        self.send_msg_to_server(codes.RECONNECT)

    def send_leave(self):
        """Informování o opuštění hry
        """
        self.send_msg_to_server(codes.LEAVE)

    def send_get_points(self):
        """Dotáže se serveru na počet bodu hráčů
        """
        self.send_msg_to_server(codes.GET_POINTS)

    def send_get_id(self, player_name:str):
        """Získá od serveru id hráče
        """
        if (len(player_name) > 0):
            self.name = player_name
            self.send_msg_to_server(codes.GET_ID, msg=player_name)

    def send_cancel_lobby(self):
        self.send_msg_to_server(codes.CANCEL_GAME)

    def send_start_game(self):
        self.send_msg_to_server(codes.START_GAME)

    def show_error(self, msg):
        #TODO zobrazit na obrazovce treba na 10 sekund
        print(msg)

    def receive_wait_for_next_round(self):
        self.thread_wait = threading.Thread(target=self.application.draw_wait_for_game, args=())
        self.thread_wait.start()
        if self.thread_game != None and self.thread_game.is_alive():
            self.thread_game.join()

    def receive_final_screen(self, msg):
        self.winner = msg
        self.thread_final_screen = threading.Thread(target=self.application.draw_final_screen, args=())
        self.thread_final_screen.start()

        if self.run_game == True:
            self.thread_game.join()
        elif self.run_wait == True:
            self.thread_wait.join()

    def go_to_menu(self):
        self.thread_menu = threading.Thread(target=self.application.draw_menu, args=())
        self.thread_menu.start()

        if self.thread_game != None and self.thread_game.is_alive():
            self.run_game = False
            self.thread_game.join()

        if self.thread_final_screen != None and self.thread_final_screen.is_alive():
            self.run_final_screen = False
            self.thread_final_screen.join()

        if self.thread_lobby != None and self.thread_lobby.is_alive():
            self.run_lobby = False
            self.thread_lobby.join()

        if self.thread_connection != None and self.thread_connection.is_alive():
            self.run_connection = False
            self.thread_connection.join()
    