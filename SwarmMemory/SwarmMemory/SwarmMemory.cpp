#include <iostream>
#include<GLFW/glfw3.h>
#include <stdlib.h>
#include <ctime>
#include <tbb/tbb.h>

#include "Util.cpp"
#include "Visuals.h"
#include "Agent.h"

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

        /* Draw Image */
        for (int i = 0; i < swarmsize; i++) {
            render_circle(swarm[i] -> body, 20, 1, 1, 0, true);
            render_circle(swarm[i] -> conn_area, 20, 1, 0, 0, false);
        }
        

        /*tbb::parallel_for(tbb::blocked_range<int>(0, swarmsize), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); ++i)
                render_agent(swarm[i]);
            });
        */

        /* Step simulation */
        tbb::parallel_for(tbb::blocked_range<int>(0, swarmsize), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); ++i)
                swarm[i] -> step();
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

    const int num_swarm = 100;
    Agent* swarm[num_swarm];

    for (int i = 0; i < num_swarm; i++) {
        float x = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - -1)));
        float y = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - -1)));
        float f = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6 - 0)));
        
        swarm[i] = new Agent(i, x, y, f);
    }

    /* The main control loop */
    frame_loop(window, swarm, num_swarm);

    glfwTerminate();
}