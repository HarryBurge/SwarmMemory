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
const int num_swarm = 100;
const bool draw_conn_circles = false;
const bool draw_conn_connections = true;


/* Key call back for window control */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void draw_agent_stuffs(vector<Agent*> swarm, int i) {
    // Connection lines
    if (draw_conn_connections) {
        for (int j = 0; j < swarm.size(); j++) {
            if (swarm[i]->id != swarm[j]->id && swarm[j]->conn_area.point_in_circle(swarm[i]->body.center)) {
                render_line(swarm[i]->body.center, swarm[j]->body.center, 0.4, 0.4, 0.4);
            }
        }
    }

    //Drawing current transfers
    while (swarm[i]->conns.size() != 0) {
        pair<int, pair<Coord, Coord>> line = swarm[i]->conns.back();

        if (line.first == 1) {
            render_line(line.second.first, line.second.second, 0.4, 0.4, 0.4);
        }
        else if (line.first == 2) {
            render_line(line.second.first, line.second.second, 0, 1, 1);
        }
        else if (line.first == 3) {
            render_line(line.second.first, line.second.second, 1, 0, 1);
        }
        else if (line.first == 4) {
            render_line(line.second.first, line.second.second, 1, 1, 1);
        }
        swarm[i]->conns.pop_back();
    }

    //Drawing agents
    float r = 0.6;
    float g = 0.6;
    float b = 0.0;

    if (swarm[i]->mem->pub_has_data_id(1)) {
        g = 1;
    }
    if (swarm[i]->mem->pub_has_data_id(2)) {
        b = 1;
    }

    render_circle(swarm[i]->body, 20, r, g, b, true);

    if (i <= 1) {
        render_circle(swarm[i]->body, 20, 1, 1, 1, true);
    }
    if (draw_conn_circles) {
        render_circle(swarm[i]->conn_area, 20, 1, 0, 0, false);
    }
}


/* Main control loop - Basically iterator */
int draw_loop(GLFWwindow* window, vector<Agent*> swarm)
{

    //Debug
    swarm[0]->mem->push_pri_mem(Data(1, 0, 4, 1, Coord(0, 0)));
    swarm[1]->mem->push_pri_mem(Data(2, 1, 4, 1, Coord(0, 0)));

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        vector<thread> threads;

        for (int i = 0; i < swarm.size(); i++) {
            draw_agent_stuffs(swarm, i);
        }


        /* step simulation */
        tbb::parallel_for(tbb::blocked_range<int>(0, swarm.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); ++i)
                swarm[i] -> step(swarm);
            });

        //for (int i = 0; i < swarm.size(); i++) {
        //    swarm[i]->step(swarm);
        //}

        glfwSwapBuffers(window);
        glfwPollEvents();

        //for (int i = 0; i < swarm.size(); i++) {
        //    for (int j = 0; j < swarm[i]->mem->pub_mem.size(); j++) {
        //        if (swarm[i]->mem->pub_mem[j].replication_num > 0) {
        //            cout << swarm[i]->to_string() << endl;
        //        }
        //    }
        //}

        int counter1 = 0;
        int counter2 = 0;

        for (int i = 0; i < swarm.size(); i++) {
            if (swarm[i]->mem->pub_has_data_id(1)) {
                counter1++;
            }
            if (swarm[i]->mem->pub_has_data_id(2)) {
                counter2++;
            }
        }

        cout << counter1 << "  " << counter2 << endl;

        //string tom;
        //cin >> tom;


       //Sleep(100);
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
        float x = -0.6 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.6 - -0.6)));
        float y = -0.6 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.6 - -0.6)));
        float f = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6 - 0)));
        
        swarm.push_back(new Agent(i, x, y, f));
    }

    //int i = 0;

    //for (float x = -0.8; x < 0.81; x += 0.15) {
    //    for (float y = -0.8; y < 0.81; y += 0.15) {
    //        float f = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6 - 0)));
    //        i++;
    //        swarm.push_back(new Agent(i, x, y, f));
    //    }
    //}


    /* The main control loop */
    draw_loop(window, swarm);

    glfwTerminate();
}