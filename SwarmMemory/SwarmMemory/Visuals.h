#ifndef VIS_H
#define VIS_H

#include<GLFW/glfw3.h>
#include "Util.cpp"
#include "Agent.h"

void render_triangle(Triangle);
void render_circle(Circle, int, float, float, float, bool);
void render_quadrilatral(Quadrilateral);
void render_line(Coord, Coord, float, float, float);

#endif