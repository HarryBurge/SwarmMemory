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

#include "Util.cpp"
#include "Visuals.h"
#include "Agent.h"
using namespace std;

const string filename = "SimpleSuicideReplication.csv";

const int num_swarm = 50;

/* Visual stuff */
const bool draw_conn_circles = false;
const bool draw_conn_connections = true;

/* Chance to lose agent */
const float lose_agent = -0.001;

/* Amount of data to start with in begining*/
const int data_at_start = 1;

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

    if (swarm[i]->mem->pub_has_data_id(0)) {
        b = 0.6;
    }

    if (swarm[i]->mem->pub_has_data_id(1)) {
        g = 0.6;
    }

    render_circle(swarm[i]->body, 20, r, g, b, true);

    if (draw_conn_circles) {
        render_circle(swarm[i]->conn_area, 20, 1, 0, 0, false);
    }
}


/* Main control loop - Basically iterator */
int draw_loop(GLFWwindow* window, vector<Agent*> swarm)
{
    /* Logging */
    ofstream outputFile;

    outputFile.open(filename);
    outputFile << "iteration,";

    for (int i = 0; i < data_at_start + data_during; i++) {
        outputFile << "c" << i << ",m" << i << ",s" << i << ",mx" << i << ",mn" << i;
    }

    outputFile << "\n";


    //int data_going = 0;

    vector<Coord> data_areas;

    /* random data at the start */
    for (int i = 0; i < data_at_start; i++) {
        bool yn = true;

        while (yn) {
            int ind_a = rand() % swarm.size();

            if (swarm[ind_a]->mem->space_in_pri()) {

                //float x = -0.6 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.6 - -0.6)));
                //float y = -0.6 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.6 - -0.6)));

                swarm[ind_a]->mem->push_pri_mem(Data(i, ind_a, 0, Coord(swarm[ind_a]->body.center.x, swarm[ind_a]->body.center.y)));

                // Visuals
                data_areas.push_back(Coord(swarm[ind_a]->body.center.x, swarm[ind_a]->body.center.y));

                yn = false;
            }
        }
    }

    while (!glfwWindowShouldClose(window) && swarm.size() > 0)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < data_areas.size(); i++) {
            for (int j = 0; j < max_dupes+1; j++) {
                if (i == 0) {
                    render_circle(Circle(data_areas[i].x, data_areas[i].y, 0 + j * steper), 20, 0, 0, 0.6, false);
                }
                else if (i == 1) {
                    render_circle(Circle(data_areas[i].x, data_areas[i].y, 0 + j * steper), 20, 0, 0.6, 0, false);
                }
            }
        }

        /* Random data learn */
        float rando = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));

        //if (rando < data_going_random && data_going <= data_during) {
        //    data_going++;
        //    bool yn = true;

        //    while (yn) {
        //        int ind_a = rand() % swarm.size();

        //        if (swarm[ind_a]->mem->space_in_pri()) {
        //            swarm[ind_a]->mem->push_pri_mem(Data(data_at_start - 1 + data_going, ind_a, 4, 0, Coord(0, 0)));
        //            yn = false;
        //        }
        //    }
        //}

        /* Draw last frame */
        for (int i = 0; i < swarm.size(); i++) {
            draw_agent_stuffs(swarm, i);
        }

        /* step simulation */
        //tbb::parallel_for(tbb::blocked_range<int>(0, swarm.size()), [&](tbb::blocked_range<int> r) {
        //    for (int i = r.begin(); i < r.end(); ++i)
        //        swarm[i] -> step(swarm);
        //    });

        for (int i = 0; i < swarm.size(); i++) {
            swarm[i]->step(swarm);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();


        /* Loss of agent */
        rando = 0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)));

        if (rando < lose_agent && iterations > 100) {
            swarm.pop_back();
        }

        /* debug */
        int counter[data_at_start + data_during] = { 0 };
        
        vector<float> dists_per_data[data_at_start + data_during];

        for (int i = 0; i < swarm.size(); i++) {
            for (int j = 0; j < data_at_start + data_during; j++) {
                if (swarm[i]->mem->pub_has_data_id(j)) {
                    counter[j]++;
                    dists_per_data[j].push_back(swarm[i]->body.center.distance(swarm[i]->mem->get_pub_id(j).target_area));
                }
            }
        }

        cout << iterations << " ";
        outputFile << iterations;
        for (int i = 0; i < data_at_start + data_during; i++) {
            cout << counter[i] << " ";
            outputFile << "," << counter[i];

            double sum = std::accumulate(dists_per_data[i].begin(), dists_per_data[i].end(), 0.0);
            double mean = sum / dists_per_data[i].size();

            double sq_sum = std::inner_product(dists_per_data[i].begin(), dists_per_data[i].end(), dists_per_data[i].begin(), 0.0);
            double stdev = std::sqrt(sq_sum / dists_per_data[i].size() - mean * mean);

            double max = *max_element(std::begin(dists_per_data[i]), std::end(dists_per_data[i]));
            double min = *min_element(std::begin(dists_per_data[i]), std::end(dists_per_data[i]));

            cout << mean << " " << stdev << " " << max << " " << min;
            outputFile << "," << mean << "," << stdev << "," << max << "," << min;
        }
        cout << endl;
        outputFile << "\n";

        ///* Debug */
        //cout << swarm[25]->to_string() << endl;

        iterations++;

        if (iterations > runtime) {
            outputFile.close();
            return 0;
        }

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