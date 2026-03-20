#GWThreads/workspace.py
class Workspace:
    def __init__(self) -> None:
        self.current_focus = None

    def process_task(self, PerceptionBuffer):
        if PerceptionBuffer == [] or not PerceptionBuffer:
            self.current_focus = None
            return None
        else:
            return self.perceiveVision(PerceptionBuffer)
        

    def perceiveVision(self, PerceptioBuffer):

        self.current_focus = min(
            PerceptioBuffer, 
            key=lambda x: x["distance"]
        )

        return self.current_focus