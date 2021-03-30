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


class Sim:

    def __init__(self, model = None, agents=50):

        self.agents = []
        self.data_points = []
        self.log = pd.DataFrame()

        for i in range(agents):
            self.agents.append(Agent(self, i, np.random.uniform(-0.7, 0.7), np.random.uniform(-0.7, 0.7), np.random.uniform(0, 2*np.pi)))

        self.agents[0].pushto_mem(Data(0, 0, 0), 'pri')
        
        for i in [-0.4, 0, 0.4]:
            for j in [-0.4, 0, 0.4]:
                self.data_points.append((i,j))

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
                        


    def results(self):
        pass


if __name__ == '__main__':
    freeze_support()

    model = Sequential()
    model.add(Input(shape=(9)))
    model.add(Dense(9))
    model.add(Dense(4))


    # # models = []
    # sims = []

    # for i in range(4):
    #     # models.append(KerasPickleWrapper(tensorflow.keras.models.clone_model(model)))
    #     temp = KerasPickleWrapper(tensorflow.keras.models.clone_model(model))
    #     sims.append(Sim(temp))
    
    # p = Pool()
    # p.map(Sim.run, sims)#,models)


    # tom = Sim(model)
    # tom.run(verbose=True)