#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
#include "trackball.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <GL/glui.h>


float angle0 = 0;
float angle1 = 0;
float angle2 = 0;
float px = 0;
float py = 0;
float pz = 1.0;
int angle3 = 0;
int angle4 = 0;
int angle5 = 0;
int   main_window;

/***************************************** myGlutIdle() ***********/

void myGlutIdle()
{
  /* According to the GLUT specification, the current window is
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != main_window )
    glutSetWindow(main_window);

  glutPostRedisplay();
}


void cross_product(const gsl_vector *u, const gsl_vector *v, gsl_vector *product)
{
        double p1 = gsl_vector_get(u, 1)*gsl_vector_get(v, 2)
                - gsl_vector_get(u, 2)*gsl_vector_get(v, 1);

        double p2 = gsl_vector_get(u, 2)*gsl_vector_get(v, 0)
                - gsl_vector_get(u, 0)*gsl_vector_get(v, 2);

        double p3 = gsl_vector_get(u, 0)*gsl_vector_get(v, 1)
                - gsl_vector_get(u, 1)*gsl_vector_get(v, 0);

        gsl_vector_set(product, 0, p1);
        gsl_vector_set(product, 1, p2);
        gsl_vector_set(product, 2, p3);
}


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




// DE - we need a structure here that stores all of the manpulator angles and parameters
//

//struct pumaParam {
//  float theta [6];
//}


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


void drawHalfSphere(int scaley, int scalex, GLfloat r) {
  int i, j;
  GLfloat v[scalex*scaley][3];
  GLfloat v_tip[3];

  for (i=0; i<scalex; ++i) {
    for (j=0; j<scaley; ++j) {
      v[i*scaley+j][0]=r*cos(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
      v[i*scaley+j][1]=r*sin(i*M_PI/(2*scalex));
      v[i*scaley+j][2]=r*sin(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
    }
  }

  glBegin(GL_QUADS);
    for (i=0; i<scalex; ++i) {
      for (j=0; j<scaley; ++j) {
        glVertex3fv(v[i*scaley+j]);
        glVertex3fv(v[i*scaley+(j+1)%scaley]);
        glVertex3fv(v[(i+1)*scaley+(j+1)%scaley]);
        glVertex3fv(v[(i+1)*scaley+j]);
      }
    }
  glEnd();
}


void calcCrossProduct(float * v1, float * v2, float * v3, float * cross) {

  gsl_vector * v[3];
  gsl_vector *product;
  double scale = 0;

  v[0] = gsl_vector_alloc (3);
  v[1] = gsl_vector_alloc (3);
  v[2] = gsl_vector_alloc (3);
  product = gsl_vector_alloc (3);

  gsl_vector_set(v[0],0,v1[0]);
  gsl_vector_set(v[0],1,v1[1]);
  gsl_vector_set(v[0],2,v1[2]);

  gsl_vector_set(v[1],0,v2[0]);
  gsl_vector_set(v[1],1,v2[1]);
  gsl_vector_set(v[1],2,v2[2]);

  gsl_vector_set(v[2],0,v3[0]);
  gsl_vector_set(v[2],1,v3[1]);
  gsl_vector_set(v[2],2,v3[2]);

  gsl_vector_sub (v[1], v[0]);
  gsl_vector_sub (v[0], v[2]);

  cross_product(v[1], v[0], product);

  scale += pow( gsl_vector_get(product, 0), 2 );
  scale += pow( gsl_vector_get(product, 1), 2 );
  scale += pow( gsl_vector_get(product, 2), 2 );
  scale = sqrt( scale );

  gsl_vector_scale(product, 1.0 / scale);

  cross[0] = gsl_vector_get(product, 0);
  cross[1] = gsl_vector_get(product, 1);
  cross[2] = gsl_vector_get(product, 2);
}



void drawModCone(int scaley, GLfloat r, GLfloat h) {
  int i, j;
  GLfloat v[scaley][3];
  GLfloat v_tip[3];
  GLfloat cross[3];

    for (j=0; j<scaley; ++j) {
      v[j][0]=r*cos(j*2*M_PI/scaley);
      v[j][1]=r*sin(j*2*M_PI/scaley);
      v[j][2]=0.0;
      //printf("x = %f, y = %f, z= %f", v[scaley][0], v[scaley][1], v[scaley][2] ) ;
    }


  v_tip[0]=r*cos(0.0*2*M_PI/20.0);
  v_tip[1]=r*sin(0.0*2*M_PI/20.0);
  v_tip[2]=h;

  glBegin(GL_QUADS);
     for (j=0; j<scaley-1; ++j) {
       calcCrossProduct(v[j], v_tip, v[j+1], cross);
       glNormal3f(cross[0], cross[1], cross[2]);
       glVertex3fv(v[j]);
       glVertex3fv(v_tip);
       glVertex3fv(v[j+1]);
       glVertex3fv(v[j]);
     }
     glVertex3fv(v[scaley-1]);
     glVertex3fv(v_tip);
     glVertex3fv(v[0]);
     glVertex3fv(v[scaley-1]);

  glEnd();
}


void solveInverse() {

   double a2 = 1.4;
   double a3 = 0;
   double d3 = 0;
   double d4 = 1.4;
   double alpha23;
   double _int, i, j, k;


   _int = sqrt( pow(px, 2) + pow(py, 2) - pow(d3, 2) );
   angle0 = ( atan2( py, px) - atan2(d3, _int) ) * 180.0 / M_PI ;

   printf("angle0 = %f\n", angle0);

   k = ( pow(px, 2) + pow(py, 2) + pow(px, 2) - pow(a2, 2) - pow(a3, 2) - pow(d3, 2)  - pow(d4, 2) ) / (2*a2);
   _int = sqrt( pow(a3, 2) + pow(d4, 2) - pow(k, 2) );
   angle2 = ( atan2(a3, d4) - atan2(k, _int) ) * 180.0 / M_PI;

   printf("angle2 = %f\n", angle2);


   i =  ( -a3 - a2*cos(angle2 * M_PI / 180) )*pz -
          (cos(angle0 * M_PI / 180) * px + sin(angle0 * M_PI / 180) * py ) * (d4 - a2*sin(angle2 * M_PI / 180)) ;
   j =  ( a2* sin(angle2 * M_PI / 180) - d4 )*pz - (a3 + a2*cos(angle2 * M_PI / 180))*(cos(angle0 * M_PI / 180) * px + sin(angle0 * M_PI / 180) * py );
   alpha23 = atan2( i, j) * 180.0 / M_PI ;
   printf("alpha23 = %f\n", alpha23);

   angle1 = alpha23 - angle2;

   printf("angle1 = %f\n", angle1);

}


void drawPuma(GLUquadric *quadric) {

    float arm_len = 1.4;
    float arm_rad = 0.15;

          // base [0]

          glColor3f(0.5f, 0.5f, 0.5f);
          glRotatef(angle0, 0.0f, 0.0f, 1.0f);
          glPushMatrix();
              glTranslatef(0.0f,0.0f,0.0f);

              //Cylinder render (fill or line)
              gluQuadricDrawStyle(quadric, GLU_FILL);

              gluSphere(quadric, 0.7f, 60, 60);
              glPushMatrix();
                glTranslatef(0.0f, 0.0f,0.5f);
                gluCylinder(quadric, 0.5, 0.7, 0.7, 60, 60);
              glPopMatrix();

              glTranslatef(0.0f, 0.0f,1.2f);
              //circle on bottom
              gluDisk(quadric, 0, 0.7f, 40, 1);
          glPopMatrix();

          // arm1 [1]
          glColor3f(1.0f, 0.0f, 0.0f);
          glRotatef(-90, 1.0f, 0.0f, 0.0f);
          glTranslatef(0.0f, 0.0f, -arm_rad); //link offset
          glRotatef(angle1, 0.0f, 0.0f, 1.0f);
          glPushMatrix();
            //DE - draw cylinder about the x axis
            glPushMatrix();
              glRotatef(90, 0.0f, 1.0f, 0.0f);
              gluCylinder(quadric, arm_rad, arm_rad, arm_len, 40, 40);
            glPopMatrix();
            glTranslatef(arm_len,0.0f,0.0f);
            gluSphere(quadric, arm_rad, 20, 20);
          glPopMatrix();

          // arm2 [2]
          glColor3f(0.0f, 0.0f, 1.0f);
          glTranslatef(arm_len, 0, arm_rad);
          glRotatef(angle2, 0.0f, 0.0f, 1.0f);
          glPushMatrix();
            gluSphere(quadric, arm_rad, 20, 20);

            //glTranslatef(-0.2f, 0.0f, 0.0f);
            //DE - draw cylinder about the x axis
            glPushMatrix();
              glRotatef(-90, 1.0f, 0.0f, 0.0f);
              gluCylinder(quadric, arm_rad, arm_rad, arm_len-(0.05 + 0.8), 40, 40);
            glPopMatrix();
            //glTranslatef(0.0f, arm_len ,0.0f);
          glPopMatrix();

          // rotating bit [3]
          glColor3f(1.0f, 0.0f, 1.0f);
          glRotatef(-90, 1.0f, 0.0f, 0.0f);
          glTranslatef(0.0f ,0.0f, arm_len - (0.05 + 0.8));
          glRotatef(angle3, 0.0f, 0.0f, 1.0f);
          glPushMatrix();

            glPushMatrix();
              glScalef(arm_rad + 0.1, arm_rad, 0.05);
              DrawCube();
            glPopMatrix();

            glPushMatrix();
              glTranslatef(-(0.05 + arm_rad), 0.0f, 0.4 + 0.05);
              glScalef(0.05, arm_rad, 0.4);
              DrawCube();
            glPopMatrix();

            glPushMatrix();
            glTranslatef((0.05 + arm_rad), 0.0f, 0.4 + 0.05);
              glScalef(0.05, arm_rad, 0.4);
              DrawCube();
            glPopMatrix();

          glPopMatrix();


          // arm3
          glColor3f(0.5f, 0.5f, 0.5f);

          glTranslatef(0.0f,0.0f, 0.05 + arm_rad);
          glTranslatef(0.0f,0.0f, 0.4 + arm_rad);
          glRotatef(90, 1.0f, 0.0f, 0.0f);
          glRotatef(90, 0.0f, 1.0f, 0.0f);


          glRotatef(angle4, 0.0f, 0.0f, 1.0f);

          glPushMatrix();
            //glTranslatef(0.0f, 0.0f, 0.0f  );
            glTranslatef(0.0f,-(0.4 + arm_rad), 0.0f  );
            gluSphere(quadric, arm_rad, 20, 20);
            glPushMatrix();
              glRotatef(-90, 1.0f, 0.0f, 0.0f);
              gluCylinder(quadric, arm_rad, arm_rad, arm_len/2, 40, 40);
            glPopMatrix();

          glPopMatrix();


          glTranslatef(0.0f,arm_len/2 - (0.4 + arm_rad), 0.0f  );

          glRotatef(-90, 1.0f, 0.0f, 0.0f);
          glRotatef(angle5, 0.0f, 0.0f, 1.0f);

          glPushMatrix();

            glPushAttrib(GL_CURRENT_BIT);
            glColor3f(0.0f, 1.0f, 0.0f);
            gluSphere(quadric, arm_rad, 20, 20);
            glPushMatrix();
              gluCylinder(quadric, arm_rad, arm_rad, (arm_len/2 - 2*arm_rad), 40, 40);
            glPopMatrix();

            glTranslatef(0.0f,0.0f,(arm_len/2 - 2*arm_rad));

            glPushMatrix();
              drawModCone(30, arm_rad, 2*arm_rad);
            glPopMatrix();

            glPopAttrib();
            glFlush();

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
  glTranslatef(0, 0, -8);
  glMultMatrixf(&(m[0][0]));
  //glRotatef(180, 0, 0, 1);

  if (height) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  glLoadName(0);
  //drawContainer();

      //New quadric object
      GLUquadric *quadric = gluNewQuadric();

      // solve inverse kinematics
      solveInverse();

      // draw the puma manipulator
      glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glRotatef(0, 0.0f, 1.0f, 0.0f);
        drawPuma(quadric);
      glPopMatrix();                        //load first position



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

  if (left_mouse) {
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
      // left_mouse = GL_FALSE;
      // dropSelection();
      // glutPostRedisplay();
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

static void
Usage(void)
{
  printf("Usage: puzzle [-s]\n");
  printf("   -s:  Run in single buffered mode\n");
  exit(-1);
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
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 's':
        doubleBuffer = 0;
        break;
      default:
        Usage();
      }
    } else {
      Usage();
    }
  }

  glutInitWindowSize(W, H);
  if (doubleBuffer) {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
  } else {
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_SINGLE | GLUT_MULTISAMPLE);
  }

  main_window = glutCreateWindow("room");

  init();

  glGetIntegerv(GL_VIEWPORT, viewport);

  glutReshapeFunc(Reshape);
  glutDisplayFunc(redraw);
  //glutKeyboardFunc(keyboard);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  glutVisibilityFunc(visibility);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/
  GLUI *glui = GLUI_Master.create_glui( "GLUI" );
  //new GLUI_Checkbox( glui, "Wireframe", &wireframe );
  (new GLUI_Spinner( glui, "px:", &px ))
    ->set_float_limits( -5.0, 5.0);
  (new GLUI_Spinner( glui, "py:", &py ))
    ->set_float_limits( -5.0, 5.0);
  (new GLUI_Spinner( glui, "pz:", &pz ))
    ->set_float_limits( -5.0, 5.0);
  (new GLUI_Spinner( glui, "Angle 3:", &angle3 ))
    ->set_int_limits( -180, 180);
  (new GLUI_Spinner( glui, "Angle 4:", &angle4 ))
    ->set_int_limits( -180, 180);
  (new GLUI_Spinner( glui, "Angle 5:", &angle5 ))
    ->set_int_limits( -180, 180);

  glui->set_main_gfx_window( main_window );

  /* We register the idle callback with GLUI, *not* with GLUT */
  GLUI_Master.set_glutIdleFunc( myGlutIdle );


  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */




}
