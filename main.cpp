#include "lib.h"

int main_window;
int viewportRow = 0, viewportColumn = 0;
int xmin = -200, xmax = 200, ymin = -200, ymax = 200, numberOfLines = 5, option = 1;
struct Line *lines = createLineArray(numberOfLines);
int last_x, last_y; // den xreiazontai kapou gia twra

GLUI *glui_subwin;
GLUI_Button *begin_button, *quit_button, *reset_button;
GLUI_Panel *panel_coordinates_min, *panel_coordinates_max, *panel_numberOfLines;
GLUI_EditText *text_xmin, *text_xmax, *text_ymin, *text_ymax, *text_num_of_lines;

// drawing the stipple-lined axes and the middle-of-the-window axis
void drawAxes()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(2);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xFF00);
    glColor3ub(5, 161, 114);
    glBegin(GL_LINES);
    glVertex2i(xmin, TOP_BORDER);
    glVertex2i(xmin, -TOP_BORDER);
    glVertex2i(xmax, TOP_BORDER);
    glVertex2i(xmax, -TOP_BORDER);
    glVertex2i(RIGHT_BORDER, ymin);
    glVertex2i(-RIGHT_BORDER, ymin);
    glVertex2i(RIGHT_BORDER, ymax);
    glVertex2i(-RIGHT_BORDER, ymax);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glPopAttrib();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(1);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xFF00);
    glColor3ub(100, 100, 100);
    glBegin(GL_LINES);
    glVertex2i(-RIGHT_BORDER, 0);
    glVertex2i(RIGHT_BORDER, 0);
    glVertex2i(0, -TOP_BORDER);
    glVertex2i(0, TOP_BORDER);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glPopAttrib();
}

void drawViewportBorder()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glLineWidth(1);
    glColor3ub(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(-RIGHT_BORDER, TOP_BORDER);
    glVertex2d(RIGHT_BORDER, TOP_BORDER);
    glVertex2d(RIGHT_BORDER, -TOP_BORDER);
    glVertex2d(-RIGHT_BORDER, -TOP_BORDER);
    glEnd();
    glPopAttrib();
}

// drawing clipping rectangle
void drawRect(int xmin, int xmax, int ymin, int ymax)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    glColor3ub(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2i(xmin, ymin);
    glVertex2i(xmin, ymax);
    glVertex2i(xmax, ymax);
    glVertex2i(xmax, ymin);
    glEnd();
    glPopMatrix();
    glPopAttrib();
}

void typeText(int x, int y, char *txt)
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glColor3ub(161, 5, 52);
    glRasterPos2f(x, y);
    int len, i;
    len = (int)strlen(txt);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, txt[i]);
    glPopAttrib();
}

void normalKeysPressed(unsigned char key, int x, int y)
{
    if (key == 27)
        free(lines);
    exit(0);
    glutPostRedisplay();

}

void mouse(int button, int button_state, int x, int y)
{ 

}

void reshape(int width, int height)
{
    if (height == 0)
        height = 1;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int x, y, w, h;
    GLUI_Master.get_viewport_area(&x, &y, &w, &h);
    glViewport(viewportColumn * w / 2, (viewportRow * h) + (glutGet(GLUT_WINDOW_HEIGHT) - h), w / 2, h);
    gluOrtho2D(-RIGHT_BORDER, RIGHT_BORDER, -TOP_BORDER, TOP_BORDER);
    glMatrixMode(GL_MODELVIEW);
    drawViewportBorder();
    drawAxes();
}

void clippedLines() // display2
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    viewportRow = 0; viewportColumn = 0;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    drawAxes();
    csClipLines(lines, xmin, xmax, ymin, ymax, numberOfLines);
    drawRect(xmin, xmax, ymax, ymin);    
    typeText(-(RIGHT_BORDER - 250), (TOP_BORDER - 40), "Cohen-Sutherland Line Clipping");


    viewportRow = 0;viewportColumn = 1;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    drawAxes();
    drawRect(xmin, xmax, ymax, ymin);    
    skalaClipLines(lines, xmin, xmax, ymin, ymax, numberOfLines);
    typeText(-(RIGHT_BORDER - 300), (TOP_BORDER - 40), "Skala Line Clipping");
    
    glutSwapBuffers();
}

void display() //draws lines 
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    viewportRow = 0; viewportColumn = 0;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    drawAxes();
    drawLines(lines, numberOfLines);
    drawRect(xmin, xmax, ymax, ymin);
    typeText(-(RIGHT_BORDER - 250), (TOP_BORDER - 40), "Cohen-Sutherland Line Clipping");


    viewportRow = 0; viewportColumn = 1;
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    drawAxes();
    drawLines(lines, numberOfLines);
    drawRect(xmin, xmax, ymax, ymin);
   
    typeText(-(RIGHT_BORDER - 300), (TOP_BORDER - 40), "Skala Line Clipping");

    glutSwapBuffers();
}

bool flag;

void gluiCallbacks(int control)
{
    if (control == 0) // begin clipping
    {
        switch (flag)
        {
        case true:
            display();
            flag = false;
            break;
        case false:
            clippedLines();
            break;
        }
    }
    if (control == 1) // redraw lines
    {
        display();
        flag = false;
    }
    if (control == 2)
        text_xmin->set_int_val(xmin);
    if (control == 3)
        text_ymin->set_int_val(ymin);
    if (control == 4)
        text_xmax->set_int_val(xmax);
    if (control == 5)
        text_ymax->set_int_val(ymax);
    if (control == 6)
    {
        text_num_of_lines->set_int_val(numberOfLines);
        free(lines);
        lines = createLineArray(numberOfLines);
        flag = true;
    }
    if (control == 7) // exit
        exit(0);
}

void gluiSetup()
{
    GLUI_Master.set_glutDisplayFunc(display);
    GLUI_Master.set_glutMouseFunc(mouse);
    GLUI_Master.set_glutKeyboardFunc(normalKeysPressed);
    GLUI_Master.set_glutReshapeFunc(reshape);

    glui_subwin = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_BOTTOM);
    glui_subwin->set_main_gfx_window(main_window);

    panel_coordinates_min = glui_subwin->add_panel("Clipping Window min coordinates:");
    text_xmin = glui_subwin->add_edittext_to_panel(panel_coordinates_min, "xmin:", GLUI_EDITTEXT_INT, &xmin, 2, gluiCallbacks);
    text_ymin = glui_subwin->add_edittext_to_panel(panel_coordinates_min, "ymin:", GLUI_EDITTEXT_INT, &ymin, 3, gluiCallbacks);

    glui_subwin->add_column(false);

    panel_coordinates_max = glui_subwin->add_panel("Clipping Window max coordinates:");
    text_xmax = glui_subwin->add_edittext_to_panel(panel_coordinates_max, "xmax:", GLUI_EDITTEXT_INT, &xmax, 4, gluiCallbacks);
    text_ymax = glui_subwin->add_edittext_to_panel(panel_coordinates_max, "ymax:", GLUI_EDITTEXT_INT, &ymax, 5, gluiCallbacks);

    text_xmin->set_int_limits(-RIGHT_BORDER, xmax, GLUI_LIMIT_CLAMP);
    text_xmax->set_int_limits(xmin, RIGHT_BORDER, GLUI_LIMIT_CLAMP);
    text_ymin->set_int_limits(-TOP_BORDER, ymax, GLUI_LIMIT_CLAMP);
    text_ymax->set_int_limits(ymin, TOP_BORDER, GLUI_LIMIT_CLAMP);

    glui_subwin->add_column(false);

    panel_numberOfLines = glui_subwin->add_panel("Lines:");
    text_num_of_lines = glui_subwin->add_edittext_to_panel(panel_numberOfLines, "Number of lines:", GLUI_EDITTEXT_INT, &numberOfLines, 6, gluiCallbacks);

    glui_subwin->add_column(false);

    begin_button = glui_subwin->add_button("Begin Clipping", 0, gluiCallbacks);
    reset_button = glui_subwin->add_button("Redraw Lines", 1, gluiCallbacks);
    quit_button = glui_subwin->add_button("Quit", 7, gluiCallbacks);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(900, 650);
    glutInitWindowPosition(100, 100);

    main_window = glutCreateWindow("OpenGL Project 1");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glutDisplayFunc(display);
    gluiSetup();
    glutMainLoop();
    return EXIT_SUCCESS;
}