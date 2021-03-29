import numpy as np
import pandas as pd
from packet import Packet
import time
from pathos.multiprocessing import freeze_support

factor = 50
freeze_support()

def dist(x1,y1,x2,y2):
    return np.sqrt(np.power(x1 - x2, 2) + np.power(y1 - y2, 2))

class Agent(object):

    def __init__(self, sim, id, x, y, facing, model=None, con_radius=0.25, pub_mem_lim=10, pri_mem_lim=10):

        self.sim = sim
        self.id = id

        self.x = x
        self.y = y
        self.facing = facing
        self.con_radius = con_radius

        self.pub_mem = []
        self.pri_mem = []
        self.pub_mem_lim = pub_mem_lim
        self.pri_mem_lim = pri_mem_lim

        self.mem_pc = 0

        self.ai = model

        # Logging info
        self.track2 = np.zeros(2)
        self.since_last = np.zeros(3)


    def move(self, dangle, speed):
        self.facing += dangle

        if self.facing > 2*np.pi:
            self.facing = 0
        elif self.facing < 0:
            self.facing = 2*np.pi

        self.x += np.cos(self.facing) * speed
        self.y += np.sin(self.facing) * speed


    def recieved(self, packet):
        if packet.sendid != self.id and (packet.recid == self.id or packet.recid == -1):

            if packet.type == 1:
                return True

            if packet.type == 2 and self.space_in_mem('pub') and (not self.mem_has_id(packet.data.id, 'pri')) and (not self.mem_has_id(packet.data.id, 'pub')):
                self.pushto_mem(packet.data, 'pub')
                self.track2 += 1
                return True
            elif packet.type == 2:
                self.track2[1] += 1

            if packet.type == 3 and self.mem_has_id(packet.data.id, 'pub'):
                return True

            if packet.type == 4 and self.mem_has_id(packet.data.id, 'pub'):
                return self.pub_mem_lim - len(self.pub_mem)

        return False

    def message(self, packet):
        
        collect = []

        for i in self.sim.agents:

            if i.id != self.id and dist(self.x, self.y, i.x, i.y) <= self.con_radius:
                collect.append(i.recieved(packet))

        return collect


    def pushto_mem(self, data, pri_or_pub):

        if pri_or_pub == 'pub' and len(self.pub_mem) < self.pub_mem_lim - 1:
            self.pub_mem.append(data)
            return True
        elif pri_or_pub == 'pri' and len(self.pri_mem) < self.pri_mem_lim - 1:
            self.pri_mem.append(data)
            return True
        return False

    def space_in_mem(self, pri_or_pub):
        if pri_or_pub == 'pub' and len(self.pub_mem) < self.pub_mem_lim:
            return True
        elif pri_or_pub == 'pri' and len(self.pri_mem) < self.pri_mem_lim:
            return True
        return False

    def mem_has_id(self, id, pri_or_pub):
        if pri_or_pub == 'pub':
            for i in self.pub_mem:

                if i.id == id:
                    return True

        if pri_or_pub == 'pri':
            for i in self.pri_mem:

                if i.id == id:
                    return True

        return False

    def get_data_mem(self, id, pri_or_pub):
        if pri_or_pub == 'pub':
            for i in self.pub_mem:

                if i.id == id:
                    return i

        if pri_or_pub == 'pri':
            for i in self.pri_mem:

                if i.id == id:
                    return i

        return False

    def remove_data_mem(self, id, pri_or_pub):
        if pri_or_pub == 'pub':

            index = None
            for i in range(len(self.pub_mem)):

                if self.pub_mem[i].id == id:
                    index = i
            if index != None:
                self.pub_mem = self.pub_mem[:i] + self.pub_mem[i+1:]

        if pri_or_pub == 'pri':

            index = None
            for i in range(len(self.pri_mem)):

                if self.pri_mem[i].id == id:
                    index = i
                
            if index != None:
                self.pri_mem = self.pri_mem[:i] + self.pri_mem[i+1:]

        return False


    def step(self):
        # self.move(np.random.uniform(-0.001, 0.004), 0.0002)

        vecs = np.zeros((len(self.sim.agents), 2))

        for i in np.arange(len(self.sim.agents)):

            if self.sim.agents[i].id != self.id:
                
                dist_to = dist(self.x, self.y, self.sim.agents[i].x, self.sim.agents[i].y)
                invdist = 0.24 - dist_to

                if dist_to <= self.con_radius:
                    vecs[i] = np.array( [(self.x-self.sim.agents[i].x)*invdist/dist_to, (self.y-self.sim.agents[i].y)*invdist/dist_to] )


        resultant_vec = np.sum(vecs, axis=0)

        centdist = dist(self.x, self.y, 0, 0)*0.03
        resultant_vec += np.array( [-self.x*np.sqrt(8)-centdist, -self.y*np.sqrt(8)-centdist] )

        angle = np.arctan(resultant_vec[1] / resultant_vec[0])
        if resultant_vec[0] < 0:
            angle += np.pi

        self.facing = angle
        self.move(0, 0.0002)


        # Aproved 2 versus amount
        apVSam=0
        if self.track2[1] != 0:
            apVSam = self.track2[0]/self.track2[1]

        # Space left in public memory
        splt = self.pub_mem_lim - len(self.pub_mem)

        # Duplication ratio
        # Amount of agents around
        # Average space in other agents mems

        total_agents_around = 0
        duplicates = 0
        total_space_in_agent_mem = 0

        for i in self.sim.agents:

            if i.id != self.id and dist(self.x, self.y, i.x, i.y) <= self.con_radius:

                total_space_in_agent_mem += i.pub_mem_lim - len(i.pub_mem)
                total_agents_around += 1
                if len(self.pub_mem) > 0 and i.mem_has_id(self.pub_mem[self.mem_pc], 'pub'):
                    duplicates += 1

        dupes_ratio = 0
        avgspace = self.pub_mem_lim - len(self.pub_mem)

        if total_agents_around != 0:
            dupes_ratio = duplicates/total_agents_around
            avgsapce = total_space_in_agent_mem/total_agents_around

        # Distance to data point
        dist_to_point = 0
        if len(self.pub_mem) > 0:
            dist_to_point = dist(self.x, self.y, self.pub_mem[self.mem_pc].x, self.pub_mem[self.mem_pc].y)

        # Since last suicide
        # Since last migration
        # Since last replication

        to_ai = np.array([[apVSam, splt, total_agents_around, dupes_ratio, avgspace, dist_to_point, self.since_last[0], self.since_last[1], self.since_last[2]]])


        output = self.ai()(to_ai)
        argmax = np.argmax(output)
        if argmax == 0: #Do nothing
            pass
        elif argmax == 1 and len(self.pub_mem) != 0: #Replicate
            self.message(Packet(2, self.id, -1, data=self.pub_mem[self.mem_pc]))
            self.since_last[0] = 0
        elif argmax == 2 and len(self.pub_mem) != 0: #Suicide
            self.since_last[1] = 0
            self.remove_data_mem(self.pub_mem[self.mem_pc].id, 'pub')
        elif argmax == 3: #Migrate
            self.since_last[2] = 0
            pass


        self.mem_pc += 1
        if self.mem_pc >= len(self.pub_mem):
            self.mem_pc = 0

        self.track2 = self.track2*0.8
        self.since_last += 1


if __name__ == '__main__':
    print("Can't be run")
