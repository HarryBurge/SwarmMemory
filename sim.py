from agent import Agent
from data import Data
import numpy as np
import pandas as pd
from pathos.multiprocessing import ProcessingPool as Pool
from pathos.multiprocessing import freeze_support
from keras.layers import Dense, Dropout, Input, LSTM
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
from scipy.ndimage.filters import gaussian_filter1d
from textwrap import wrap
import copy
from datetime import datetime
os.environ["CUDA_VISIBLE_DEVICES"]="-1" 
# np.set_printoptions(threshold=sys.maxsize)


class Sim:

    def __init__(self, model = None, agents=50):

        self.agents = []
        self.log = [None]*10000
        # self.agent_logs = pd.DataFrame() # For debug

        for i in range(agents):
            self.agents.append(Agent(self, i, np.random.uniform(-0.7, 0.7), np.random.uniform(-0.7, 0.7), np.random.uniform(0, 2*np.pi)))

        
        for ind, (i, j) in enumerate([(i,j) for j in [-0.4, 0, 0.4] for i in [-0.4, 0, 0.4]]):
            argmin = np.argmin([agent.dist(k.x, k.y, i, j) for k in self.agents])
            self.agents[argmin].pushto_mem(Data(ind, i, j), 'pri')

        self.model = model


    def run(self, model=None, runtime=5000, non_cor_chance = 0.006, cor_when = None, verbose = False):
        
        # for i in self.agents:
        #     i.ai = model
        # for i in self.agents:
        #     i.ai = self.model


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
            ratio_total = np.ones((21,21))
            ratio_dupes = np.zeros((9,21,21))
            mem_spread = np.zeros(len(self.agents))

            for ind, i in enumerate(self.agents):

                mem_spread[ind] = i.pub_mem_lim - len(i.pub_mem)

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

            self.log[iteration] = np.array([nagents, dupes, dists, ratio_dupes/ratio_total, stability, mem_spread])

        return self


    def fitness(self):
        
        # Take into account nagents compared to dupes
        # Make sum of stability the smallest
        # As long as dists roughly in the correct range, mean under certain range but max past certain threshold
        se_of_dupes = 0
        sum_of_stab = 0
        distmax_min = 0
        num_of_it = 0
        memspread = 0

        for iteration in np.arange(len(self.log)):
            if type(self.log[iteration]) != type(None):
                num_of_it += 1
                se_of_dupes = se_of_dupes - np.power(np.mean(self.log[iteration][1]) - self.log[iteration][0], 2)
                sum_of_stab = sum_of_stab - np.mean(self.log[iteration][4])
                distmax_min = distmax_min + np.mean(np.power(np.max(self.log[iteration][2]) - np.min(self.log[iteration][2]), 2))
                memspread = memspread - np.std(self.log[iteration][5])

        return (se_of_dupes + sum_of_stab*100 + distmax_min*100 + memspread*100)/num_of_it



def plot_sims(logs):
    
    # Nagents
    nagents = np.zeros((len(logs), 10000))
    dupes = np.zeros((len(logs), 10000, 9))
    ratios = np.zeros((9, 21, 21))
    dists = np.zeros((len(logs), 10000, 9, 50))
    stab = np.zeros((len(logs), 10000, 9))
    spread = np.zeros((len(logs), 10000, 50))

    for ind, run in enumerate(logs):
        for i in np.arange(10000):
            if type(run[i]) != type(None):
                nagents[ind, i] = nagents[ind, i] + run[i][0]
                dupes[ind, i] = dupes[ind, i] + run[i][1]

                temp = np.zeros((9,50))
                temp[:run[i][2].shape[0],:run[i][2].shape[1]] = run[i][2]
                dists[ind, i] = dists[ind, i] + temp

                ratios = ratios + run[i][3]
                stab[ind, i] = stab[ind, i] + run[i][4]
                spread[ind, i] = spread[ind, i] + run[i][5]

            else:
                break


    # Plotting init
    fig3 = plt.figure(constrained_layout=False, figsize=(15, 8))
    fig3.subplots_adjust(left=0.052, bottom=0.057, right=0.967, top=0.976, wspace=0, hspace=0)
    # gs = fig3.add_gridspec(4, 4+len(ratio_mean)//3) #Change to 6 with large data
    gs = fig3.add_gridspec(30, 4+3)

    f3_ax1 = fig3.add_subplot(gs[0:6, 0:4])
    f3_ax2 = fig3.add_subplot(gs[6:12, 0:4])
    f3_ax3 = fig3.add_subplot(gs[12:18, 0:4])
    f3_ax4 = fig3.add_subplot(gs[18:24, 0:4])
    f3_ax5 = fig3.add_subplot(gs[24:30, 0:4])

    fig_dists = []
    for i in range(3):
        for j in range(3):
            fig_dists.append(fig3.add_subplot(gs[j*10:j*10+10, 4+i]))

    iterations = np.arange(10000)

    # Plotting 3 graphs
    # Dupes
    line = f3_ax1.fill_between( iterations, gaussian_filter1d(np.mean(np.mean(dupes, axis=0), axis=1) + np.mean(np.std(dupes, axis=0),axis=1), sigma=50), gaussian_filter1d(np.mean(np.mean(dupes, axis=0),axis=1) - np.mean(np.std(dupes, axis=0),axis=1), sigma=50),alpha=0.5)
    line.set_color('green')

    for i in np.arange(9):
        line, = f3_ax1.plot( iterations, gaussian_filter1d(np.mean(dupes, axis=0).T[i], sigma=50) )

    # Agents
    line = f3_ax2.fill_between( iterations, gaussian_filter1d(np.mean(nagents, axis=0)+np.std(nagents, axis=0), sigma=50), gaussian_filter1d(np.mean(nagents, axis=0)-np.std(nagents, axis=0), sigma=50),alpha=0.5)
    line.set_color('green')

    line, = f3_ax2.plot( iterations, np.mean(nagents,axis=0) )
    line.set_color('purple')

    # Dists
    line = f3_ax3.fill_between( iterations, gaussian_filter1d(np.mean(np.max(np.max(dists, axis=3), axis=0), axis=1), sigma=50), gaussian_filter1d(np.mean(np.min(np.min(dists, axis=3), axis=0),axis=1), sigma=50), alpha=0.5)
    line.set_color('orange')

    line = f3_ax3.fill_between( iterations, gaussian_filter1d(np.mean(np.mean(np.mean(dists, axis=3), axis=0),axis=1)+np.mean(np.mean(np.std(dists, axis=3), axis=0),axis=1), sigma=50), gaussian_filter1d(np.mean(np.mean(np.mean(dists, axis=3), axis=0),axis=1)-np.mean(np.mean(np.std(dists, axis=3), axis=0),axis=1), sigma=50), alpha=0.5 )
    line.set_color('green')

    for i in np.arange(9):
        line, = f3_ax3.plot( iterations, gaussian_filter1d(np.mean(np.mean(dists, axis=3), axis=0).T[i], sigma=50) )

    # Stability
    line = f3_ax4.fill_between( iterations, gaussian_filter1d((np.mean(np.mean(stab, axis=0),axis=1)+np.std(np.mean(stab, axis=0),axis=1))/5, sigma=50), gaussian_filter1d((np.mean(np.mean(stab, axis=0),axis=1)-np.std(np.mean(stab, axis=0),axis=1))/5, sigma=50), alpha=0.5)
    line.set_color('green')

    for i in np.arange(9):
        f3_ax4.plot(iterations, gaussian_filter1d(np.mean(stab, axis=0).T[i]/5, sigma=50))


    # # Ratios
    X, Y = np.meshgrid(np.arange(-1, 1.01, 0.1), np.arange(-1, 1.01, 0.1))

    for i in range(9):
        ratios[i] = np.nan_to_num(ratios[i], 0)
        fig_dists[i].contourf(X, Y, ratios[i])

    # Data areas
    for ind, (i, j) in enumerate([(i,j) for j in [-0.4, 0, 0.4] for i in [-0.4, 0, 0.4]]):
        fig_dists[ind].plot(j, i, 'ro')

    # # Spread
    line = f3_ax5.fill_between( iterations, gaussian_filter1d(10-np.max(np.max(spread, axis=0),axis=1), sigma=50), gaussian_filter1d(10-np.min(np.min(spread, axis=0),axis=1), sigma=50), alpha=0.5 )
    line.set_color('orange')

    line = f3_ax5.fill_between( iterations, gaussian_filter1d(10-(np.mean(np.mean(spread, axis=0),axis=1)+np.std(np.mean(spread, axis=0),axis=1)), sigma=50), gaussian_filter1d(10-(np.mean(np.mean(spread, axis=0),axis=1)-np.std(np.mean(spread, axis=0),axis=1)), sigma=50), alpha=0.5 )
    line.set_color('green')

    f3_ax5.plot(iterations, gaussian_filter1d(10-np.mean(np.mean(spread, axis=0),axis=1), sigma=50))


    

    ### Formating
    for i in fig_dists:
        i.set_ylim(top=1, bottom=-1)
        i.set_xlim(left=-1, right=1)
        i.set_yticklabels([])
        i.set_xticklabels([])
        i.set_xticks([])
        i.set_yticks([])

        #1 settings
    f3_ax1.set_ylabel("\n".join(wrap('Duplicated Data', 20)))
    f3_ax1.set_ylim(bottom=0)
    f3_ax1.set_xticklabels([])
    f3_ax1.set_xticks([])

    #2 settings
    f3_ax2.set_ylabel("\n".join(wrap('Number of Agents', 20)))
    f3_ax2.set_ylim(top=52, bottom=0)
    f3_ax2.set_xticklabels([])
    f3_ax2.set_xticks([])

    #3 settings
    f3_ax3.set_ylabel("\n".join(wrap('Data Distance', 20)))
    f3_ax3.set_xticklabels([])
    f3_ax3.set_xticks([])

    #4 Settings
    f3_ax4.set_ylabel("\n".join(wrap("Instability", 20)))
    f3_ax4.set_xticklabels([])
    f3_ax4.set_xticks([])

    # 5 Setttings
    f3_ax5.set_xlabel('Iterations')
    f3_ax5.set_ylabel("\n".join(wrap("Pub-Mem Spread", 20)))

    plt.show()


def ga(pop_size, parrellel_size, iterations, bots_to_mate):
    model = Sequential()
    model.add(Input(shape=(5, 10)))
    # model.add(LSTM(7, return_sequences=False))
    # model.add(Dense(5))
    model.add(Dense(7))
    model.add(Dense(4))


    # Inital population
    population = []

    for i in range(pop_size):
        population.append(Sim(KerasPickleWrapper(tensorflow.keras.models.clone_model(model))))


    for iters in range(iterations):
        # Fitness of models
        p = Pool(parrellel_size)
        out = p.map(Sim.run, population)

        pop_fitness = [out[i].fitness() for i in range(pop_size)]
        scaled_fitness = (pop_fitness - np.min(pop_fitness)) / (np.max(pop_fitness) - np.min(pop_fitness))

        print("Population fitness max = {}, Average fitness = {}".format(np.max(pop_fitness), np.mean(pop_fitness)))
        max_model = population[np.argmax(pop_fitness)].model().save("Model_Test1/Model{}-{}".format(iters, np.max(pop_fitness)))

        # Repopulate
        new_population = []
        # # Random weight mutation
        # for i in range(pop_size):
        #     new_population.append(Sim(KerasPickleWrapper(tensorflow.keras.models.clone_model(model))))

        #     parents = np.random.choice(population, size=2, replace=False, p=scaled_fitness/np.sum(scaled_fitness))
        #     weights1 = parents[0].model().get_weights()
        #     weights2 = parents[1].model().get_weights()

        #     new_weights = copy.deepcopy(weights1)

        #     for j in range(len(new_weights)):
        #         if len(new_weights[j].shape)==2:
                    
        #             for x in range(new_weights[j].shape[0]):
        #                 for y in range(new_weights[j].shape[1]):
        #                     val = np.random.choice([1,2,3], p=[0.497, 0.497, 0.006])
        #                     if val == 1:
        #                         new_weights[j][x][y] = weights1[j][x][y]
        #                     elif val == 2:
        #                         new_weights[j][x][y] = weights2[j][x][y]
        #                     else:
        #                         new_weights[j][x][y] = np.random.uniform()

        #     new_population[-1].model().set_weights(new_weights)

        # Certain amount of passed bots pull through then rest random
        for i in range(bots_to_mate):
            new_population.append(Sim(KerasPickleWrapper(tensorflow.keras.models.clone_model(model))))

            parents = np.random.choice(population, size=2, replace=False, p=scaled_fitness/np.sum(scaled_fitness))
            weights1 = parents[0].model().get_weights()
            weights2 = parents[1].model().get_weights()

            new_weights = copy.deepcopy(weights1)

            for j in range(len(new_weights)):
                if len(new_weights[j].shape)==2:
                    
                    for x in range(new_weights[j].shape[0]):
                        for y in range(new_weights[j].shape[1]):
                            val = np.random.choice([1,2], p=[0.5, 0.5])
                            if val == 1:
                                new_weights[j][x][y] = weights1[j][x][y]
                            elif val == 2:
                                new_weights[j][x][y] = weights2[j][x][y]

            new_population[-1].model().set_weights(new_weights)

        for i in range(pop_size - bots_to_mate):
            new_population.append(Sim(KerasPickleWrapper(tensorflow.keras.models.clone_model(model))))

        population = new_population







## TODO: Log memory spread stuff
if __name__ == '__main__':
    freeze_support()
    print(f"Start time {datetime.now().strftime('%d/%m/%Y %H:%M:%S')}")
    ga(60, 12, 1, 50)
    print(f"End time {datetime.now().strftime('%d/%m/%Y %H:%M:%S')}")

    # model = Sequential()
    # model.add(Input(shape=(5, 10)))
    # model.add(LSTM(7, return_sequences=False))
    # model.add(Dense(5))
    # model.add(Dense(4))

    # # # # print(model.get_weights())

    # sims = []

    # for i in range(10):
    #     # temp = KerasPickleWrapper(tensorflow.keras.models.clone_model(model))
    #     temp = KerasPickleWrapper(model)
    #     sims.append(Sim(temp))
    
    # p = Pool(10)
    # out = p.map(Sim.run, sims)#,models)

    # logs = [out[i].log for i in range(10)]

    # plot_sims(logs)
    # for i in range(20):
    #     print(out[i].fitness())


    # tom = Sim(KerasPickleWrapper(model))
    # tom.run(verbose=True)

    # plot_sims([tom.log])

    # def custom_loss(y_true, y_pred):
    #     print(y_true)
    #     return y_true

    # model.compile(
    #     loss=custom_loss, 
    #     optimizer='adam'
    #     )

    # print(np.array(out[0].agent_logs))
    # print(out[0].fitness())

    # y_loss = np.zeros(np.array(out[0].agent_logs).shape[0])*out[0].fitness()

    # model.fit(
    #     x=np.array(out[0].agent_logs), 
    #     y=y_loss, 
    #     batch_size=10, 
    #     epochs=2, 
    #     verbose=1
    #     )



    


        

