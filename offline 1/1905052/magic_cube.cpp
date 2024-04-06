#include <bits/stdc++.h>
#ifdef __linux__
#include <GL/glut.h>
#elif WIN64
#include <GL/glut.h>
#include <windows.h>
#endif
using namespace std;

GLfloat base_triangle_scale = 1.0;
GLfloat base_triangle_Gx = 0;
GLfloat base_triangle_Gy = 0;
GLfloat base_triangle_Gz = 0;
GLfloat radius=0.577;
GLfloat sphere_face_scale = 0;
GLfloat sphere_face_translateX = 1.0;
GLfloat sphere_face_translateY = 0;
GLfloat sphere_face_translateZ = 0;
GLfloat cylinder_theta = M_PI / 2.0;
GLfloat cylinder_height = sqrt(2.0);
GLfloat cylinder_translateX = 1/cylinder_height;
GLfloat rotate_ang = 0;

struct point 
{
  GLfloat x, y, z;
};

struct point camera_pos, camera_eye_dir, camera_up_dir, camera_right_dir;

void base_traiangle()
{
    glBegin(GL_TRIANGLES);{
        glVertex3f(1, 0, 0);
        glVertex3f(0, 1, 0);
        glVertex3f(0, 0, 1);
    }glEnd();
}

void half_octahedron() 
{
  for (int i = 0; i < 4; i++)
  {
    if (i % 2 != 0)
        glColor3d(0, 0.5, 0.5);
    else
        glColor3d(0.5, 0, 0.5);

    glRotated(90, 0, 1, 0); // angle is 90 degree
    glPushMatrix();
    glTranslated(base_triangle_Gx, base_triangle_Gy, base_triangle_Gz);
    glScaled(base_triangle_scale, base_triangle_scale, base_triangle_scale);
    base_traiangle();
    glPopMatrix();
  }
}

void octahedron() 
{
  glPushMatrix();
  half_octahedron();
  glPopMatrix();

  glPushMatrix();
  glRotated(180, 0, 0, 1);
  half_octahedron();
  glPopMatrix(); 
}

void unit_cubesphere_face(int subdivision) 
{
    const double angle_rad = acos(-1) / 180.0f;

    double normal_longitude_Y[3];
    double normal_latitude_Z[3];
    double direction_vec[3];  // direction vector intersecting 2 planes normal_longitude_Y  cross_product  normal_latitude_Z
    double longitude_angle_Y;
    double latitude_angle_Z;

    int vertices_row = (int)pow(2, subdivision) + 1;

    struct point points[vertices_row][vertices_row];

    // latitudinal plane is rotated from 45 degrees to -45 degrees along Z axis from top to bottom
    for (unsigned int i = 0; i < vertices_row; i++)
    {
      latitude_angle_Z = angle_rad * (45.0f - 90.0f * i / (vertices_row - 1));
      normal_latitude_Z[0] = -sin(latitude_angle_Z);
      normal_latitude_Z[1] = cos(latitude_angle_Z);
      normal_latitude_Z[2] = 0;

      // longitudinal plane is rotated from -45 degrees to 45 along Y axis from left to right
      for (unsigned int j = 0; j < vertices_row; ++j)
      {
        longitude_angle_Y = angle_rad * (-45.0f + 90.0f * j / (vertices_row - 1));
        normal_longitude_Y[0] = -sin(longitude_angle_Y);
        normal_longitude_Y[1] = 0;
        normal_longitude_Y[2] = -cos(longitude_angle_Y);

        // find direction vector of intersected line, n1 x n2
        direction_vec[0] = normal_longitude_Y[1] * normal_latitude_Z[2] - normal_longitude_Y[2] * normal_latitude_Z[1];
        direction_vec[1] = normal_longitude_Y[2] * normal_latitude_Z[0] - normal_longitude_Y[0] * normal_latitude_Z[2];
        direction_vec[2] = normal_longitude_Y[0] * normal_latitude_Z[1] - normal_longitude_Y[1] * normal_latitude_Z[0];

        float factor = radius / sqrt(direction_vec[0] * direction_vec[0] + direction_vec[1] * direction_vec[1] + direction_vec[2] * direction_vec[2]);

        direction_vec[0] *= factor;
        direction_vec[1] *= factor;
        direction_vec[2] *= factor;

        points[i][j].x = direction_vec[0];
        points[i][j].y = direction_vec[1];
        points[i][j].z = direction_vec[2];
      }
    }

  glBegin(GL_QUADS);
  for (int j = 0; j < vertices_row-1; j++)
  {
    for (int i = 0; i < vertices_row-1; i++)
    {
      glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
      glVertex3f(points[j][i + 1].x, points[j][i + 1].y, points[j][i + 1].z);

      glVertex3f(points[j + 1][i + 1].x, points[j + 1][i + 1].y, points[j + 1][i + 1].z);
      glVertex3f(points[j + 1][i].x, points[j + 1][i].y, points[j + 1][i].z);
    }
  }
  glEnd();
}

void sphere_face() 
{
  glPushMatrix();
  glTranslated(sphere_face_translateX, sphere_face_translateY, sphere_face_translateZ);
  glScaled(sphere_face_scale, sphere_face_scale, sphere_face_scale);
  unit_cubesphere_face(5);
  glPopMatrix();
}

void sphere(double radius)
{ 
  GLfloat angle = 90;

  glPushMatrix();
  // side faces are drawn below
  for (int i = 0; i < 4; i++) 
  {
    if (i % 2)
      glColor3d(0, 0, 1.0);
    else
      glColor3d(0, 1.0, 0);

    sphere_face();
    glRotated(angle, 0, 1, 0);
  }
  glPopMatrix();

  glPushMatrix();
  glColor3d(1.0, 0, 0);
  glRotated(angle, 0, 0, 1);      
  sphere_face(); // top face is drawn

  glRotated(angle * 2, 0, 0, 1);
  sphere_face(); // bottom face is drawn
  glPopMatrix();
}

void cylinder_helper(int subdivision) 
{
  GLfloat prevx = radius, prevy = 0;
  GLfloat newx, newy;
  GLfloat theta = -cylinder_theta / 2.0;
  GLfloat halfHeight = cylinder_height / 2.0; 

  glPushMatrix();
  glBegin(GL_QUADS);
  for (int i = 1; i <= subdivision; i++)
  {
    theta += cylinder_theta / subdivision;
    newx = radius * cos(theta);
    newy = radius * sin(theta);

    glVertex3f(newx, newy, halfHeight);
    glVertex3f(newx, newy, -halfHeight);

    glVertex3f(prevx, prevy, -halfHeight);
    glVertex3f(prevx, prevy, halfHeight);

    prevx = newx;
    prevy = newy;
  }
  glEnd();
  glPopMatrix();
}

void cylinder() 
{
  glPushMatrix();
  glRotated(45, 0, 1, 0);
  glTranslated(cylinder_translateX, 0, 0);
  glScaled(sphere_face_scale, sphere_face_scale, 1);
  cylinder_helper(50);
  glPopMatrix();
}

void four_cylinder() 
{
  glPushMatrix();
  for (int i = 1; i <= 4; i++)
  {
    cylinder();
    glRotated(90, 0, 1, 0);
  }
  glPopMatrix();
}

// display function is called whenever the window needs to be repainted.
void display()
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // controling camera
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z,camera_pos.x + camera_eye_dir.x, camera_pos.y + camera_eye_dir.y, camera_pos.z + camera_eye_dir.z,camera_up_dir.x, camera_up_dir.y, camera_up_dir.z);

    glScaled(2, 2, 2);
    glRotated(rotate_ang, 0, 1, 0);
    glPushMatrix();
    octahedron();
    glPopMatrix();
    
    glPushMatrix();
    sphere(0.577);
    glPopMatrix();

    // cylinder is drawn here
    glPushMatrix();
    glColor3d(0.5, 0.5, 0);
    four_cylinder();

    glRotated(90, 1, 0, 0);
    four_cylinder();

    glRotated(90, 0, 0, 1);
    four_cylinder();
    glPopMatrix();

    glutSwapBuffers();
}

void look_left()
{
  camera_right_dir.x=(camera_right_dir.x*cos(0.01)+camera_eye_dir.x*sin(0.01));
  camera_right_dir.y=(camera_right_dir.y*cos(0.01)+camera_eye_dir.y*sin(0.01));
  camera_right_dir.z=(camera_right_dir.z*cos(0.01)+camera_eye_dir.z*sin(0.01));

  camera_eye_dir.x=(camera_eye_dir.x*cos(0.01)-camera_right_dir.x*sin(0.01));
  camera_eye_dir.y=(camera_eye_dir.y*cos(0.01)-camera_right_dir.y*sin(0.01));
  camera_eye_dir.z=(camera_eye_dir.z*cos(0.01)-camera_right_dir.z*sin(0.01));
}

void look_right()
{
  camera_right_dir.x=(camera_right_dir.x*cos(-0.01)+camera_eye_dir.x*sin(-0.01));
  camera_right_dir.y=(camera_right_dir.y*cos(-0.01)+camera_eye_dir.y*sin(-0.01));
  camera_right_dir.z=(camera_right_dir.z*cos(-0.01)+camera_eye_dir.z*sin(-0.01));

  camera_eye_dir.x=(camera_eye_dir.x*cos(-0.01)-camera_right_dir.x*sin(-0.01));
  camera_eye_dir.y=(camera_eye_dir.y*cos(-0.01)-camera_right_dir.y*sin(-0.01));
  camera_eye_dir.z=(camera_eye_dir.z*cos(-0.01)-camera_right_dir.z*sin(-0.01));
}

void look_up()
{
  camera_eye_dir.x=(camera_eye_dir.x*cos(0.01)+camera_up_dir.x*sin(0.01));
  camera_eye_dir.y=(camera_eye_dir.y*cos(0.01)+camera_up_dir.y*sin(0.01));
  camera_eye_dir.z=(camera_eye_dir.z*cos(0.01)+camera_up_dir.z*sin(0.01));

  camera_up_dir.x=(camera_up_dir.x*cos(0.01)-camera_eye_dir.x*sin(0.01));
  camera_up_dir.y=(camera_up_dir.y*cos(0.01)-camera_eye_dir.y*sin(0.01));
  camera_up_dir.z=(camera_up_dir.z*cos(0.01)-camera_eye_dir.z*sin(0.01));
}

void look_down()
{
  camera_eye_dir.x=(camera_eye_dir.x*cos(-0.01)+camera_up_dir.x*sin(-0.01));
  camera_eye_dir.y=(camera_eye_dir.y*cos(-0.01)+camera_up_dir.y*sin(-0.01));
  camera_eye_dir.z=(camera_eye_dir.z*cos(-0.01)+camera_up_dir.z*sin(-0.01));

  camera_up_dir.x=(camera_up_dir.x*cos(-0.01)-camera_eye_dir.x*sin(-0.01));
  camera_up_dir.y=(camera_up_dir.y*cos(-0.01)-camera_eye_dir.y*sin(-0.01));
  camera_up_dir.z=(camera_up_dir.z*cos(-0.01)-camera_eye_dir.z*sin(-0.01));
} 

void tilt_counterclockwise()
{
  camera_up_dir.x=(camera_up_dir.x*cos(0.01)-camera_right_dir.x*sin(0.01));
  camera_up_dir.y=(camera_up_dir.y*cos(0.01)-camera_right_dir.y*sin(0.01));
  camera_up_dir.z=(camera_up_dir.z*cos(0.01)-camera_right_dir.z*sin(0.01));

  camera_right_dir.x=(camera_right_dir.x*cos(0.01)-camera_up_dir.x*sin(0.01));
  camera_right_dir.y=(camera_right_dir.y*cos(0.01)-camera_up_dir.y*sin(0.01));
  camera_right_dir.z=(camera_right_dir.z*cos(0.01)-camera_up_dir.z*sin(0.01));
}

void tilt_clockwise()
{
  camera_up_dir.x=(camera_up_dir.x*cos(-0.01)-camera_right_dir.x*sin(-0.01));
  camera_up_dir.y=(camera_up_dir.y*cos(-0.01)-camera_right_dir.y*sin(-0.01));
  camera_up_dir.z=(camera_up_dir.z*cos(-0.01)-camera_right_dir.z*sin(-0.01));

  camera_right_dir.x=(camera_right_dir.x*cos(-0.01)-camera_up_dir.x*sin(-0.01));
  camera_right_dir.y=(camera_right_dir.y*cos(-0.01)-camera_up_dir.y*sin(-0.01));
  camera_right_dir.z=(camera_right_dir.z*cos(-0.01)-camera_up_dir.z*sin(-0.01));
}

void move_up_wo_cng_ref_pnt()
{
  camera_pos.y += 0.1;
  camera_eye_dir.y -= 0.1;
}

void move_down_wo_cng_ref_pnt()
{
  camera_pos.y -= 0.1;
  camera_eye_dir.y += 0.1;
}

void move_forward()
{
  camera_pos.x+=camera_eye_dir.x*0.1;
  camera_pos.y+=camera_eye_dir.y*0.1;
  camera_pos.z+=camera_eye_dir.z*0.1;
}

void move_backward()
{
  camera_pos.x-=camera_eye_dir.x*0.1;
  camera_pos.y-=camera_eye_dir.y*0.1;
  camera_pos.z-=camera_eye_dir.z*0.1;
}

void move_right()
{
  camera_pos.x+=camera_right_dir.x*0.1;
  camera_pos.y+=camera_right_dir.y*0.1;
  camera_pos.z+=camera_right_dir.z*0.1;
}

void move_left()
{
  camera_pos.x-=camera_right_dir.x*0.1;
  camera_pos.y-=camera_right_dir.y*0.1;
  camera_pos.z-=camera_right_dir.z*0.1;
}

void move_up()
{
  camera_pos.x+=camera_up_dir.x*0.1;
  camera_pos.y+=camera_up_dir.y*0.1;
  camera_pos.z+=camera_up_dir.z*0.1;
}

void move_down()
{
  camera_pos.x-=camera_up_dir.x*0.1;
  camera_pos.y-=camera_up_dir.y*0.1;
  camera_pos.z-=camera_up_dir.z*0.1;
}

void keyboardListener(unsigned char key, int x, int y) 
{
    switch(key)
    {
        case '1':
            look_left();
            break;
        case '2':
            look_right();
            break;
        case '3':
            look_up();
            break;
        case '4':
            look_down();
            break;
        case '5':
            tilt_counterclockwise();
            break;
        case '6':
            tilt_clockwise();
            break;
        case 'w':
            move_up_wo_cng_ref_pnt();
            break;
        case 's':
            move_down_wo_cng_ref_pnt();
            break;
        case '.':
            if(base_triangle_scale<1.0)
            {
                base_triangle_scale+=(1.0/16);
                base_triangle_Gx-=(0.3333/16);
                base_triangle_Gy-=(0.3333/16);
                base_triangle_Gz-=(0.3333/16);

                sphere_face_scale-=(1.0/16);
                sphere_face_translateX+=(1.0/16);

                cylinder_height+=(sqrt(2.0)/16);
                cylinder_translateX+=((1.0/sqrt(2.0))/16);
            }
            break;
        case ',':
            if(base_triangle_scale>0.0)
            {
                base_triangle_scale-=(1.0/16);
                base_triangle_Gx+=(0.3333/16);
                base_triangle_Gy+=(0.3333/16);
                base_triangle_Gz+=(0.3333/16);

                sphere_face_scale+=(1.0/16);
                sphere_face_translateX-=(1.0/16);

                cylinder_height-=(sqrt(2.0)/16);
                cylinder_translateX-=((1.0/sqrt(2.0))/16);
            }
            break;
        case 'a':
            rotate_ang-=5;
            break;
        case 'd':
            rotate_ang+=5;
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) 
{
    switch(key)
    {
        case GLUT_KEY_UP:
            move_forward();
            break;
        case GLUT_KEY_DOWN:
            move_backward();
            break;
        case GLUT_KEY_RIGHT:
            move_right();
            break;
        case GLUT_KEY_LEFT:
            move_left();
            break;
        case GLUT_KEY_PAGE_UP:
            move_up();
            break;
        case GLUT_KEY_PAGE_DOWN:
            move_down();
            break;
        default:
            return;
    }
    glutPostRedisplay();
}

void idle() 
{
  glutPostRedisplay(); 
}

void init() 
{
    glClearColor(0.0f, 0.0f, 0.0f,1.0f); // background color is set to black and opaque

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 500);

    camera_pos.x = 0;
    camera_pos.y = 0;
    camera_pos.z = 10;

    camera_eye_dir.x = 0;
    camera_eye_dir.y = 0;
    camera_eye_dir.z = -10;

    camera_up_dir.x = 0;
    camera_up_dir.y = 1;
    camera_up_dir.z = 0;
    
    camera_right_dir.x = 1;
    camera_right_dir.y = 0;
    camera_right_dir.z = 0;

}

int main(int argc,char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);   // initial width & height of window is set
    glutInitWindowPosition(700, 200);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Magic Cube");
    glutDisplayFunc(display);

    glutKeyboardFunc(keyboardListener); // Register for callback handler of normal key event
    glutSpecialFunc(specialKeyListener);// Register for callback handler of special key event
    glutIdleFunc(idle);
    init();
    glutMainLoop();
    return 0;
}