import socket

import msg_codes as codes
import gui

class Client:
    def __init__(self, name: str) -> None:
        self.HOST = '127.0.0.1'  # The server's hostname or IP address
        self.PORT = 10000        # The port used by the server
        self.name = name
        self.gui = None
        self.id = -1

        self.application = gui.Gui()
        self.application.set_client(self)
        self.application.draw_menu()

    def set_gui(self, gui:gui.Gui):
        self.gui = gui

    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.HOST, self.PORT))
            return True
        except:
            return False

    def send_msg_to_server(self, code, msg = '-1'):
        """Odešle zprávu na server

        Args:
            code (int): kód zprávy
            msg (string): dodatečné informace ke zprávě
        """
        text = f'{self.id};{code};{msg}'
        print(f'Odesílám: {text}')
        self.socket.sendall(str.encode(text))

    def receive_from_server(self):
        """Poslouchá server a zjišťuje zda nebylo něco přijato
        """
        while(True):
            data = ''
            try:
                data = self.socket.recv(1024)
            except:
                print('něco špatně se serverem')
                # TODO

            data = data.decode('UTF-8')
            print(f'přijímám: {data}\n')

            if len(data) == 0:
                continue

            data = data.split(';')
            if len(data) < 3:
                continue

            msg_code = int(data[0])
            msg_text = data[1]

            if msg_code <= 0 or len(msg_text) <= 0:
                continue

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


    def receive_pixel(self, response: str):
        """Namaluje pixel na obrazovku

        Args:
            response (str): odpověď ze serveru
        """
        if response == None or response == '' or response == codes.ERR:
            return

        params = response.split(',')
        if len(params) != 3:
            return

        self.gui.draw_pixel(params[0], params[1], params[2])

    def receive_item(self, response: str):
        """Ukáže jaké slovo má hráč malovat

        Args:
            response (str): odpověď ze serveru co má hráč malovat
        """
        if response == None or response == '' or response == codes.ERR:
            return

        self.gui.draw_item_to_draw(response)

    def receive_quess_response(self, response: str):
        if response == None or response == '' or response == codes.ERR:
            return

        if response == '1':
            self.gui.draw_quess_response('Správně')
        else:
            self.gui.draw_quess_response('Špatně')

    def receive_create_game_response(self, response: str):
        """Zjistí zda se povedla založit hra

        Args:
            # response (str): odpověď ze serveru ve formátu response_CODE,id_mistnosti
        """
        if response == None or response == '' or response == codes.ERR:
            return

        params = response.split(',')
        if len(params) != 2:
            return

        if params[0] == codes.ERR:
            # TODO Nepovedlo se
            pass
        else:
            # TODO povedlo se a založit, ukázat zakladeli id a tak
            pass

    def receive_connect_response(self, response: str):
        """Připojí hráče do hry pokud může

        Args:
            response (str): odpověď se serveru, obsahuje zda se povedlo připojit, pokud ano obsahuje dále údaje o hře
        """
        if response == None or response == '' or response == codes.ERR:
            return

        params = response.split(',')

        # TODO Přizpůsobit GUI

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
        if response == None or response == '':
            return

        points = [int(numeric_string)
                  for numeric_string in response.split(',')]

        if len(points) <= 1:
            return

        # TODO update GUI

    def receive_id(self, response: str):
        """Nastaví id hráče a změní obrazovku

        Args:
            response (str): přijatá zpráva ze serveru
        """
        if response == None or response == '':
            return

        id = int(response)
        if id <= 0:
            return

        self.id = id
        self.application.draw_connection()

    def send_get_games(self):
        """Zeptá se serveru na aktuální hry do kterých se lze připojit
        """
        pass

    def send_pixel(self, x: int, y: int, color: str):
        """Odešle na server požadavek na vyplnění pixelu

        Args:
            x (int): x pixelu
            y (int): y pixelu
            color (str): barva pixelu
        """
        msg = f'{x}|{y}|{color}'
        self.send_msg_to_server(codes.SET_PIXEL, msg)

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

    def send_connect_to_game(self):
        """Žádost o připojení
        """
        self.send_msg_to_server(codes.CONNECT_TO_GAME)

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

    def send_get_id(self):
        """Získá od serveru id hráče
        """
        self.send_msg_to_server(codes.GET_ID)

    