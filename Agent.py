#Agent.py
from PerceptionSystem.ProtoVision import Ray, vision
import math
from Config import *

class Agent:
    def __init__(self):
        self.position = [400, 300]   # start somewhere in world
        self.head_angle = HEADANGLE        # radians
        self.vision = vision()

    def update(self, WorldEntityLists):

        # Sync vision with agent
        self.vision.origin = self.position
        self.vision.angle = self.head_angle #type: ignore

        seen, ViewBuffer = self.vision.scan(WorldEntityLists)

        print(f"Agent sees {len(seen)} objects. Entites: ", WorldEntityLists, end="\r")

        # Simple test: rotate head every tick
        self.head_angle +=  math.radians(ROTATIONSPEED)