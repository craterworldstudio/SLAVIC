#Environment/Object.py
import math
import random
from Config import *

class Object:
    def __init__(self, Data):
        self.Position = [(random.randrange(WORLDLIMITS[0][0], WORLDLIMITS[0][1])), (random.randrange(WORLDLIMITS[1][0], WORLDLIMITS[1][1]))] #World Positon
        self.Dist = 0   #Dist from Agent
        self.Direction = [1.0, 0.0] #World Direction
        self.Speed = 0
        self.Data = Data  #WorldEvents.json
        self.lastAction = "Spawn"
        self.ObjectUpdateTick = 10;
        self.stepSize = 0;                   #How fast an object moves.
        self.Colliders = []
        
        self.move_timer = 0
        self.idle_timer = 0

        self.CreateAtrributes()
        

    def CreateAtrributes(self):
        self.shape = self.Data["Objects"][random.randint(0, len(self.Data["Objects"])-1)]
        self.color = self.Data["Properties"]["Color"][random.randint(0, len(self.Data["Properties"]["Color"])-1)]
        self.size  = list(self.Data["Properties"]["Size"].keys())[random.randint(0, len(list(self.Data["Properties"]["Size"].keys()))-1)]
        self.sizenum = self.Data["Properties"]["Size"][self.size]
        self.state = self.Data["Properties"]["State"][random.randint(0, len(self.Data["Properties"]["State"])-1)]
        self.ObjectUpdateTick = random.randrange(5, 20)
        self.stepSize = 10 * self.Data["Properties"]["Size"][self.size]

        self.redrawColliderPoints()

    def redrawColliderPoints(self):
        """
        - +
        + +
        + -
        - -
        """
        self.Colliders.clear()
        x, y = self.Position
        s = self.sizenum

        if self.shape == "Block":
                
            p1 = [self.Position[0] - (self.sizenum/2), self.Position[1] + (self.sizenum/2)]
            p2 = [self.Position[0] + (self.sizenum/2), self.Position[1] + (self.sizenum/2)]
            p3 = [self.Position[0] + (self.sizenum/2), self.Position[1] - (self.sizenum/2)]
            p4 = [self.Position[0] - (self.sizenum/2), self.Position[1] - (self.sizenum/2)]

            self.Colliders.extend([(p1, p2), (p2, p3), (p3, p4), (p4, p1)])

        if self.shape == "Pyramid":
                
            p1 = [self.Position[0],                  self.Position[1] - (self.sizenum)]
            p2 = [self.Position[0] - (self.sizenum), self.Position[1] + (self.sizenum)]
            p3 = [self.Position[0] + (self.sizenum), self.Position[1] + (self.sizenum)]

            self.Colliders.extend([(p1, p2), (p2, p3), (p3, p1)])

        elif self.shape in ["Circle", "Sphere"]:
            segments = 12  # increase for smoother circle
            angle_step = (2 * math.pi) / segments

            points = []
            for i in range(segments):
                angle = i * angle_step
                px = x + math.cos(angle) * s
                py = y + math.sin(angle) * s
                points.append([px, py])

            # connect points in loop
            for i in range(len(points)):
                p1 = points[i]
                p2 = points[(i + 1) % len(points)]
                self.Colliders.append((p1, p2))

        elif self.shape == "Plane":
            w = s
            h = s * 0.8  # match renderer

            p1 = [x,     y]
            p2 = [x + w, y]
            p3 = [x + w, y + h]
            p4 = [x,     y + h]

            self.Colliders.extend([
                (p1, p2),
                (p2, p3),
                (p3, p4),
                (p4, p1)
            ])
        

    def chooseAct(self, WorldEntityList, tick) -> str:
        entity_count = len(WorldEntityList)
         # --- Population logic ---
        if entity_count < LOW_ENTITY_THRESHOLD:
            action = "Duplicate"

        elif entity_count > HIGH_ENTITY_THRESHOLD:
            action = random.choice(["Stop", "Maintain", "Destroy"])

        else:
            action = self.Data["Phenomenon"][random.randint(0, len(self.Data["Phenomenon"])-1)]

            while action == "Spawn": 
                action = "Maintain"
                #action = self.Data["Phenomenon"][random.randint(0, len(self.Data["Phenomenon"])-1)]

        self.lastAction = action
        self.Act(WorldEntityList, tick)
        #self.printdetails()
        return action
    
    def printdetails(self):
        print(f"""
            Object:
                Postition: {self.Position}
                Distance: {self.Dist}
                Direction: {self.Direction}
                Speed: {self.Speed}
                Action: {self.lastAction}
                Tick Rate: {self.ObjectUpdateTick}
                Shape: {self.shape}
                Color: {self.color}
                Size: {self.size}
                State: {self.state}
                Step Size: {self.stepSize}
            """)

    def update_position(self):
        self.Position[0] += self.Direction[0] * self.Speed if WORLDLIMITS[0][0] <= (self.Position[0] + (self.Direction[0] * self.Speed)) <= WORLDLIMITS[0][1] else 0 # type: ignore
        self.Position[1] += self.Direction[1] * self.Speed if WORLDLIMITS[1][0] <= (self.Position[1] + (self.Direction[1] * self.Speed)) <= WORLDLIMITS[1][1] else 0 # type: ignore
        self.redrawColliderPoints()

    def Act(self, WorldEntityList, tick):
        act = self.lastAction

        if self.Speed > 0:
            self.move_timer += 1
            self.idle_timer = 0
        else:
            self.idle_timer += 1
            self.move_timer = 0

        if tick % self.ObjectUpdateTick != 0:
            return
        
        if self.Speed == 0 and self.idle_timer > random.randint(30, 80):
            act = "Move"

        if act == "Destroy":
            WorldEntityList.remove(self)
            del self

        elif act == "Duplicate":
            if len(WorldEntityList) < HIGH_ENTITY_THRESHOLD:
                newOrg = Object(self.Data)
                newOrg.Position = [
                    self.Position[0] + random.randint(-3, 3), 
                    self.Position[1] + random.randint(-3, 3)]
                WorldEntityList.append(newOrg)

        elif act == "Maintain":
            pass

        elif act == "Stop":
            self.Speed = 0
        
        elif act == "Grow":
            sizelist = list(self.Data["Properties"]["Size"].keys())
            idx = sizelist.index(self.size)

            if idx < len(sizelist) - 1:
                self.size = sizelist[idx + 1]
                self.sizenum = self.Data["Properties"]["Size"][self.size]
            else:
                self.size = sizelist[-1]
                self.sizenum = self.Data["Properties"]["Size"][self.size]
                
            self.stepSize = 10 * self.Data["Properties"]["Size"][self.size]

            self.redrawColliderPoints()
            #nxtGrowthIdx = (sizelist.index(self.size) + 1) if not (sizelist.index(self.size) < len(sizelist)) else -1
            #self.size = list(self.Data["Properties"]["Size"].keys())[nxtGrowthIdx]

        elif act == "Shrink":
            sizelist = list(self.Data["Properties"]["Size"].keys())
            idx = sizelist.index(self.size)

            if idx > 0:
                self.size = sizelist[idx - 1]
                self.sizenum = self.Data["Properties"]["Size"][self.size]
            else:
                self.size = sizelist[0]
                self.sizenum = self.Data["Properties"]["Size"][self.size]

            self.stepSize = 10 * self.Data["Properties"]["Size"][self.size]

            self.redrawColliderPoints()
            #prevGrowthIdx = (sizelist.index(self.size) - 1) if not (sizelist.index(self.size) == 0) else 0
            #self.size = list(self.Data["Properties"]["Size"].keys())[prevGrowthIdx]

        elif act == "Move":
            if self.move_timer > random.randint(20, 60):
                self.Speed = 0
                return
            
            while True:
                dx = random.randint(-1,1)
                dy = random.randint(-1,1)
            
                if dx != 0 or dy != 0:
                    break

            length = math.sqrt(dx*dx + dy*dy)

            if length != 0:
                self.Direction[0] = dx / length
                self.Direction[1] = dy / length

            #opx, opy = self.Position[0], self.Position[1]

            self.Speed = self.stepSize
            
            #npx = int(self.Direction[0] * self.Speed)
            #npy = int(self.Direction[1] * self.Speed)

            #self.Speed = math.modf(math.sqrt((npx - opx) ** 2 + (npy - opy) ** 2))

        
            

        
        