import random, numpy as np

class VSManager:

    def __init__(self, dims) -> None:
        self.Cache = {}
        self.Memory = []
        self.dims = dims

    def newVector(self, empty = False):
        newVec = np.random.normal(0, 1, self.dims) if empty == False else np.zeros(self.dims) + 1e-10
        return newVec / np.linalg.norm(newVec)
    
    def bind(self, a, b):
        return a * b
    
    def bundle(self, listA):
        vBund = np.sum(listA, axis=0)
        norm = np.linalg.norm(vBund)
        return vBund / norm if norm != 0 else vBund
    
    def store(self, BufferFrame):
        MemVec = self.encode(BufferFrame)
        self.Memory.append(MemVec)
    
    def get(self, key):
        if key not in self.Cache: 
            self.Cache[key] = self.newVector()

        return self.Cache[key].copy()
    
    def cosine_similarity(self, a, b):
        prod = np.dot(a, b)

        magA = np.linalg.norm(a)
        magB =  np.linalg.norm(b)

        if magA == 0 or magB == 0:
            return 0.0

        return prod / (magA * magB)

    def encode(self, BufferFrame):
        if BufferFrame['color']:
            colorV = self.get(BufferFrame['color'])
            sizeV = self.get(BufferFrame['size'])
        else:
            colorV = self.newVector(True)
            sizeV = self.newVector(True)

        #attributeV = self.bind(colorV, sizeV)
        
        Distance = "near" if BufferFrame['distance'] < 50.0 else "far"
        distV = self.get(Distance)

        COLOR = self.get("ROLE_COLOR")
        SIZE  = self.get("ROLE_SIZE")
        DIST  = self.get("ROLE_DISTANCE")

        return self.bundle([
            self.bind(COLOR, colorV),
            self.bind(SIZE , sizeV ),
            self.bind(DIST , distV )
        ])
    
    def decode(self, MemVec):
        COLOR = self.get("ROLE_COLOR")
        SIZE  = self.get("ROLE_SIZE")
        DIST  = self.get("ROLE_DISTANCE")

        queryC = self.bind(MemVec, COLOR)
        queryS = self.bind(MemVec, SIZE)
        queryD = self.bind(MemVec, DIST)

        ObjectAttribute = [None, 0.0, 0.0]
        recMatchStrengthList = []
        recMatchStrength = -1.0

        for key in ["red", "green", "blue", "white", "black", "yellow", "pink"]:
            kCVec = self.get(key)
            CCMatchStrength = self.cosine_similarity(kCVec, queryC)

            if recMatchStrength < CCMatchStrength:
                ObjectAttribute[0] = key
                recMatchStrength = CCMatchStrength
        recMatchStrengthList.append(recMatchStrength)
        recMatchStrength = -1.0

        for key in ["tiny", "small", "medium", "big", "large"]:
            kCVec = self.get(key)
            CCMatchStrength = self.cosine_similarity(kCVec, queryS)

            if recMatchStrength < CCMatchStrength:
                ObjectAttribute[1] = key
                recMatchStrength = CCMatchStrength

        recMatchStrengthList.append(recMatchStrength)
        recMatchStrength = -1.0

        for key in ["near", "far"]:
            kCVec = self.get(key)
            CCMatchStrength = self.cosine_similarity(kCVec, queryD)

            if recMatchStrength < CCMatchStrength:
                ObjectAttribute[2] = key
                recMatchStrength = CCMatchStrength

        recMatchStrengthList.append(recMatchStrength)


        return ObjectAttribute, recMatchStrengthList


    
    def recall(self, BufferFrame):
        if not self.Memory:
            return None, 0.0

        CBufVec = self.encode(BufferFrame)

        recMatch = None
        recMatchStrength = -1.0
        #scores_list = []

        for PMemVec in self.Memory:

            CMemMatchStrength = self.cosine_similarity(CBufVec, PMemVec)
            #scores_list.append(CMemMatchStrength)

            if recMatchStrength < CMemMatchStrength:
                recMatchStrength = CMemMatchStrength
                recMatch = PMemVec

        #scores_list.sort()

        return recMatch, recMatchStrength
    


import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

class LiveMemoryView:
    def __init__(self, vs_manager):
        self.vs = vs_manager
        self.pca = PCA(n_components=2)

        plt.ion()  # interactive mode
        self.fig, self.ax = plt.subplots()

    def update(self):
        if len(self.vs.Memory) < 2:
            return

        data = np.array(self.vs.Memory)

        reduced = self.pca.fit_transform(data)

        self.ax.clear()
        self.ax.scatter(reduced[:, 0], reduced[:, 1])

        self.ax.set_title("Live Memory Space")
        plt.draw()
        plt.pause(0.001)