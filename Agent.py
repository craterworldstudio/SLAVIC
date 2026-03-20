#Agent.py
from GWThreads import workspace as ws

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
        
        cWS = ws.Workspace()

        processedBuffer = cWS.process_task(ViewBuffer)
        minDist = float('inf')
        if processedBuffer['color'] is not None: minDist = processedBuffer['distance'] #type: ignore

        print(f"Agent sees {len(seen)} objects. \t Entites: {len(WorldEntityLists)} \t Minimum Distance: {minDist}", end="\r")

        # Simple test: rotate head every tick
        self.head_angle +=  math.radians(ROTATIONSPEED)