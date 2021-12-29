import pygame
from gui import Gui
from client import Client
import threading

if __name__ == '__main__':
    application = Gui()
    client = Client('Pepa', application)
    client.send_get_id()
    thread = threading.Thread(target=client.receive_from_server)
    thread.start()
    application.draw_menu()
    pygame.quit()
