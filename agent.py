import numpy as np
import pandas as pd
from packet import Packet
import time

factor = 50

class Agent(object):

    def __init__(self, sim, id, x, y, facing, model=None, con_radius=0.25, pub_mem_lim=10, pri_mem_lim=10):

        # TODO: AI intergration
        self.sim = sim
        self.id = id
        self.x = x
        self.y = y
        self.facing = facing

        self.con_radius = con_radius

        self.pub_mem = []
        self.pri_mem = []

        self.mem_pc = 0

        self.pub_mem_lim = pub_mem_lim
        self.pri_mem_lim = pri_mem_lim

        self.internal_log = pd.DataFrame()
        self.ai = model

        # Logging info
        self.track2 = [0,0]
        self.since_last = [0,0,0]


    def move(self, dangle, speed):
        self.facing += dangle

        if self.facing > 2*np.pi:
            self.facing = 0
        elif self.facing < 0:
            self.facing = 2*np.pi

        self.x += np.cos(self.facing) * speed
        self.y += np.sin(self.facing) * speed


    # TODO: Could convert to just sending ture or false if needed to for faster
    def recieved(self, packet):
        if packet.sendid != self.id and (packet.recid == self.id or packet.recid == -1):

            if packet.type == 1:
                return True

            if packet.type == 2 and self.space_in_mem('pub') and (not self.mem_has_id(packet.data.id, 'pri')) and (not self.mem_has_id(packet.data.id, 'pub')):
                self.pushto_mem(packet.data, 'pub')
                self.track2[0] += 1
                self.track2[1] += 1
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

            if i.id != self.id and np.sqrt(pow(self.x - i.x, 2) + pow(self.y - i.y, 2)) <= self.con_radius:
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

        vecs = []

        for i in self.sim.agents:

            if i.id != self.id:
                
                dist = np.sqrt(pow(self.x - i.x, 2) + pow(self.y - i.y, 2))
                invdist = 0.24 - dist

                if dist <= self.con_radius:
                    vecs.append(np.array( [(self.x-i.x)*invdist/dist, (self.y-i.y)*invdist/dist] ))

        resultant_vec = np.array([0.0,0.0])
        for i in vecs:
            resultant_vec += i

        resultant_vec += np.array( [-self.x*np.sqrt(8) - np.sqrt(pow(self.x, 2) + pow(self.y, 2))*0.03, -self.y*np.sqrt(8) - np.sqrt(pow(self.x, 2) + pow(self.y, 2))*0.03] )

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

            if i.id != self.id and np.sqrt(pow(self.x - i.x, 2) + pow(self.y - i.y, 2)) <= self.con_radius:

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
            dist_to_point = np.sqrt(pow(self.x - self.pub_mem[self.mem_pc].x, 2) + pow(self.y - self.pub_mem[self.mem_pc].y, 2))

        # Since last suicide
        # Since last migration
        # Since last replication


        self.internal_log = pd.concat([self.internal_log, pd.DataFrame([[apVSam, splt, total_agents_around, dupes_ratio, avgspace, dist_to_point]+self.since_last])], axis=0)

        if self.internal_log.shape[0] > 19:
            self.internal_log = self.internal_log.iloc[1:]




        if self.ai != None:
            pass
            # TODO: Make AI use logs to do actions


        self.track2 = [0,0]
        self.mem_pc += 1
        if self.mem_pc >= len(self.pub_mem):
            self.mem_pc = 0
        for i in range(len(self.since_last)):
            self.since_last[i] += 1

