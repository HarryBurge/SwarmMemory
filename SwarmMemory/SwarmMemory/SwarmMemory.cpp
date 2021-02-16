#include <iostream>
#include<GLFW/glfw3.h>
#include <stdlib.h>
#include <ctime>
#include <tbb/tbb.h>
#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <numeric>
#include <thread>

#include "Util.cpp"
#include "Visuals.h"
#include "Agent.h"
using namespace std;

const string prelimfilename = "SimpleSuicideReplication";

const int num_swarm = 30;

/* Visual stuff */
const bool draw_conn_circles = false;
const bool draw_conn_connections = true;

/* Chance to lose agent */
const float lose_agent = -0.003;
const float corrilated_failure = -0.0003;

/* Amount of data to start with in begining*/
const int data_at_start = 9;

/* Chance to produce data, and amount to go up to*/
const float data_going_random = 0.1;
const int data_during = 0;

/* Test iterator */
int iterations = 0;
const int runtime = 10000;


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
    float r = 0.2;
    float g = 0.2;
    float b = 0.2;

    if (swarm[i]->mem->pub_has_data_id(0) || swarm[i]->mem->pri_has_data_id(0)) {
        g = 0.8;
    }

    if (swarm[i]->mem->pub_has_data_id(1) || swarm[i]->mem->pri_has_data_id(1)) {
        b = 0.8;
    }

    render_circle(swarm[i]->body, 20, r, g, b, true);

    if (draw_conn_circles) {
        render_circle(swarm[i]->conn_area, 20, 1, 0, 0, false);
    }
}


/* Main control loop - Basically iterator */
int draw_loop(GLFWwindow* window, vector<Agent*> swarm, string filename, vector<Coord> data_areas)
{
    /* Logging */
    ofstream outputFile;

    outputFile.open(filename);
    outputFile << "iteration, total_datas, (x,y,c1,...,cn)*";
    outputFile << "\n";

    for (int i = 0; i < data_areas.size(); i++) {

        float mindist = 100;
        int min_arg = 0;

        for (int j = 0; j < swarm.size(); j++) {
            if (mindist > data_areas[i].distance(swarm[j]->body.center)) {
                mindist = data_areas[i].distance(swarm[j]->body.center);
                min_arg = j;
            }
        }

        cout << min_arg << endl;

        // Push data to swarm member
        swarm[min_arg]->mem->push_pri_mem(Data(i, min_arg, 0, data_areas[i]));
    }


    while (!glfwWindowShouldClose(window) && swarm.size() > 0)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Background Colour
        render_circle(Circle(0,0,2), 10, 1, 1, 1, true);

        for (int i = 0; i < data_areas.size(); i++) {
            render_circle(Circle(data_areas[i].x, data_areas[i].y, 0.03), 20, 1, 0, 0, true);
        }

        /* Draw last frame */
        for (int i = 0; i < swarm.size(); i++) {
            draw_agent_stuffs(swarm, i);
        }

        /* step simulation */
        tbb::parallel_for(tbb::blocked_range<int>(0, swarm.size()), [&](tbb::blocked_range<int> r) {
            for (int i = r.begin(); i < r.end(); ++i)
                swarm[i] -> step(swarm);
            });

        glfwSwapBuffers(window);
        glfwPollEvents();


        /* Loss of agent */
        float rando = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));

        if (rando < lose_agent && iterations > 100) {
            swarm.pop_back();
        }


        /* Corrilated failure */
        rando = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));

        vector<Agent*> swarmtemp;
        if (rando < corrilated_failure && iterations > 100) {
            for (int i = 0; i < swarm.size(); i++) {
                if (swarm[i]->body.center.distance(data_areas[rand() % data_areas.size()]) > 0.25) {
                    swarmtemp.push_back(swarm[i]);
                }
            }
            swarm = swarmtemp;
        }


        /* debug */
        int counter[data_at_start + data_during] = { 0 };

        for (int i = 0; i < swarm.size(); i++) {
            for (int j = 0; j < data_at_start + data_during; j++) {
                if (swarm[i]->mem->pub_has_data_id(j)) {
                    counter[j]++;
                }
            }
        }

        cout << iterations;

        for (int i = 0; i < data_at_start + data_during; i++) {
            cout << " " << counter[i] << " ";
        }
        cout << endl;


        /* Logging */
        outputFile << iterations;
        outputFile << "," << data_at_start + data_during;

        for (int i = 0; i < swarm.size(); i++) {
            outputFile << "," << swarm[i]->body.center.x << "," << swarm[i]->body.center.y;

            for (int j = 0; j < data_at_start + data_during; j++) {
                if (swarm[i]->mem->pub_has_data_id(j)) {
                    outputFile << "," << swarm[i]->body.center.distance(swarm[i]->mem->get_pub_id(j).target_area);
                }
                else {
                    outputFile << ",-1";
                }
            }
        }

        outputFile << "\n";

        /* Timing */
        iterations++;

        if (iterations > runtime) {
            
            for (int i = 0; i < data_areas.size(); i++) {
                outputFile << data_areas[i].x << "," << data_areas[i].y << ",";
            }

            outputFile.close();
            return 0;
        }

    }
    return 0;
}


int main()
{
    vector<Coord> data_areas;

    /* random data at the start */
    //for (int i = 0; i < data_at_start; i++) {
    //    // Visuals
    //    data_areas.push_back(Coord(-0.7 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.6 - -0.6))), -0.6 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.6 - -0.6)))));
    //}

    float rang[] = { -0.4, 0, 0.4 };
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            data_areas.push_back(Coord(rang[x], rang[y]));
        }
    }

    for (int k = 1; k < 6; k++) {
        iterations = 0;

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
            float x = -0.7 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.7 - -0.7)));
            float y = -0.7 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.7 - -0.7)));
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
        draw_loop(window, swarm, prelimfilename + "_" + std::to_string(k) + ".csv", data_areas);

        glfwTerminate();
    }
}