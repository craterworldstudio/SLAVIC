#Agent.py
from GWThreads import workspace as ws

from PerceptionSystem.ProtoVision import Ray, vision
from MemoryUnit.memory import VSManager, LiveMemoryView

import math
from Config import *


class Agent:
    def __init__(self):
        self.position = [400, 300]   # start somewhere in world
        self.head_angle = HEADANGLE        # radians
        self.vision = vision()
        self.cWS = ws.Workspace()
        self.vsm = VSManager(dims = DIMS)
        self.memory_view = LiveMemoryView(self.vsm)
        self.frame_count = 0

    def update(self, WorldEntityLists):
        self.frame_count += 1

        if len(self.vsm.Memory) > MAX_MEMORY:
            self.vsm.Memory.pop(0)

        # Sync vision with agent
        self.vision.origin = self.position
        self.vision.angle = self.head_angle #type: ignore

        seen, ViewBuffer = self.vision.scan(WorldEntityLists)
    
        processedBuffer = self.cWS.process_task(ViewBuffer)

        minDist = float('inf')
        score = 0.0
        if processedBuffer and processedBuffer['color'] is not None:  #type: ignore

            minDist = processedBuffer['distance'] #type: ignore 
            if not self.vsm.Memory:
                self.vsm.store(processedBuffer)
            else:   
                mem, score = self.vsm.recall(processedBuffer)
                if score < 0.85: self.vsm.store(processedBuffer)
            #print(f"MemScore: {round(score, 3)}", end=" | ")

        print(f"Agent sees {len(seen)} objects. Entites: {len(WorldEntityLists)} MinDist: {minDist} MemScore: {round(score,3)}", end="\r")

        #if self.frame_count % 10 == 0:
        self.memory_view.update()

        # Simple test: rotate head every tick
        self.head_angle +=  math.radians(ROTATIONSPEED)