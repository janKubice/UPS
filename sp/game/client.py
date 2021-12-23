import socket

import msg_codes as codes

class Client:
    def __init__(self, name) -> None:
        self.HOST = '127.0.0.1'  # The server's hostname or IP address
        self.PORT = 10000        # The port used by the server
        self.name = name
        self.id = -1
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.connect((self.HOST, self.PORT))

    def send_msg_to_server(self, code, msg):
        """Odešle zprávu na server

        Args:
            code (int): kód zprávy
            msg (string): dodatečné informace ke zprávě
        """
        self.socket.sendall(f'{self.id};{code};{msg}')

    def receive_from_server(self):
        """Poslouchá server a zjišťuje zda nebylo něco přijato
        """
        while(True):
            try:
                data = self.socket.recv(1024)
            except:
                print('něco špatně se serverem')
                #TODO

            if len(data) == 0:
                continue

            data = data.split(';')
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
                pass
            elif msg_code == codes.CREATE_GAME:
                pass
            elif msg_code == codes.CONNECT_TO_GAME:
                pass
            elif msg_code == codes.RECONNECT:
                pass
            elif msg_code == codes.LEAVE:
                pass
            elif msg_code == codes.GET_POINTS:
                pass
            elif msg_code == codes.GET_ID:
                self.receive_id(msg_text)

    def receive_games(self, response:str):
        """Vypíše běžící hry na obrazovku

        Args:
            response (str): text ze serveru obsahující hry ve tvaru p_hracu_1:max_hracu_1:id_mistnosti_1:stav_1,p_hracu_2:max_hracu_2:id_mistnosti_2:stav_2...
            Jednolivé hry jsou odděleny pomocí , a jednotlivé údaje v jedné hře pomocí :
        """
        if response == None or response == '' or response == codes.ERR:
            return

        games = response.split(',')

        for game in games:
            game_info = game.split(':')
            #TODO updatnout GUI a vypsat postupně všechny hry
        pass

    def receive_pixel(self, response:str):
        """Namaluje pixel na obrazovku

        Args:
            response (str): odpověď ze serveru
        """
        if response == None or response == '' or response == codes.ERR:
            return

        params = response.split(',')
        if len(params) is not 3:
            return

        #TODO aktualizovat hrací plochu

    def receive_item(self, response:str):
        """Ukáže jaké slovo má hráč malovat

        Args:
            response (str): odpověď ze serveru co má hráč malovat
        """
        if response == None or response == '' or response == codes.ERR:
            return
        
        #TODO ukázat slovo hráči

    def receive_quess_response(self, response:str):
        if response == None or response == '' or response == codes.ERR:
            return

        if response == 't':
            #TODO ukázat hráči že uhodl
            pass
        else:
            #TODO ukázat hráči, že neuhodl
            pass

    def receive_create_game_response(self, response:str):
        """Zjistí zda se povedla založit hra

        Args:
            # response (str): odpověď ze serveru ve formátu response_CODE,id_mistnosti
        """
        if response == None or response == '' or response == codes.ERR:
            return

        params = response.split(',')
        if len(params) is not 2:
            return

        if params[0] == codes.ERR:
            #TODO Nepovedlo se
            pass
        else:
            #TODO povedlo se a založit, ukázat zakladeli id a tak
            pass

    def receive_connect_response(self, response:str):
        """Připojí hráče do hry pokud může

        Args:
            response (str): odpověď se serveru, obsahuje zda se povedlo připojit, pokud ano obsahuje dále údaje o hře
        """
        if response == None or response == '' or response == codes.ERR:
            return
        
        params = response.split(',')

        #TODO Přizpůsobit GUI


    def receive_reconnect_response(self, response:str):
        """Připojí hráče do hry, přepne obrazovku a zobrazí hru v jajím stavu

        Args:
            response (str): zpráva ze serveru
        """
        if response == None or response == '' or response == codes.ERR:
            return

        #TODO vymyslet co je potřeba přijmout


    def receive_leave_response(self, response:str):
        if response == None or response == '':
            return

        code = int(response)
        if code == codes.OK:
            #TODO exit a tak
            pass
        else:
            #TODO zkusit znovu a zatím neodpojovat
            pass

    def receive_get_points(self, response:str):
        if response == None or response == '':
            return

        points = [int(numeric_string) for numeric_string in response.split(',')]

        if len(points) <= 1:
            return

        #TODO update GUI

    def receive_id(self, response:str):
        """Nastaví id hráče

        Args:
            response (str): přijatá zpráva ze serveru
        """
        if response == None or response == '':
            return
        
        id = int(response)
        if id <= 0:
            return

        self.id = id

    def send_get_games(self):
        """Zeptá se serveru na aktuální hry do kterých se lze připojit
        """
        pass

    def send_pixel(self, x: int , y:int, color:str):
        """Odešle na server požadavek na vyplnění pixelu

        Args:
            x (int): x pixelu
            y (int): y pixelu
            color (str): barva pixelu
        """
        pass

    def send_get_item_to_draw(self):
        """Dotáže se serveru na obrázek ke kreslení

        Returns:
            str: objekt který má hráč namalovat
        """
        pass

    def send_quess(self, quess: str):
        """Odešle na server tip toho co si hráč myslí, pokud je to správně vrací True jinak False

        Args:
            quess (str): odhad hráče
        """
        pass

    def send_create_new_game(self):
        """Odešle požadavek k vytvoření nové hry
        """
        pass

    def send_connect_to_game(self):
        pass

    def send_reconnect_to_game(self):
        pass

    def send_leave(self):
        pass

    def send_get_points(self):
        """Dotáže se serveru na počet bodu hráčů
        """
        pass

    def send_get_id(self):
        """Získá od serveru id hráče
        """
        pass