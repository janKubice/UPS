import socket

class Client:
    def __init__(self, name) -> None:
        self.HOST = '127.0.0.1'  # The server's hostname or IP address
        self.PORT = 10000        # The port used by the server
        self.name = name
        self.id = self.get_id()


    def send_test(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((self.HOST, self.PORT))
            s.sendall(b'Hello, world')
            data = s.recv(1024)

        print('Received', repr(data))

    def get_games(self):
        """Vrátí aktuální hry ke kterým se lze připojit ve formátu pole odděleného ; :
            id, name, p_max, p_actual, id, name....
        """
        pass

    def send_pixel(self, x: int , y:int) -> bool:
        """Odešle na server požadavek na vyplnění pixelu, pokud projde vrátí True.

        Args:
            x (int): x pixelu
            y (int): y pixelu

            Returns:
                bool: jestli lze pixel vyplnit
        """
        pass

    def get_item_to_draw(self) -> str:
        """Dotáže se serveru na obrázek ke kreslení

        Returns:
            str: objekt který má hráč namalovat
        """
        pass

    def send_quess(self, quess: str) -> bool:
        """Odešle na server tip toho co si hráč myslí, pokud je to správně vrací True jinak False

        Args:
            quess (str): odhad hráče

        Returns:
            bool: zda je tip dobře
        """
        pass

    def send_create_new_game(self) -> bool:
        pass

    def get_points(self) -> int:
        """Dotáže se serveru na počet bodu hráče

        Returns:
            int: počet bodů
        """
        pass

    def get_id(self) -> int:
        """Získá od serveru id hráče

        Returns:
            int: id hráče
        """
        pass