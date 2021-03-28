from agent import Agent
from data import Data
import numpy as np
import pandas as pd
from pathos.multiprocessing import ProcessingPool as Pool
from pathos.multiprocessing import freeze_support
from tensorflow.keras.layers import LSTM, Dense, Dropout, Input
from tensorflow.keras.models import Sequential

class Sim:

    def __init__(self, model):

        self.agents = []
        self.data_points = []
        self.log = pd.DataFrame()

        for i in range(50):
            self.agents.append(Agent(self, i, np.random.uniform(-0.7, 0.7), np.random.uniform(-0.7, 0.7), np.random.uniform(0, 2*np.pi), model=model)) # TODO: Add the model to the agents and initiilize there posistions

        self.agents[0].pushto_mem(Data(0, 0, 0), 'pri')
        
        for i in [-0.4, 0, 0.4]:
            for j in [-0.4, 0, 0.4]:
                self.data_points.append((i,j))


    def run(self, runtime=4000, non_cor_chance = -0.003, cor_when = None, verbose = True):
        
        for iteration in range(runtime):

            if verbose: print(f"{iteration}/{runtime}")

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
    freeze_support()

    model = Sequential()
    model.add(Input(shape=(20,9)))
    model.add(LSTM(10, return_sequences=True))
    model.add(Dropout(0.1))
    model.add(LSTM(5, return_sequences=True))
    model.add(Dropout(0.1))
    model.add(Dense(3))
    model.add(Dense(3))

    tom = Sim(model)
    tom.run(verbose=True)

    # sims = [Sim(model), Sim(model), Sim(model), Sim(model), Sim(model), Sim(model)]

    # p = Pool()
    # p.map(Sim.run, sims)