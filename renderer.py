#renderer.py
import pygame

colortorgb = {
    "red": (255, 0, 0),
    "green": (0, 255, 0),
    "blue": (0, 0, 255),
    "white": (255, 255, 255),
    "black": (0, 0, 0),
    "yellow": (255, 255, 0),
    "pink": (255, 105, 180)  # nicer than pure (255, 0, 255)
}

def draw_objects(screen, objects):
    for obj in objects:

        if obj.shape == "Circle":
            pygame.draw.circle(
                screen,
                colortorgb[obj.color],
                (int(obj.Position[0]), int(obj.Position[1])),
                int(obj.sizenum)
            )
        
        if obj.shape == "Block":
            pygame.draw.rect(
                screen,
                colortorgb[obj.color],
                (
                    int(obj.Position[0] - obj.sizenum/2),
                    int(obj.Position[1] - obj.sizenum/2),
                    int(obj.sizenum),
                    int(obj.sizenum)
                )
            )

        if obj.shape == "Sphere":
            pygame.draw.circle(
                screen,
                colortorgb[obj.color],
                (int(obj.Position[0]), int(obj.Position[1])),
                int(obj.sizenum)
            )

        if obj.shape == "Plane":
            pygame.draw.rect(
                screen,
                colortorgb[obj.color],
                
                (int(obj.Position[0]), int(obj.Position[1]), int(obj.sizenum), int(obj.sizenum * 0.8))
            )

        if obj.shape == "Pyramid":
            x, y = int(obj.Position[0]), int(obj.Position[1])
            size = int(obj.sizenum)

            points = [
                (x, y - size),          # Top point
                (x - size, y + size),   # Bottom left
                (x + size, y + size)    # Bottom right
            ]


            pygame.draw.polygon(
                screen,
                colortorgb[obj.color],
                points
            )

def draw_agent(screen, agent):
    pygame.draw.circle(
        screen,
        (255, 0, 0),
        (int(agent.position[0]), int(agent.position[1])),
        6
    )

import math

def draw_rays(screen, agent):
    for frame in agent.vision.CurrentFrame:

        start = agent.position
        end = frame["point"]

        pygame.draw.line(
            screen,
            (255, 255, 0),
            start,
            (int(end[0]), int(end[1])),
            1
        )

def render(screen, objects, agent):
    screen.fill((0, 0, 0))

    draw_objects(screen, objects)
    draw_agent(screen, agent)
    draw_rays(screen, agent)

    pygame.display.flip()