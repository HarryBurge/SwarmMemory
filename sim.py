from agent import Agent
import numpy as np
import pandas as pd
from pathos.multiprocessing import ProcessingPool as Pool
from pathos.multiprocessing import freeze_support

class Sim:

    def __init__(self, model):

        self.agents = []
        self.data_points = []
        self.log = pd.DataFrame()

        for i in range(50):
            self.agents.append(Agent(self, i, np.random.uniform(-0.7, 0.7), np.random.uniform(-0.7, 0.7), np.random.uniform(0, 2*np.pi))) # TODO: Add the model to the agents and initiilize there posistions

        for i in [-0.4, 0, 0.4]:
            for j in [-0.4, 0, 0.4]:
                self.data_points.append((i,j))

    def run(self, runtime=4000, non_cor_chance = -0.003, cor_when = 3000, verbose = False):
        
        for iteration in range(runtime):

            if verbose: print(f"{iteration}/{runtime}")

            # Step agents in threads
            # p = Pool()
            # p.map(Agent.step, self.agents)

            for i in self.agents:
                i.step()


            if non_cor_chance != None and np.random.uniform(0, 1) < non_cor_chance and len(self.agents) != 0:
                del self.agents[-1]

            if cor_when != None and iteration == cor_when:
                survived_agents = []

                for i in self.agents:
                    if np.sqrt(pow(i.x, 2) + pow(i.y, 2)) <= 0.25:
                        survived_agents.append(i)

                self.agents = survived_agents
                        


    def results(self):
        pass


if __name__ == '__main__':  
    tom = Sim('t')
    freeze_support()
    print(len(tom.agents))
    tom.run(verbose=True)
    print(len(tom.agents))
