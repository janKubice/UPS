import pygame
import client
import threading

if __name__ == '__main__':
    player = client.Client()
    #thread = threading.Thread(target=player.receive_from_server)
    #thread.start()
    pygame.quit()
