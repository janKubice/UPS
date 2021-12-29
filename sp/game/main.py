import pygame
from gui import Gui
from client import Client
import threading

if __name__ == '__main__':
    client = Client('Pepa')
    client.send_msg_to_server(1,"1")
    thread = threading.Thread(target=client.receive_from_server)
    thread.start()
    application = Gui(client)
    application.draw_menu()
    pygame.quit()
