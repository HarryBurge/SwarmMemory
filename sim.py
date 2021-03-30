from agent import Agent
from data import Data
import numpy as np
import pandas as pd
from pathos.multiprocessing import ProcessingPool as Pool
from pathos.multiprocessing import freeze_support
from keras.layers import Dense, Dropout, Input
from keras.models import Sequential
import pygad.kerasga
import matplotlib.pyplot as plt
from joblib import Parallel, delayed
import pickle
import types
import tempfile
import keras.models
import tensorflow
from keras_pickle_wrapper import KerasPickleWrapper
import os
import agent
import sys
os.environ["CUDA_VISIBLE_DEVICES"]="-1" 
np.set_printoptions(threshold=sys.maxsize)


class Sim:

    def __init__(self, model = None, agents=50):

        self.agents = []
        self.log = [None]*10000

        for i in range(agents):
            self.agents.append(Agent(self, i, np.random.uniform(-0.7, 0.7), np.random.uniform(-0.7, 0.7), np.random.uniform(0, 2*np.pi)))

        
        for ind, (i, j) in enumerate([(i,j) for j in [-0.4, 0, 0.4] for i in [-0.4, 0, 0.4]]):
            argmin = np.argmin([agent.dist(k.x, k.y, i, j) for k in self.agents])
            self.agents[argmin].pushto_mem(Data(ind, i, j), 'pri')

        self.model = model


    def run(self, model=None, runtime=40, non_cor_chance = -0.003, cor_when = None, verbose = True):
        
        # for i in self.agents:
        #     i.ai = model
        for i in self.agents:
            i.ai = self.model


        for iteration in range(runtime):

            if verbose: print(f"{iteration}/{runtime}")

            for i in self.agents:
                i.step()

            if non_cor_chance != None and np.random.uniform(0, 1) < non_cor_chance and len(self.agents) != 0:
                del self.agents[-1]

            if cor_when != None and iteration == cor_when:
                survived_agents = []

                for i in self.agents:
                    if np.sqrt(np.power(i.x, 2) + np.power(i.y, 2)) <= 0.25:
                        survived_agents.append(i)

                self.agents = survived_agents

            # Do calcs for the logging shiz
            dupes = np.zeros(9)
            nagents = len(self.agents)
            stability = np.zeros(9)
            dists = np.zeros((9, len(self.agents)))
            ratio_total = np.ones((20,20))
            ratio_dupes = np.zeros((9,20,20))

            for ind, i in enumerate(self.agents):

                for j in np.arange(9):

                    tmp_x = int(np.round(i.x, 1)*10+10)
                    tmp_y = int(np.round(i.y, 1)*10+10)
                    ratio_total[tmp_x][tmp_y] = ratio_total[tmp_x][tmp_y]+1

                    if i.mem_has_id(j, 'pub'):
                        dupes[j] += 1

                        mem_item = i.get_data_mem(j, 'pub')
                        dists[j][ind] = agent.dist(i.x, i.y, mem_item.x, mem_item.y)

                        ratio_dupes[j][tmp_x][tmp_y] = ratio_dupes[j][tmp_x][tmp_y]+1

                        if iteration != 0:
                            stability[j] = np.abs(dupes[j]-dupes[j-1])

                    # print(ratio_dupes[j][tmp_x][tmp_y], tmp_x, tmp_y)

            self.log[iteration] = [nagents, dupes, dists, ratio_dupes/ratio_total, stability]
            # print(self.log[iteration])    


    def fitness(self):
        
        # Take into account nagents compared to dupes
        # Make sum of stability the smallest
        # As long as dists roughly in the correct range, mean under certain range but max past certain threshold
        pass



if __name__ == '__main__':
    freeze_support()

    model = Sequential()
    model.add(Input(shape=(9)))
    model.add(Dense(9))
    model.add(Dense(4))


    # # models = []
    # sims = []

    # for i in range(100):
    #     # models.append(KerasPickleWrapper(tensorflow.keras.models.clone_model(model)))
    #     temp = KerasPickleWrapper(tensorflow.keras.models.clone_model(model))
    #     sims.append(Sim(temp))
    
    # p = Pool(6)
    # p.map(Sim.run, sims)#,models)


    tom = Sim(KerasPickleWrapper(model))
    tom.run(verbose=True)