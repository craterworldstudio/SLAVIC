#World.py
from Environment import Object
from Agent import Agent
import pygame
from Config import *
from renderer import *
from utils import Loader
import time

WorldEntityList = []
tick = 0
tick_duration = 1 / WTICKSPEED

data = Loader.loadEventsData()
Cr1 = Object.Object(data)
WorldEntityList.append(Cr1)
Cr2 = Object.Object(data)
WorldEntityList.append(Cr2)

agent = Agent()

pygame.init()
screen = pygame.display.set_mode((800, 600))
clock = pygame.time.Clock()


while True:
    start = time.time()

    # --- WORLD UPDATE ---
    for obj in WorldEntityList:
        obj.chooseAct(WorldEntityList, tick)
        obj.update_position()


     # --- AGENT UPDATE ---
    agent.update(WorldEntityList)
    
    render(screen, WorldEntityList, agent)

    if len(WorldEntityList) == 0: 
        RCr1 = Object.Object(data)
        WorldEntityList.append(RCr1)

    clock.tick(60)
    tick+=1

    elapsed = time.time() - start
    sleep_time = tick_duration - elapsed

    if sleep_time > 0:
        time.sleep(sleep_time)