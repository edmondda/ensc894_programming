#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
#include "trackball.h"

#define WIDTH 4
#define HEIGHT 5


void changeState(void);

int curX, curY, visible;

static float curquat[4];
static int doubleBuffer = 1;
static int height = 2;

static int W = 800, H = 600;
static GLint viewport[4];

static int left_mouse, middle_mouse;
static int mousex, mousey;
static int solving;
static int spinning;
static float lastquat[4];
static int sel_piece;

static void
Reshape(int width, int depth)
{

  W = width;
  H = depth;
  glViewport(0, 0, W, H);
  glGetIntegerv(GL_VIEWPORT, viewport);
}



  void drawBackWall(float x1, float x2, float y1, float y2, float z_pos, float thickness) {

    glNormal3f(0, 0, -1);
    glVertex3f(x1, y1, z_pos);
    glVertex3f(x2, y1, z_pos);
    glVertex3f(x2, y2, z_pos);
    glVertex3f(x1, y2, z_pos);
    glNormal3f(0, 0, 1);
    glVertex3f(x1, y1, thickness + z_pos);
    glVertex3f(x2, y1, thickness + z_pos);
    glVertex3f(x2, y2 - thickness, thickness + z_pos);
    glVertex3f(x1, y2 - thickness, thickness + z_pos);
    glNormal3f(0, -1, 0);
    glVertex3f(x1, y1, z_pos);
    glVertex3f(x2, y1, z_pos);
    glVertex3f(x2, y1, z_pos + thickness);
    glVertex3f(x1, y1, z_pos + thickness);


    glNormal3f(-1, 0, 0);
    glVertex3f(x1, y1, z_pos);
    glVertex3f(x1, y2, z_pos);
    glVertex3f(x1, y2, z_pos + thickness);
    glVertex3f(x1, y1, z_pos + thickness);

    glNormal3f(1, 0, 0);
    glVertex3f(x2, y1, z_pos);
    glVertex3f(x2, y2, z_pos);
    glVertex3f(x2, y2, z_pos + thickness);
    glVertex3f(x2, y1, z_pos + thickness);


  }

  void drawRightWall(float z1, float z2, float y1, float y2, float x_pos, float thickness) {

    /* right wall */
    glNormal3f(-1, 0, 0);
    glVertex3f(x_pos, y1, z1);
    glVertex3f(x_pos, y1, z2);
    glVertex3f(x_pos, y2, z2);
    glVertex3f(x_pos, y2, z1);
    glNormal3f(1, 0, 0);
    glVertex3f(x_pos + thickness, y1, z1);
    glVertex3f(x_pos + thickness, y1, z2);
    glVertex3f(x_pos + thickness, y2 - thickness, z2);
    glVertex3f(x_pos + thickness, y2 - thickness, z1);


    glNormal3f(0, 0, -1);
    glVertex3f(x_pos, y1, z1);
    glVertex3f(x_pos, y2, z1);
    glVertex3f(x_pos + thickness, y2, z1);
    glVertex3f(x_pos + thickness, y1, z1);


    glNormal3f(0, -1, 0);
    glVertex3f(x_pos, y1, z2);
    glVertex3f(x_pos, y1, z1);
    glVertex3f(x_pos + thickness, y1, z1);
    glVertex3f(x_pos + thickness, y1, z2);

    glNormal3f(0, 0, 1);
    glVertex3f(x_pos, y1, z2);
    glVertex3f(x_pos, y2, z2);
    glVertex3f(x_pos + thickness, y2, z2);
    glVertex3f(x_pos + thickness, y1, z2);

}



  void drawDoor(float z1, float z2, float y1, float y2, float x1, float x2) {

    /* right wall */
    glNormal3f(-1, 0, 0);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z2);
    glVertex3f(x1, y2, z2);
    glVertex3f(x1, y2, z1);

    glNormal3f(1, 0, 0);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x2, y2, z1);

    glNormal3f(0, 0, -1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y2, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x2, y1, z1);

    glNormal3f(0, -1, 0);
    glVertex3f(x1, y1, z2);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y1, z2);

    glNormal3f(0, 1, 0);
    glVertex3f(x1, y2, z2);
    glVertex3f(x1, y2, z1);
    glVertex3f(x2, y2, z1);
    glVertex3f(x2, y2, z2);

    glNormal3f(0, 0, 1);
    glVertex3f(x1, y1, z2);
    glVertex3f(x1, y2, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x2, y1, z2);

}

void drawFloor(float x1, float x2, float z1, float z2, float y_pos, float thickness) {

  glNormal3f(0, 1, 0);
  glVertex3f(x1, y_pos, z1);
  glVertex3f(x1, y_pos, z2);
  glVertex3f(x2, y_pos, z2);
  glVertex3f(x2, y_pos, z1);
  glNormal3f(0, -1, 0);
  glVertex3f(x1, y_pos - thickness, z1);
  glVertex3f(x1, y_pos - thickness, z2);
  glVertex3f(x2, y_pos - thickness, z2);
  glVertex3f(x2, y_pos - thickness, z1);

  glNormal3f(0, 0, 1);
  glVertex3f(x1, y_pos, z2);
  glVertex3f(x2, y_pos, z2);
  glVertex3f(x2 - thickness, y_pos - thickness, z2);
  glVertex3f(x1 + thickness, y_pos - thickness, z2);


}

/* left wall */
void drawLeftWall(float z1, float z2, float y1, float y2, float x_pos, float thickness) {

  glNormal3f(1, 0, 0);
  glVertex3f(x_pos, y1, z1);
  glVertex3f(x_pos, y2, z1);
  glVertex3f(x_pos, y2, z2);
  glVertex3f(x_pos, y1, z2);
  glNormal3f(-1, 0, 0);
  glVertex3f(x_pos - thickness, y1, z1);
  glVertex3f(x_pos - thickness, y2 - thickness, z1);
  glVertex3f(x_pos - thickness, y2 - thickness, z2);
  glVertex3f(x_pos - thickness, y1, z2);


  glNormal3f(0, 0, -1);
  glVertex3f(x_pos, y1, z1);
  glVertex3f(x_pos, y2, z1);
  glVertex3f(x_pos - thickness, y2, z1);
  glVertex3f(x_pos - thickness, y1, z1);


  glNormal3f(0, -1, 0);
  glVertex3f(x_pos, y1, z2);
  glVertex3f(x_pos, y1, z1);
  glVertex3f(x_pos - thickness, y1, z1);
  glVertex3f(x_pos - thickness, y1, z2);

  /* front trim */
  glNormal3f(0, 0, 1);
  glVertex3f(x_pos, y2, z2);
  glVertex3f(x_pos, y1, z2);
  glVertex3f(x_pos - thickness, y1, z2);
  glVertex3f(x_pos - thickness, y2, z2);

}


void DrawCube(void)
{

  glBegin(GL_QUADS);        // Draw The Cube Using quads
    glNormal3f(0, 1, 0);
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Bottom Right Of The Quad (Top)

    glNormal3f(0, -1, 0);
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Bottom)

    glNormal3f(0, 0, 1);
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Front)

    glNormal3f(0, 0, -1);
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Top Right Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Top Left Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Bottom Right Of The Quad (Back)

    glNormal3f(-1, 0, 0);
    glVertex3f(-1.0f, 1.0f, 1.0f);    // Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);    // Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);    // Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);    // Bottom Right Of The Quad (Left)

    glNormal3f(1, 0, 0);
    glVertex3f( 1.0f, 1.0f,-1.0f);    // Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);    // Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);    // Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);    // Bottom Right Of The Quad (Right)
  glEnd();            // End Drawing The Cube

}



void drawHumanoid(GLUquadric *quadric, double larm, double rarm, double lleg, double rleg) {

          // left leg
          glPushMatrix();
              glTranslatef(0.1f,0.0f,0.0f);
              // -90 is down, can adjust for forward and back movement
              glRotatef(-90 + lleg, 1.0f, 0.0f, 0.0f);
              glRotatef(8, 0.0f, 1.0f, 0.0f);
              //glRotatef(60.0f, 0.0f, 0.0f, 1.0f);        //put the cylinder horizontal

              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);
              gluCylinder(quadric, 0.06, 0.06, 0.8, 20, 20);
          glPopMatrix();

          // right leg
          glPushMatrix();
              glTranslatef(-0.1f,0.0f,0.0f);
              // -90 is down, can adjust for forward and back movement
              glRotatef(-90 + rleg, 1.0f, 0.0f, 0.0f);
              glRotatef(-8, 0.0f, 1.0f, 0.0f);

              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);
              gluCylinder(quadric, 0.06, 0.06, 0.8, 20, 20);
          glPopMatrix();

          // hips
          gluQuadricDrawStyle(quadric, GLU_FILL);
          gluSphere(quadric, 0.15f, 20, 20);

          //torso
          glPushMatrix();
              glTranslatef(0.0f, -0.8f,0.0f);
              glRotatef(-90, 1.0f, 0.0f, 0.0f);
              glRotatef(-0, 0.0f, 1.0f, 0.0f);

              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);
              gluCylinder(quadric, 0.1, 0.1, 0.8, 20, 20);

          glPopMatrix();

          //shoulders
          glPushMatrix();

              glTranslatef(0.2f, -0.5f,0.0f);

              glRotatef(-90, 1.0f, 0.0f, 0.0f);
              glRotatef(-90, 0.0f, 1.0f, 0.0f);
              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);
              gluCylinder(quadric, 0.1, 0.1, 0.4, 20, 20);

          glPopMatrix();

          glPushMatrix();
              glTranslatef(0.2f, -0.5f,0.0f);
              gluSphere(quadric, 0.1f, 20, 20);
          glPopMatrix();

          glPushMatrix();
              glTranslatef(-0.2f, -0.5f,0.0f);
              gluSphere(quadric, 0.1f, 20, 20);
          glPopMatrix();

          // head
          glPushMatrix();
              glTranslatef(0.0f, -0.8f,0.0f);
              gluSphere(quadric, 0.2f, 20, 20);
          glPopMatrix();

          // left arm
          glPushMatrix();
              glTranslatef(0.2f,-0.5f,0.0f);
              // side to side
              // -90 is down, can adjust for forward and back movement
              glRotatef(-90 + larm, 1.0f, 0.0f, 0.0f);
              glRotatef(8, 0.0f, 1.0f, 0.0f);

              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);
              gluCylinder(quadric, 0.06, 0.06, 0.5, 20, 20);
          glPopMatrix();

          // right arm
          glPushMatrix();                       //save cylinder center
              glTranslatef(-0.2f,-0.5f,0.0f);
              // -90 is down, can adjust for forward and back movement
              glRotatef(-90 + rarm, 1.0f, 0.0f, 0.0f);
              glRotatef(-8, 0.0f, 1.0f, 0.0f);

              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);
              gluCylinder(quadric, 0.06, 0.06, 0.5, 20, 20);
          glPopMatrix();

}


void
drawAll(void)
{
  int i, j;
  float m[4][4];

  build_rotmatrix(m, curquat);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -12);
  glMultMatrixf(&(m[0][0]));
  glRotatef(180, 0, 0, 1);

  if (height) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  glLoadName(0);
  //drawContainer();


  float doorx = 1.0;
  float doorz = 0.0;


  float depth = 8.0;
  float width = 10.0;
  float height = 3.0;
  float thickness = 0.1;
  float door_width = 1;
  float door_height = 2.5;

  float x_offset = 0.0;
  float z_offset = 0.0;


  float baseRadius = 0.4;
  float topRadius  = 0.6;
  float clyHeight     = 2.5;
  int   slices     = 40;
  int   stacks     = 40;
  float a          = 60;

  glBegin(GL_QUADS);

  glColor3f(1,1,0);

  // main perimeter
  drawBackWall(-width/2, width/2, -height/2, height/2, -depth/2, thickness);
  drawRightWall(-depth/2, depth/2, -height/2, height/2, -width/2, thickness);
  drawLeftWall(-depth/2, depth/2, -height/2, height/2, width/2, thickness);

  //bathroom wall
  drawBackWall(-width/2, -width/4, -height/2, height/2, -depth/8, thickness);
  // door
  drawBackWall(-width/4 + door_width, 0, -height/2, height/2, -depth/8, thickness);


  drawLeftWall(-depth/2, depth/4, -height/2, height/2, 0, thickness);
  // door
  drawLeftWall(depth/4 + door_width, depth/2, -height/2, height/2, 0, thickness);

  glColor3f(0.5f, 0.35f, 0.05f);

  drawFloor(-width/2, width/2, -depth/2, depth/2, height/2, thickness);

  //New quadric object
  GLUquadric *quadric = gluNewQuadric();

  // dummy
  glPushMatrix();
      glRotatef(90, 0.0f, 1.0f, 0.0f);
      glRotatef(-15, 0.0f, 1.0f, 0.0f);
      glTranslatef(-0.0f,0.0f,0.0f);
      glRotatef(45.f, 0.0f, 1.0f, 0.0f);
      //Cylinder render (fill or line)
      gluQuadricDrawStyle(quadric, GLU_FILL);
      gluCylinder(quadric, 0.06, 0.06, 0.03, 20, 20);
  glPopMatrix();


  // living room
  glColor3f(0,1,0);
  glPushMatrix();
    glTranslatef(3.0f, 0.8f, 2.0f);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    drawHumanoid(quadric, 0, 0, 45, 45);

    glTranslatef(0.0f, 0.6f - 0.01, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluDisk(quadric, 0, 0.5, 40, 1);
  glPopMatrix();

  // draw path from living room -> kitchen
  glLineWidth(5);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
    glVertex3f(3.0, 1.4, 2.0);
    glVertex3f(3.5, 1.4, 2.0);

    glVertex3f(3.5, 1.4, 2.0);
    glVertex3f(3.5, 1.4, -1.0);
  glEnd();


  // kitchen humanoid
  glColor3f(1,0,0);
  glPushMatrix();
    glTranslatef(3.5f, 0.7f, -1.0f);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    drawHumanoid(quadric, 60, 0, 0, 0);

    glTranslatef(0.0f, 0.7f - 0.01, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluDisk(quadric, 0, 0.5, 40, 1);
  glPopMatrix();

  // draw path from kitchen -> walking
  glLineWidth(5);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
  glVertex3f(3.5, 1.4, -1.0);
  glVertex3f(1.0, 1.4, 1.0);
  glEnd();

  // walking
  glColor3f(1,1,0);
  glPushMatrix();
    glTranslatef(1.0f, 0.7f, 1.0f);
    glRotatef(0, 0.0f, 1.0f, 0.0f);
    drawHumanoid(quadric, 30, -30, 0, 0);

    glTranslatef(0.0f, 0.7f - 0.01, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluDisk(quadric, 0, 0.5, 40, 1);
  glPopMatrix();

  // draw path from walking -> bed
  glLineWidth(5);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
    glVertex3f(1.0, 1.4, 1.0);
    glVertex3f(1.0, 1.4, 2.5);

    glVertex3f(1.0, 1.4, 2.5);
    glVertex3f(-1.0, 1.4, 2.5);

    glVertex3f(-1.0, 1.4, 2.5);
    glVertex3f(-1.0, 1.4, 0);

    glVertex3f(-1.0, 1.4, 0);
    glVertex3f(-3.5, 1.4, 0);

    glVertex3f(-3.5, 1.4, 0);
    glVertex3f(-3.5, 1.4, 1);
  glEnd();

  // bedroom humanoid
  glColor3f(0,0,1);
  glPushMatrix();
    glTranslatef(-3.5f, 0.7f, 1.0f);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    drawHumanoid(quadric, 0, 0, 0, 0);

    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.2f - 0.01, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluDisk(quadric, 0, 0.5, 40, 1);

  glPopMatrix();

  // draw path from bed -> bathroom
  glLineWidth(5);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINES);
    glVertex3f(-3.5, 1.4, 1);
    glVertex3f(-3.5, 1.4, 0);

    glVertex3f(-3.5, 1.4, 0);
    glVertex3f(-2.0, 1.4, 0);

    glVertex3f(-2.0, 1.4, 0);
    glVertex3f(-2.0, 1.4, -2);

    glVertex3f(-2.0, 1.4, -2);
    glVertex3f(-4.5, 1.4, -2);

  glEnd();


  // bathroom humanoid
  glColor3f(0,1,1);

  glPushMatrix();
    glTranslatef(-4.25f, 0.7f, -2.0f);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    drawHumanoid(quadric, 60, 30, 45, 45);

    glTranslatef(0.0f, 0.6f - 0.01, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    gluDisk(quadric, 0, 0.5, 40, 1);
  glPopMatrix();


  // draw kitchen counters
  glColor3f(0.623529, 0.623529, 0.372549);
  glPushMatrix();
    glTranslatef(width/2 - thickness - 0.5, height/2 - thickness - 0.5, -depth/2 + thickness + 2.0 );
    glScalef(0.5, 0.5, 2);
    DrawCube();
  glPopMatrix();
  glPushMatrix();
    glTranslatef(width/2 - thickness - 2.0, height/2 - thickness - 0.5, -depth/2 + thickness + 0.5 );
    glRotatef(90, 0.0f, 1.0f, 0.0f);  //cylinder rotation
    glScalef(0.5, 0.5, 2);
    DrawCube();
  glPopMatrix();
  // batchroom counter
  glPushMatrix();
    glTranslatef( -1.5 - thickness, height/2 - thickness - 0.5, -depth/2 + thickness + 0.5 );
    glRotatef(90, 0.0f, 1.0f, 0.0f);  //cylinder rotation
    glScalef(0.5, 0.5, 1.5);
    DrawCube();
  glPopMatrix();
  // sofa
  glColor3f(0.258824, 0.435294, 0.258824);
  glPushMatrix();
    glTranslatef(width/4, height/2 - thickness - 0.25, 2);
    glScalef(0.5, 0.25, 1);
    DrawCube();
  glPopMatrix();
  glPushMatrix();
    glTranslatef(width/4 - 0.35, height/2 - thickness - 0.75, 2);
    glScalef(0.15, 0.25, 1);
    DrawCube();
  glPopMatrix();


  // draw bed
  glColor3f(0.258824, 0.435294, 0.258824);
  glPushMatrix();
    glTranslatef(-width/2 + thickness + 1.5, height/2 - thickness - 0.25, 1 + 0.5);
    glScalef(1.5, 0.25, 1);
    DrawCube();
  glPopMatrix();


  glColor3f(0.752941, 0.752941, 0.752941);
  glPushMatrix();
    glTranslatef(-width/2 + thickness + 0.5, height/2 - thickness - 0.5, 0.1);
    glScalef(0.3, 0.3, 0.3);
    DrawCube();
  glPopMatrix();

  glPushMatrix();
    glTranslatef(-width/2 + thickness + 0.5, height/2 - thickness - 0.5, 3 -0.1);
    glScalef(0.3, 0.3, 0.3);
    DrawCube();
  glPopMatrix();

  // draw toilet


  // draw table

  // draw fridge

  // draw counter

  // draw toilet

  // draw shower

  glEnd();

}

void reset(void)
{
    glutPostRedisplay();
}


void
redraw(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1.0, 0.1, 100.0);

  drawAll();

  if (doubleBuffer)
    glutSwapBuffers();
  else
    glFinish();
}



/*
   ** This is a screwball function.  What it does is the following:
   ** Given screen x and y coordinates, compute the corresponding object space
   **   x and y coordinates given that the object space z is 0.9 + OFFSETZ.
   ** Since the tops of (most) pieces are at z = 0.9 + OFFSETZ, we use that
   **   number.
 */

void
motion(int x, int y)
{
  float selx, sely;

  if (middle_mouse || left_mouse) {
    if (mousex != x || mousey != y) {
      trackball(lastquat,
        (2.0*mousex - W) / W,
        (H - 2.0*mousey) / H,
        (2.0*x - W) / W,
        (H - 2.0*y) / H);
      spinning = 1;
    } else {
      spinning = 0;
    }
    changeState();
  } else {
    //computeCoords(sel_piece, x, y, &selx, &sely);
    //moveSelection(selx, sely);
  }
  mousex = x;
  mousey = y;
  glutPostRedisplay();
}


void
multMatrices(const GLfloat a[16], const GLfloat b[16], GLfloat r[16])
{
  int i, j;

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      r[i * 4 + j] =
        a[i * 4 + 0] * b[0 * 4 + j] +
        a[i * 4 + 1] * b[1 * 4 + j] +
        a[i * 4 + 2] * b[2 * 4 + j] +
        a[i * 4 + 3] * b[3 * 4 + j];
    }
  }
}

void
makeIdentity(GLfloat m[16])
{
  m[0 + 4 * 0] = 1;
  m[0 + 4 * 1] = 0;
  m[0 + 4 * 2] = 0;
  m[0 + 4 * 3] = 0;
  m[1 + 4 * 0] = 0;
  m[1 + 4 * 1] = 1;
  m[1 + 4 * 2] = 0;
  m[1 + 4 * 3] = 0;
  m[2 + 4 * 0] = 0;
  m[2 + 4 * 1] = 0;
  m[2 + 4 * 2] = 1;
  m[2 + 4 * 3] = 0;
  m[3 + 4 * 0] = 0;
  m[3 + 4 * 1] = 0;
  m[3 + 4 * 2] = 0;
  m[3 + 4 * 3] = 1;
}

/*
   ** inverse = invert(src)
 */
int
invertMatrix(const GLfloat src[16], GLfloat inverse[16])
{
  int i, j, k, swap;
  double t;
  GLfloat temp[4][4];

  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      temp[i][j] = src[i * 4 + j];
    }
  }
  makeIdentity(inverse);

  for (i = 0; i < 4; i++) {
    /*
       ** Look for largest element in column */
    swap = i;
    for (j = i + 1; j < 4; j++) {
      if (fabs(temp[j][i]) > fabs(temp[i][i])) {
        swap = j;
      }
    }

    if (swap != i) {
      /*
         ** Swap rows. */
      for (k = 0; k < 4; k++) {
        t = temp[i][k];
        temp[i][k] = temp[swap][k];
        temp[swap][k] = t;

        t = inverse[i * 4 + k];
        inverse[i * 4 + k] = inverse[swap * 4 + k];
        inverse[swap * 4 + k] = t;
      }
    }
    if (temp[i][i] == 0) {
      /*
         ** No non-zero pivot.  The matrix is singular, which
         shouldn't ** happen.  This means the user gave us a
         bad matrix. */
      return 0;
    }
    t = temp[i][i];
    for (k = 0; k < 4; k++) {
      temp[i][k] /= t;
      inverse[i * 4 + k] /= t;
    }
    for (j = 0; j < 4; j++) {
      if (j != i) {
        t = temp[j][i];
        for (k = 0; k < 4; k++) {
          temp[j][k] -= temp[i][k] * t;
          inverse[j * 4 + k] -= inverse[i * 4 + k] * t;
        }
      }
    }
  }
  return 1;
}


void
mouse(int b, int s, int x, int y)
{
  float selx, sely;

  mousex = x;
  mousey = y;
  curX = x;
  curY = y;
  if (s == GLUT_DOWN) {
    switch (b) {
    case GLUT_LEFT_BUTTON:
      // if (solving) {
      //   freeSolutions();
      //   solving = 0;
      // glutChangeToMenuEntry(1, "Solving", 1);
      //   glutSetWindowTitle("glpuzzle");
      //   movingPiece = 0;
      // }
      left_mouse = GL_TRUE;
      // sel_piece = selectPiece(mousex, mousey);
      // if (computeCoords(sel_piece, mousex, mousey, &selx, &sely)) {
      //   grabPiece(sel_piece, selx, sely);
      // }
      glutPostRedisplay();
      break;
    case GLUT_MIDDLE_BUTTON:
      middle_mouse = GL_TRUE;
      glutPostRedisplay();
      break;
    }
  } else {
    switch (b) {
    case GLUT_LEFT_BUTTON:
      left_mouse = GL_FALSE;
      // dropSelection();
      glutPostRedisplay();
      break;
    case GLUT_MIDDLE_BUTTON:
      middle_mouse = GL_FALSE;
      glutPostRedisplay();
      break;
    }
  }
  motion(x, y);
}



void
animate(void)
{
  if (spinning) {
    add_quats(lastquat, curquat, curquat);
  }
  glutPostRedisplay();
  // if (solving) {
  //   if (!continueSolving()) {
  //     solving = 0;
  //     glutChangeToMenuEntry(1, "Solving", 1);
  //     glutSetWindowTitle("glpuzzle");
  //   }
  // }
  if (!solving && !spinning && !visible) {
    glutIdleFunc(NULL);
  }
}

void
changeState(void)
{
  if (visible) {
    if (!solving && !spinning) {
      glutIdleFunc(NULL);
    } else {
      glutIdleFunc(animate);
    }
  } else {
    glutIdleFunc(NULL);
  }
}

void
init(void)
{
  static float lmodel_ambient[] =
  {0.0, 0.0, 0.0, 0.0};
  static float lmodel_twoside[] =
  {GL_FALSE};
  static float lmodel_local[] =
  {GL_FALSE};
  static float light0_ambient[] =
  {0.1, 0.1, 0.1, 1.0};
  static float light0_diffuse[] =
  {1.0, 1.0, 1.0, 0.0};
  static float light0_position[] =
  {0.8660254, 0.5, 1, 0};
  static float light0_specular[] =
  {0.0, 0.0, 0.0, 0.0};
  static float bevel_mat_ambient[] =
  {0.0, 0.0, 0.0, 1.0};
  static float bevel_mat_shininess[] =
  {40.0};
  static float bevel_mat_specular[] =
  {0.0, 0.0, 0.0, 0.0};
  static float bevel_mat_diffuse[] =
  {1.0, 0.0, 0.0, 0.0};

  // glEnable(GL_CULL_FACE);
  // glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0);

  glClearColor(0.5, 0.5, 0.5, 0.0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glEnable(GL_LIGHT0);

  glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
  glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glEnable(GL_LIGHTING);

  glMaterialfv(GL_FRONT, GL_AMBIENT, bevel_mat_ambient);
  glMaterialfv(GL_FRONT, GL_SHININESS, bevel_mat_shininess);
  glMaterialfv(GL_FRONT, GL_SPECULAR, bevel_mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, bevel_mat_diffuse);

  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glShadeModel(GL_FLAT);

  trackball(curquat, 0.0, 0.0, 0.0, 0.0);
  srandom(time(NULL));
}


void
visibility(int v)
{
  if (v == GLUT_VISIBLE) {
    visible = 1;
  } else {
    visible = 0;
  }
  changeState();
}

void
menu(int choice)
{
   switch(choice) {
   case 1:
      //toggleSolve();
      break;
   case 2:
      reset();
      break;
   case 3:
      exit(0);
      break;
   }
}


int
main(int argc, char **argv)
{

  long i;

  glutInit(&argc, argv);

  glutInitWindowSize(W, H);
  if (doubleBuffer) {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  } else {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
  }

  glutCreateWindow("room");

  init();

  printf("\n");
  printf("Use left mouse to change viewing angle\n");

  glGetIntegerv(GL_VIEWPORT, viewport);

  glutReshapeFunc(Reshape);
  glutDisplayFunc(redraw);
  //glutKeyboardFunc(keyboard);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  glutVisibilityFunc(visibility);


  glutMainLoop();
  return 0;


}
