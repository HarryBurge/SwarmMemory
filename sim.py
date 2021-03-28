from agent import Agent
from data import Data
import numpy as np
import pandas as pd
from pathos.multiprocessing import ProcessingPool as Pool
from pathos.multiprocessing import freeze_support
from tensorflow.keras.layers import LSTM, Dense, Dropout, Input
from tensorflow.keras.models import Sequential
import tensorflow
import pygad.kerasga
import matplotlib.pyplot as plt

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
    model.add(Input(shape=(1,9)))
    model.add(LSTM(10, return_sequences=True))
    model.add(Dropout(0.1))
    model.add(LSTM(5))
    model.add(Dropout(0.1))
    model.add(Dense(4))
    model.add(Dense(4))

    # models = []
    # sims = []

    # for i in range(6):
    #     models.append(tensorflow.keras.models.clone_model(model))
    #     sims.append(Sim(models[-1]))
    
    # p = Pool()
    # p.map(Sim.run, sims)


    # tom = Sim(model)
    # tom.run(verbose=True)

    keras_ga= pygad.kerasga.KerasGA(model=model,
                                     num_solutions=50)

    def fitness_func(solution, sol_index):
        model_weights_matrix = pygad.kerasga.model_weights_as_matrix(model=model,
                                                                 weights_vector=solution)
        model.set_weights(weights=model_weights_matrix)

        # Run Model
        Sim(model).run()

        return 1

    def callback_generation(ga_instance):
        print("Generation = {generation}".format(generation=ga_instance.generations_completed))
        print("Fitness    = {fitness}".format(fitness=ga_instance.best_solution()[1]))

        solution, solution_fitness, solution_idx = ga_instance.best_solution()
        best_solution_weights = pygad.kerasga.model_weights_as_matrix(model=model,
                                                                weights_vector=solution)

    def stop(ga_instance, fitnesses):
        # After run fetch best fitness
        ga_instance.plot_result(title="PyGAD & Keras - Iteration vs. Fitness", linewidth=4)

        solution, solution_fitness, solution_idx = ga_instance.best_solution()
        print("Fitness value of the best solution = {solution_fitness}".format(solution_fitness=solution_fitness))

    num_generations = 10
    num_parents_mating = 5
    initial_population = keras_ga.population_weights

    ga_instance = pygad.GA(num_generations=num_generations, 
                        num_parents_mating=num_parents_mating, 
                        initial_population=initial_population,
                        fitness_func=fitness_func,
                        on_generation=callback_generation,
                        on_stop=stop)

    try:
        ga_instance.run()
    except KeyboardInterrupt:
        ga_instance.plot_result(title="PyGAD & Keras - Iteration vs. Fitness", linewidth=4)
        solution, solution_fitness, solution_idx = ga_instance.best_solution()
        print("Fitness value of the best solution = {solution_fitness}".format(solution_fitness=solution_fitness))