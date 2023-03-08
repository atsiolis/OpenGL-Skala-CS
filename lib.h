#include <GL/glut.h>
#include <GL/glui.h>
#include <time.h>
#include <math.h>
#include <iostream>
using namespace std;

const int mask[] = {0, 1, 2, 2, 4, 0, 4, 4, 8, 1, 0, 2, 8, 1, 8, 0};
const int tab1[] = {4, 3, 0, 3, 1, 4, 0, 3, 2, 2, 4, 2, 1, 1, 0, 4};
const int tab2[] = {4, 0, 1, 1, 2, 4, 2, 2, 3, 0, 4, 1, 3, 0, 3, 4};

// defining the borders of the viewport window
#define TOP_BORDER 500
#define RIGHT_BORDER 500

// defining the code for each of the 5 sections for the CS algorithm
const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

const int SKALA_INSIDE = 0; // 0000
const int SKALA_BOTTOM = 1; // 0001
const int SKALA_TOP = 2;    // 0010
const int SKALA_RIGHT = 4;  // 0100
const int SKALA_LEFT = 8;   // 1000

struct Line
{
    int x1, x2, y1, y2;
};

struct Vector
{
    double x, y, z, k;
};

// creating a line using 2 points. A(x1,y1) and B(x2,y2)
struct Line createLine(int x1, int y1, int x2, int y2)
{
    struct Line line;
    line.x1 = x1;
    line.x2 = x2;
    line.y1 = y1;
    line.y2 = y2;
    return line;
}

// drawing 1 line
void drawLine(int x1, int y1, int x2, int y2)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glColor3ub(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();
    glPopMatrix();
    glPopAttrib();
}
/*
creating a dynamic array of lines that will later be used to draw them
first we allocate the memory needed.
srand(time(NULL)); is used to create lines randomly based on the time
(2 * RIGHT_BORDER)) - RIGHT_BORDER; is used because the viewport is set at -1000 to 1000 left to right
and we need negative coordinates as well as positive ones
*/
struct Line *createLineArray(int numberOfLines)
{
    struct Line *lines;
    int i;
    lines = (Line *)malloc(numberOfLines * sizeof(Line));

    srand(time(NULL));
    for (i = 0; i < numberOfLines; i++)
    {
        lines[i].x1 = (int)(rand() % (2 * RIGHT_BORDER)) - RIGHT_BORDER;
        lines[i].x2 = (int)(rand() % (2 * RIGHT_BORDER)) - RIGHT_BORDER;
        lines[i].y1 = (int)(rand() % (2 * TOP_BORDER)) - TOP_BORDER;
        lines[i].y2 = (int)(rand() % (2 * TOP_BORDER)) - TOP_BORDER;
    }
    return lines;
}

// drawing multiple lines using the function that draws one line multiple times
void drawLines(Line *lines, int numberOfLines)
{
    int i;
    for (i = 0; i < numberOfLines; i++)
    {
        drawLine(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
    }
}

// checks where the coordinates of a line are
int computeCode(double x, double y, int xmin, int xmax, int ymin, int ymax)
{
    // initialized as being inside
    int code = INSIDE;
    if (x < xmin) // to the left of rectangle
        code |= LEFT;
    else if (x > xmax) // to the right of rectangle
        code |= RIGHT;
    if (y < ymin) // below the rectangle
        code |= BOTTOM;
    else if (y > ymax) // above the rectangle
        code |= TOP;
    return code;
}

struct Line cohenSutherlandClip(double x1, double y1, double x2, double y2, int xmin, int xmax, int ymin, int ymax)
{
    struct Line final;
    // Compute region codes for P1, P2
    int code1 = computeCode(x1, y1, xmin, xmax, ymin, ymax);
    int code2 = computeCode(x2, y2, xmin, xmax, ymin, ymax);

    // Initialize line as outside the rectangular window
    bool accept = false;

    while (true)
    {
        if ((code1 == 0) && (code2 == 0))
        {
            // If both endpoints lie within rectangle
            accept = true;
            break;
        }
        else if (code1 & code2)
        {
            // If both endpoints are outside rectangle, in same region
            break;
        }
        else
        {
            // Some segment of line lies within the rectangle
            int code_out;
            double x, y;

            // At least one endpoint is outside the rectangle, pick it.
            if (code1 != 0)
                code_out = code1;
            else
                code_out = code2;

            /*  
            Find intersection point using formulas:
                y = y1 + slope * (x - x1), 
                x = x1 + (1 / slope) * (y - y1)
            */
            if (code_out & TOP)
            {
                // point is above the clip rectangle
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
            }
            else if (code_out & BOTTOM)
            {
                // point is below the rectangle
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
            }
            else if (code_out & RIGHT)
            {
                // point is to the right of rectangle
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
            }
            else if (code_out & LEFT)
            {
                // point is to the left of rectangle
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
            }

            /*  
            Intersection point x, y is found
            The point outside rectangle is replaced by intersection point
            */
            if (code_out == code1)
            {
                x1 = x;
                y1 = y;
                code1 = computeCode(x1, y1, xmin, xmax, ymin, ymax);
            }
            else
            {
                x2 = x;
                y2 = y;
                code2 = computeCode(x2, y2, xmin, xmax, ymin, ymax);
            }
        }
    }

    // save the coordinates
    if (accept)
    {
        final.x1 = x1;
        final.x2 = x2;
        final.y1 = y1;
        final.y2 = y2;
    }

    return final;
}

// uses the function that clips 1 line multiple times
void csClipLines(Line *lines, int xmin, int xmax, int ymin, int ymax, int numberOfLines)
{
    int i;
    struct Line *final = createLineArray(numberOfLines);

    for (i = 0; i < numberOfLines; i++)
    {
        final[i] = cohenSutherlandClip(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, xmin, xmax, ymin, ymax);
    }
    drawLines(final, numberOfLines);
}

Vector crossProduct(Vector u, Vector v)
{
    Vector vector;

    vector.x = (u.y * v.z - u.z * v.y);
    vector.y = (u.z * v.x - u.x * v.z);
    vector.z = (u.x * v.y - u.y * v.x);
    return vector;
}

Vector cross(Vector u, Vector v)
{
    Vector vector;
    double z = u.x * v.y - u.y * v.x;
    vector.x = (u.y * v.z - u.z * v.y)/z;
    vector.y = (u.z * v.x - u.x * v.z)/z;
    vector.z = (u.x * v.y - u.y * v.x);
    return vector;
}

int dot(Vector u, Vector v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

// checks where the coordinates of a line are
int skalaComputeCode(double x, double y, int xmin, int xmax, int ymin, int ymax)
{
    // initialized as being inside
    int code = SKALA_INSIDE;

    if (x < xmin)   // to the left of rectangle
        code |= SKALA_LEFT;
    else if (x > xmax)  // to the right of rectangle
        code |= SKALA_RIGHT;
    if (y < ymin)   // below the rectangle
        code |= SKALA_BOTTOM;
    else if (y > ymax)  // above the rectangle
        code |= SKALA_TOP;

    return code;
}

struct Line skalaClip(double x1, double y1, double x2, double y2, int xmin, int xmax, int ymin, int ymax)
{
    int newx1 = x1, newx2 = x2, newy1 = y1, newy2 = y2;
    Line final;
    int i;
    Vector a, b, newXA, newXB;
    Vector x[4];
    Vector e[4];
    a.x = x1;
    a.y = y1;
    a.z = 1;
    b.x = x2;
    b.y = y2;
    b.z = 1;
    Vector p = crossProduct(a, b);

    x[0].x = xmin;
    x[0].y = ymin;
    x[0].z = 1;
    x[1].x = xmax;
    x[1].y = ymin;
    x[1].z = 1;
    x[2].x = xmax;
    x[2].y = ymax;
    x[2].z = 1;
    x[3].x = xmin;
    x[3].y = ymax;
    x[3].z = 1;

    e[0].x = 0;
    e[0].y = 1;
    e[0].z = ymin;
    e[1].x = 1;
    e[1].y = 0;
    e[1].z = xmax;
    e[2].x = 0;
    e[2].y = 1;
    e[2].z = ymax;
    e[3].x = 1;
    e[3].y = 0;
    e[3].z = xmin;

    //  Compute region codes for P1, P2
    int code1 = skalaComputeCode(x1, y1, xmin, xmax, ymin, ymax);
    int code2 = skalaComputeCode(x2, y2, xmin, xmax, ymin, ymax);

    if ((code1 == 0) && (code2 == 0))
    {
        //if both endpoints lie within rectangle
        final.x1 = x1;
        final.x2 = x2;
        final.y1 = y1;
        final.y2 = y2;
        return final;
        exit;
    }
    if (code1 & code2)
    {
        exit;
    }

    int c[4];
    int sum = 0;

    for (i = 0; i < 4; i++)
    {
        if (dot(p, x[i]) >= 0)
        {
            //sum |= (1 << i);
             c[i]=1;
        }
        else
            c[i] = 0;
    }

    for (i = 0; i < 3; i++)
    {
        if (c[i] != 0)
        {
            sum = sum + pow(2, i);
        }
    }

    //if both points lie outside the rectangle 
    if ((sum == 0) || (sum == 15))
    {
        exit;
    }

    int k = tab1[sum];
    int j = tab2[sum];

    //if there are two intersections of the rectangle 
    if ((code1 != 0) && (code2 != 0))
    {
        newXA = crossProduct(p, e[k]);
        newXB = crossProduct(p, e[j]);
        final.x1 = newXA.x;
        final.y1 = newXA.y;
        final.x2 = newXB.x;
        final.y2 = newXB.y;
        return final;
    }
    //if there is only one intersection and point xb is outside 
    if (code1 == 0)
    {
        if ((code2 & mask[sum]) != 0)
        {
            newXB = cross(p, e[k]);

            newx2 = newXB.x;
            newy2 = newXB.y;
        }
        else
        {
            newXB = cross(p, e[j]);
            newx2 = newXB.x;
            newy2 = newXB.y;
        }
    }

    //if there is only one intersection and point xa is outside 
    if (code2 == 0)
    {
        if ((code1 & mask[sum]) != 0)
        {
            newXA = cross(p, e[k]);
            newx1 = newXA.x;
            newy1 = newXA.y;
        }
        else
        {
            newXA = cross(p, e[j]);
            newx1 = newXA.x;
            newy1 = newXA.y;
        }
    }

    final.x1=newx1;
    final.y1=newy1;

    final.x2=newx2;
    final.y2=newy2;
    return final;
}


void skalaClipLines(Line *lines, int xmin, int xmax, int ymin, int ymax, int numberOfLines)
{
    int i;
    struct Line *final = createLineArray(numberOfLines);

    for (i = 0; i < numberOfLines; i++)
    {
        final[i] = skalaClip(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, xmin, xmax, ymin, ymax);
    }

    drawLines(final, numberOfLines);
}