#include <iostream>
#include<GLFW/glfw3.h>
#include <stdlib.h>
#include <ctime>
#include <tbb/tbb.h>
#include <string>

#include "Util.cpp"
#include "Visuals.h"
#include "Agent.h"
#include "Data.h"

/* Key call back for window control */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}


/* Main control loop - Basically iterator */
int frame_loop(GLFWwindow* window, Agent* swarm[], int swarmsize)
{
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        //Debug
        render_circle(swarm[0]->body, 20, 1, 1, 1, true);
        render_circle(swarm[0]->conn_area, 20, 1, 0, 1, false);

        /* Draw Image */
        // change i to 0 when stopping debug
        for (int i = 1; i < swarmsize; i++) {
            render_circle(swarm[i] -> body, 20, 1, 1, 0, true);
            render_circle(swarm[i] -> conn_area, 20, 1, 0, 0, false);
        }

        /* Step simulation */
        tbb::parallel_for(tbb::blocked_range<int>(0, swarmsize), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); ++i)
                swarm[i] -> step(swarm, swarmsize);
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

    const int num_swarm = 10;
    Agent* swarm[num_swarm];

    for (int i = 0; i < num_swarm; i++) {
        float x = -0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.1 - -0.1)));
        float y = -0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.1 - -0.1)));
        float f = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6 - 0)));
        
        swarm[i] = new Agent(i, x, y, f);
    }

    /* Push local data onto agents memory */
    for (int i = 0; i < num_swarm; i++) {
        swarm[i]->mem.push_pri_mem(Data(10 + i));
    }

    //TODO: Need to make policy for public memory spread
    //TODO: Make recieve have more infomation to it like an opcode for whether to store or what
    //TODO: Possibly have some sort of jump thing on the packet
    //TODO: Create a poll info thing
    //TODO: Detect if a swarm member goes down



    /* The main control loop */
    frame_loop(window, swarm, num_swarm);

    glfwTerminate();
}