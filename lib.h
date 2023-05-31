#include <GL/glut.h>
#include <GL/glui.h>
#include <time.h>
#include <math.h>
#include <iostream>
using namespace std;

const int mask[] = { -1, 4, 4, 2, 2, -2, 4, 8, 8, 4, -2, 2, 2, 4, 4, -1 };
const int tab2[] = { -1, 0, 0, 1, 1, -2, 0, 2, 2, 0, -2, 1, 1, 0, 0, -1 };
const int tab1[] = { -1, 3, 1, 3, 2, -2, 2, 3, 3, 2, -2, 2, 3, 1, 3, -1 };

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

struct Point
{
    float x, y;
};

struct Line
{
    float x1, x2, y1, y2;
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

Point crossProduct(Point edge1, Point edge2, Point p1, Point p2)
{
    float a1,b1,c1,a2,b2,c2,d,dx,dy;

    a1=edge1.y-edge2.y;
    b1=edge2.x-edge1.x;
    c1=edge1.x*edge2.y-edge2.x*edge1.y;

    a2=p1.y-p2.y;
    b2=p2.x-p1.x;
    c2=p1.x*p2.y-p2.x*p1.y;

    d=a1*b2-a2*b1;
    dx=-c1*b2+c2*b1;
    dy=-a1*c2+a2*c1;

    return{dx/d,dy/d};
}


int isLeft(Point edge1, Point edge2, Point p)
{
    if (((edge2.y - edge1.y) * p.x + (edge1.x - edge2.x) * p.y + (edge2.x * edge1.y - edge1.x * edge2.y)) < 0)
        return 0;
    else
        return 1;
}

// checks where the coordinates of a line are
int skalaComputeCode(double x, double y, int xmin, int xmax, int ymin, int ymax)
{
    // initialized as being inside
    int code = SKALA_INSIDE;

    if (x < xmin)   // to the left of rectangle
        code = SKALA_LEFT;
    else if (x > xmax)  // to the right of rectangle
        code = SKALA_RIGHT;
    if (y < ymin)   // below the rectangle
        code |= SKALA_BOTTOM;
    else if (y > ymax)  // above the rectangle
        code |= SKALA_TOP;

    return code;
}

struct Line skalaClip(double x1, double y1, double x2, double y2, float xmin, float xmax, float ymin, float ymax)
{
    Line final;

    Point a,b,polygon[4];

    polygon[0] = {xmin, ymin};
    polygon[1] = {xmax, ymin}; 
    polygon[2] = {xmax, ymax};
    polygon[3] = {xmin, ymax};

    a.x=x1;
    a.y=y1;
    b.x=x2;
    b.y=y2;

    int vertex[4];
    
    vertex[0]=isLeft(a,b,polygon[0]);
    vertex[1]=isLeft(a,b,polygon[1]);
    vertex[2]=isLeft(a,b,polygon[2]);
    vertex[3]=isLeft(a,b,polygon[3]);


    int c=vertex[3]*8+vertex[2]*4+vertex[1]*2+vertex[0];

    
    if(tab1[c]!=-1 && tab2[c]!=-1)
    {
        //  Compute region codes for P1, P2
        int code1 = skalaComputeCode(x1, y1, xmin, xmax, ymin, ymax);
        int code2 = skalaComputeCode(x2, y2, xmin, xmax, ymin, ymax);

        if ((code1 | code2) == 0)
        {
            //if both endpoints lie within rectangle
            final.x1 = x1;
            final.x2 = x2;
            final.y1 = y1;
            final.y2 = y2;
            return final;
        }
        if ((code1 & code2)!=0)
        {
            final.x1 = 0;
            final.x2 = 0;
            final.y1 = 0;
            final.y2 = 0;
            return final;
        }

        if (c==0||c==15)
        {
            final.x1 = 0;
            final.x2 = 0;
            final.y1 = 0;
            final.y2 = 0;
            return final;
        }

        int i=tab1[c],j=tab2[c];

        if(code1!=0 && code2!=0)
        {
            Point newA=crossProduct(a,b,polygon[i],polygon[(i+1)%4]);
            Point newB=crossProduct(a,b,polygon[j],polygon[(j+1)%4]);
            final.x1=newA.x;
            final.y1=newA.y;
            final.x2=newB.x;
            final.y2=newB.y;
            return final;
        }
        else
        {
            if(code1==0)
            {
                Point newB;
                if ((code2 & mask[c])!=0)
                {
                    newB=crossProduct(a,b,polygon[i],polygon[(i+1)%4]);
                }
                else
                {
                    newB=crossProduct(a,b,polygon[j],polygon[(j+1)%4]);
                }
                final.x1=x1;
                final.y1=y1;
                final.x2=newB.x;
                final.y2=newB.y;
                return final;
            }
            else if(code2==0)
            {
                Point newA;
                if ((code1 & mask[c])!=0)
                {
                    newA=crossProduct(a,b,polygon[i],polygon[(i+1)%4]);
                }
                else
                {
                    newA=crossProduct(a,b,polygon[j],polygon[(j+1)%4]);
                }
                final.x1=newA.x;
                final.y1=newA.y;
                final.x2=x2;
                final.y2=y2;
                return final;
            }
        }
    }
}


void skalaClipLines(Line *lines, int xmin, int xmax, int ymin, int ymax, int numberOfLines)
{
    int i;
    struct Line *final = createLineArray(numberOfLines);

    for (i = 0; i < numberOfLines; i++)
    {
        //printf("Line %d: (%f, %f) -> (%f, %f)\n", i, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
        final[i] = skalaClip(lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2, xmin, xmax, ymin, ymax);
        //printf("Line %d: (%f, %f) -> (%f, %f)\n", i, final[i].x1, final[i].y1, final[i].x2, final[i].y2);
    }

    drawLines(final, numberOfLines);
}