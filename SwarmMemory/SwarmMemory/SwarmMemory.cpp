#include <iostream>
#include<GLFW/glfw3.h>
#include <stdlib.h>
#include <ctime>
#include <tbb/tbb.h>
#include <string>
#include <vector>
#include <thread>

#include "Util.cpp"
#include "Visuals.h"
#include "Agent.h"
using namespace std;
const int num_swarm = 11;


/* Key call back for window control */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


/* Main control loop - Basically iterator */
int draw_loop(GLFWwindow* window, vector<Agent*> swarm)
{

    swarm[0]->mem->push_pri_mem(Data(0, 10000, 1));

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        cout << swarm[0]->to_string() << endl;

        for (int i = 0; i < swarm.size(); i++) {
            render_circle(swarm[i]->body, 20, 1, 1, 0, true);
            render_circle(swarm[i]->conn_area, 20, 1, 0, 0, false);
        }

        for (int i = 0; i < swarm.size(); i++) {
            while (swarm[i]->conns.size() != 0) {
                pair<Coord, Coord> line = swarm[i]->conns.back();
                render_line(line.first, line.second);
                swarm[i]->conns.pop_back();
            }
        }

        /* step simulation */
        tbb::parallel_for(tbb::blocked_range<int>(0, swarm.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); ++i)
                swarm[i] -> step(swarm);
            });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}


int main()
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(1280, 960, "SwarmMemory", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    /* Init swarm */
    srand(static_cast <unsigned> (time(0)));

    vector<Agent*> swarm;

    for (int i = 0; i < num_swarm; i++) {
        float x = -0.3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.3 - -0.3)));
        float y = -0.3 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.3 - -0.3)));
        float f = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6 - 0)));
        
        swarm.push_back(new Agent(i, x, y, f));
    }

    //vector<thread> threads;

    ///* Put agents execution onto diffrent threads */
    //for (int i = 0; i < num_swarm; i++) {
    //    threads.push_back( thread( &Agent::run, swarm[i], swarm) );
    //    threads.back().detach();
    //}


    //TODO: Need to make policy for public memory spread
    //TODO: Make recieve have more infomation to it like an opcode for whether to store or what
    //TODO: Possibly have some sort of jump thing on the packet
    //TODO: Create a poll info thing
    //TODO: Detect if a swarm member goes down



    /* The main control loop */
    draw_loop(window, swarm);

    glfwTerminate();
}