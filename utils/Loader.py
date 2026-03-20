#Loader.py
import json

def loadEventsData():
    with open("Environment/WorldEvents.json", "r") as File:
        data = json.load(File)
    return data