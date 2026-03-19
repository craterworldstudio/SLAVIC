#PerceptionSystem/ProtoVision.py
from Config import *
import math

class Ray:

    def __init__(self, stepSize=1.0) -> None:
        self.casted = True
        self.Dist = 0
        self.Origin = [0, 0]
        self.Position = [0, 0]
        self.Direction = [0.0, 0.0]
        self.StepSize = stepSize
        self.HitEntity = None
        self.Hit = False

    def cast(self, angle, origin):
        self.angle = angle
        self.casted = True
        self.Hit = False
        self.HitEntity = None

        self.Origin = origin.copy()
        self.Position = origin.copy()

        self.Direction[0] = math.cos(angle)
        self.Direction[1] = math.sin(angle)

    def calculateIntersection(self, p1, p2):
        rx, ry = self.Direction # corrdinates on ray

        sx = p2[0] - p1[0] #vector for the line segment p2x - p1x
        sy = p2[1] - p1[1] #vector for the line segment p2y - p1y

        intersector = rx * sy - ry * sx

        if abs(intersector) < 1e-6:
            return None # parallel
        
        dx = p1[0] - self.Origin[0]
        dy = p1[1] - self.Origin[1]

        t = (dx * sy - dy * sx) / intersector
        u = (dx * ry - dy * rx) / intersector

        if t >= 0 and 0 <= u <= 1:
            return t
        return None


    def advanceOld(self, WorldEntityList):
        if self.casted == False:
            return 1, None, []
        
        self.Position[0] += self.Direction[0] * self.StepSize #type: ignore
        self.Position[1] += self.Direction[1] * self.StepSize #type: ignore 

        for Entity in WorldEntityList:
            dx = self.Position[0] - Entity.Position[0]
            dy = self.Position[1] - Entity.Position[1]

            if math.sqrt(dx*dx + dy*dy) < Entity.sizenum:
                self.Hit = True
                self.HitEntity = Entity
                #print(f"Ray hit at distance {self.Dist}")
                break
        
        dx = self.Position[0] - self.Origin[0]
        dy = self.Position[1] - self.Origin[1]

        self.Dist = math.sqrt(dx*dx + dy*dy)

        #self.Dist = round(math.sqrt(self.Position[0]**2 + self.Position[1]**2))

        if self.Dist >= MAXDISTANCE and self.Hit == False :
            self.Exhausted = True
            return 1, None, [self.Position, self.Dist]

        elif self.Hit and self.HitEntity != None:
            self.Exhausted = True
            return 0, self.HitEntity, [self.Position, self.Dist]
        
        return 2, None, []


    def advance(self, WorldEntityList):
        if self.casted == False:
            return 1, None, []
        
        closest_t = float('inf')
        closest_entity = None

        for Entity in WorldEntityList:
            #print("Colliders:", Entity.Colliders)
            for line in Entity.Colliders:
                t = self.calculateIntersection(line[0], line[1])
                #print("Checking entity:", Entity.shape)
                
                #print("t:", t)

                if t is not None and t < closest_t and t <= MAXDISTANCE:
                    closest_t = t
                    closest_entity = Entity
            
        if closest_entity != None:
            self.Dist = closest_t

            hit_x = self.Origin[0] + self.Direction[0] * self.Dist
            hit_y = self.Origin[1] + self.Direction[1] * self.Dist

            return 0, closest_entity, [[hit_x, hit_y], self.Dist]

        limitX = self.Origin[0] + self.Direction[0] * MAXDISTANCE
        limitY = self.Origin[1] + self.Direction[1] * MAXDISTANCE
        self.Dist = MAXDISTANCE
        return 1, None, [[limitX, limitY], MAXDISTANCE]
            
        

    #def terminate(self):
    #    del self



class vision:

    def __init__(self) -> None:
        self.rays = [Ray(RAYSTEPSIZE) for _ in range(RAYDENSITY)]
        self.angle = 0
        self.origin = [0, 0]
        self.EntitiesInVision = set()
        self.CurrentFrame = []

    def scan(self, WorldEntityLists):
        self.CurrentFrame.clear()
        self.EntitiesInVision.clear()
        #for i in range(0, RAYDENSITY): self.rays.append(Ray(RAYSTEPSIZE))
        
        startangle = self.angle - math.radians(FOV) / 2
        angle = startangle
        anglestepsize = math.radians(FOV) / RAYDENSITY
        
        
        for ray in self.rays:
            ray.Dist = 0
            ray.cast(angle, self.origin)
            angle += anglestepsize

            
            StsCode,  Entity, ViewBuffer = ray.advance(WorldEntityLists) 

            if StsCode == 0: 
                self.EntitiesInVision.add(Entity)

                if ViewBuffer != []:
                    self.CurrentFrame.append({
                            "point": ViewBuffer[0],
                            "distance": ViewBuffer[1],
                            "color": Entity.color,  #type: ignore
                            "angle": ray.angle,
                            "shape": Entity.shape, #type: ignore
                            "size": Entity.sizenum #type: ignore
                        })

            elif StsCode == 1:
                if ViewBuffer:
                    self.CurrentFrame.append({
                        "point": ViewBuffer[0],
                        "distance": ViewBuffer[1],
                        "color": None,
                        "angle": ray.angle,
                        "shape": None,
                        "size": None
                    })

        #self.rays.clear()
        return  self.EntitiesInVision, self.CurrentFrame


        