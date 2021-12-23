import pygame
from gui import Gui

if __name__ == '__main__':
    #client = Client('Pepa')
    #thread = threading.Thread(target=client.receive_from_server)
    #thread.start()
    application = Gui()
    application.draw_menu()
    pygame.quit()
