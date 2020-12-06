#include "Visuals.h"

#include<GLFW/glfw3.h>

#include "Util.cpp"
#include "Agent.h"

void render_triangle(Triangle* body) {
    /* Draw triangles based on coords */
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 3; i++) {
        glVertex2f(body->cs[i].x, body->cs[i].y);
    }
    glEnd();
}

void render_circle(Circle* body, int num_segments = 20, float r = 1, float g = 1, float b = 1, bool filled = false)
{
    if (filled) {glBegin(GL_TRIANGLE_FAN);}
    else {glBegin(GL_LINE_LOOP);}

    glColor3f(r, g, b);

    for (int ii = 0; ii < num_segments; ii++)
    {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

        float x = body->r * cosf(theta);//calculate the x component
        float y = body->r * sinf(theta);//calculate the y component

        glVertex2f(x + body->center.x, y + body->center.y);//output vertex

    }
    glColor3f(1, 1, 1);
    glEnd();
}

void render_quadrilatral(Quadrilateral* body) {
    glBegin(GL_TRIANGLES);
    glVertex2f(body->cs[0].x, body->cs[0].y);
    glVertex2f(body->cs[1].x, body->cs[1].y);
    glVertex2f(body->cs[2].x, body->cs[2].y);
    glVertex2f(body->cs[0].x, body->cs[0].y);
    glVertex2f(body->cs[2].x, body->cs[2].y);
    glVertex2f(body->cs[3].x, body->cs[3].y);
    glEnd();
}
