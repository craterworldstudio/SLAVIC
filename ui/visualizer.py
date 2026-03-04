# ui/visualizer.py
import math
import time

import pygame

from core.interaction import clamp

def draw_mini_graph(screen, x, y, width, height, history, color, title):
    if len(history) < 2: return
    
    # Draw frame
    pygame.draw.rect(screen, (40, 40, 50), (x, y, width, height), 1)
    
    # Scale points and draw lines
    points = []
    for i, val in enumerate(history):
        px = x + (i * (width / len(history)))
        py = (y + height) - (val * height)
        points.append((px, py))
    
    if len(points) > 1:
        pygame.draw.lines(screen, color, False, points, 2)

def run_visualizer(state_manager):
    pygame.init()
    screen = pygame.display.set_mode((800, 600))
    clock = pygame.time.Clock()

    while True:
        state = state_manager.load_state()
        screen.fill((20, 20, 30)) # Dark void

        # 👁️ THE "MANIFESTATION" FLAG
        # The agent's size and color drift based on its internal state.
        energy = state.get("energy", 0.5)
        tension = state.get("internal_tension", 0.1)
        
        # Color: High tension = Redder, High Social = Bluer
        color = (
            int(clamp(tension * 255)), 
            int(clamp(energy * 255)), 
            int(clamp(state['world']['social_presence'] * 255))
        )
        
        t = time.time()
        # 💓 THE "VITALITY" FLAG
        # Pulse speed is driven by tension. High tension = frantic pulse.
        pulse_speed = 2 + (state.get("internal_tension", 0) * 10)
        pulse = math.sin(t * pulse_speed) * 10

        # Base radius + energy growth + the pulse
        radius = int(20 + (energy * 30) + pulse)

        pygame.draw.circle(screen, color, (400, 300), radius)
        
        # ⚡ THE "DIVINE INTERVENTION" FLAG
        # Allows you to click to 'feed' the agent resources or talk to it.
        for event in pygame.event.get():
            if event.type == pygame.MOUSEBUTTONDOWN:
                state['world']['resources'] += 1.0
                state['emotions']['confidence'] += 0.1
                state_manager.save_state(state)

        pygame.display.flip()
        clock.tick(30)