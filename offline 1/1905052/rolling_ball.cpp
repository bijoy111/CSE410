#include <bits/stdc++.h>
#ifdef __linux__
#include <GL/glut.h>
#elif WIN64
#include <GL/glut.h>
#include <windows.h>
#endif
using namespace std;

struct point 
{
  GLfloat x, y, z;
};
struct point camera_pos, camera_eye_dir, camera_up_dir, camera_right_dir;

struct point ball_pos,ball_eye_dir,ball_up_dir,ball_right_dir;
double ball_radius=0.2;
double rotate_own_axis_angle=0;
int sim_mode=0;
double event_time;
void collision_time();

void init() 
{
    glClearColor(0.0f, 0.0f, 0.0f,1.0f); // background color is set to black and opaque

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 500);

    camera_pos.x = 0;
    camera_pos.y = 0;
    camera_pos.z = 60;

    camera_eye_dir.x = 0;
    camera_eye_dir.y = 0;
    camera_eye_dir.z = -60;

    camera_up_dir.x = 0;
    camera_up_dir.y = 1;
    camera_up_dir.z = 0;
    
    camera_right_dir.x = 1;
    camera_right_dir.y = 0;
    camera_right_dir.z = 0;
    
    ball_pos.x = 0;
    ball_pos.y = 0;
    ball_pos.z = 0;

    ball_eye_dir.x = 1;
    ball_eye_dir.y = 1;
    ball_eye_dir.z = 0;

    ball_up_dir.x = 0;
    ball_up_dir.y = 0;
    ball_up_dir.z = 1;
  
    ball_right_dir.x=(ball_up_dir.y*ball_eye_dir.z)-(ball_up_dir.z*ball_eye_dir.y);
    ball_right_dir.y=(ball_up_dir.z*ball_eye_dir.x)-(ball_up_dir.x*ball_eye_dir.z);
    ball_right_dir.z=(ball_up_dir.x*ball_eye_dir.y)-(ball_up_dir.y*ball_eye_dir.x);

    double tmp=sqrt(ball_right_dir.x*ball_right_dir.x+ball_right_dir.y*ball_right_dir.y+ball_right_dir.z*ball_right_dir.z);

    ball_right_dir.x=ball_right_dir.x/tmp;
    ball_right_dir.y=ball_right_dir.y/tmp;
    ball_right_dir.z=ball_right_dir.z/tmp;
}

void billiard_board(double a) 
{
    for (int i = -40; i < 40; i++) 
    {
        for (int j = -40; j < 40; j++) 
        {
            glPushMatrix();
            glTranslatef(i * a, j * a, 0);

            if(i==-5&&(j>=-5&&j<=5))
            {
                glBegin(GL_QUADS);
                {      
                    glColor3f(1.0f, 0.0f, 0.0f); // red
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, a, 0);
                    glVertex3f(a, a, 0);
                    glVertex3f(a, 0, 0);
                }
                glEnd();
            }
            else if(i==5&&(j>=-5&&j<=5))
            {
                glBegin(GL_QUADS);
                {      
                    glColor3f(1.0f, 0.0f, 0.0f); // red
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, a, 0);
                    glVertex3f(a, a, 0);
                    glVertex3f(a, 0, 0);
                }
                glEnd();
            }
            else if(j==-5&&(i>=-5&&i<=5))
            {
                glBegin(GL_QUADS);
                {      
                    glColor3f(1.0f, 0.0f, 0.0f); // red
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, a, 0);
                    glVertex3f(a, a, 0);
                    glVertex3f(a, 0, 0);
                }
                glEnd();
            }
            else if(j==5&&(i>=-5&&i<=5))
            {
                glBegin(GL_QUADS);
                {      
                    glColor3f(1.0f, 0.0f, 0.0f); // red
                    glVertex3f(0, 0, 0);
                    glVertex3f(0, a, 0);
                    glVertex3f(a, a, 0);
                    glVertex3f(a, 0, 0);
                }
                glEnd();
            }
            else 
            {
                if((i+j)%2==0)
                {
                    glBegin(GL_QUADS);
                    {      
                        glColor3f(0.0f, 0.0f, 0.0f); // Black
                        glVertex3f(0, 0, 0);
                        glVertex3f(0, a, 0);
                        glVertex3f(a, a, 0);
                        glVertex3f(a, 0, 0);
                    }
                    glEnd();
                }
                else
                {  
                    glBegin(GL_QUADS);
                    {      
                        glColor3f(1.0f, 1.0f, 1.0f); // White
                        glVertex3f(0, 0, 0);
                        glVertex3f(0, a, 0);
                        glVertex3f(a, a, 0);
                        glVertex3f(a, 0, 0);
                    }
                    glEnd();
                }
            }
            glPopMatrix();
        }
    }
}

void sphere_draw_helper()
{
    double stacks=9;
    double sectors=9;
    double radius=0.2;
    glTranslatef(0, 0, ball_radius);

    glRotatef(rotate_own_axis_angle, ball_eye_dir.x, ball_eye_dir.y, ball_eye_dir.z);

    for (int i = 0; i < stacks; i++) 
    {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sectors; j++) 
        {
            if (i < stacks/2) 
            {
                if (j % 2) 
                {
                    glColor3f(1, 0, 0);
                } 
                else 
                {
                    glColor3f(0, 1, 0);
                }
            } 
            else 
            {
                if (j % 2) 
                {
                    glColor3f(0, 1, 0);
                } 
                else 
                {
                    glColor3f(1, 0, 0);
                }
            }

            double x = radius * sin(i * M_PI / stacks) * cos(2 * j * M_PI / sectors);
            double y = radius * sin(i * M_PI / stacks) * sin(2 * j * M_PI / sectors);
            double z = radius * cos(i * M_PI / stacks);
            glVertex3f(x, y, z);

            x = radius * sin((i+1) * M_PI / stacks) * cos(2 * j * M_PI / sectors);
            y = radius * sin((i+1) * M_PI / stacks) * sin(2 * j * M_PI / sectors);
            z = radius * cos((i+1) * M_PI / stacks);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}

void sphere_draw() 
{
    glTranslatef(ball_pos.x, ball_pos.y, ball_pos.z);
        
    glBegin(GL_LINES); 
    {
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, ball_radius);
        glVertex3f(ball_right_dir.x,ball_right_dir.y,ball_right_dir.z+(ball_radius*0.5));
    } glEnd();

    sphere_draw_helper();
}

void display() 
{
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // controling camera
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z,camera_eye_dir.x,camera_eye_dir.y, camera_eye_dir.z,camera_up_dir.x, camera_up_dir.y, camera_up_dir.z);

    billiard_board(5);

    glScalef(10,10,10);
    sphere_draw();

    //cout<<ball_pos.x<<" "<<ball_pos.y<<" "<<ball_pos.z<<endl;
    //cout<<ball_right_dir.x<<" "<<ball_right_dir.y<<" "<<ball_right_dir.z<<endl;

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

void direction_rotate_clockwise()
{
    double tmp=0.05;
    double x=(ball_right_dir.y*ball_up_dir.z)-(ball_right_dir.z*ball_up_dir.y);
    double y=(ball_right_dir.z*ball_up_dir.x)-(ball_right_dir.x*ball_up_dir.z);
    double z=(ball_right_dir.x*ball_up_dir.y)-(ball_right_dir.y*ball_up_dir.x);
    double xx=x/(sqrt(x*x+y*y+z*z));
    double yy=y/(sqrt(x*x+y*y+z*z));
    double zz=z/(sqrt(x*x+y*y+z*z));
    x=xx,y=yy,z=zz;
    x=x*tan(tmp);
    y=y*tan(tmp);
    z=z*tan(tmp);
    xx=x/(sqrt(x*x+y*y+z*z));
    yy=y/(sqrt(x*x+y*y+z*z));
    zz=z/(sqrt(x*x+y*y+z*z));

    ball_right_dir.x=xx;
    ball_right_dir.y=yy;
    ball_right_dir.z=zz; 

    //collision_time();
}

void direction_rotate_cc()
{
    double tmp=0.05;
    double x=(ball_right_dir.y*ball_up_dir.z)-(ball_right_dir.z*ball_up_dir.y);
    double y=(ball_right_dir.z*ball_up_dir.x)-(ball_right_dir.x*ball_up_dir.z);
    double z=(ball_right_dir.x*ball_up_dir.y)-(ball_right_dir.y*ball_up_dir.x);
    double xx=x/(sqrt(x*x+y*y+z*z));
    double yy=y/(sqrt(x*x+y*y+z*z));
    double zz=z/(sqrt(x*x+y*y+z*z));
    x=xx,y=yy,z=zz;
    x=x*(-tan(tmp));
    y=y*(-tan(tmp));
    z=z*(-tan(tmp));
    xx=x/(sqrt(x*x+y*y+z*z));
    yy=y/(sqrt(x*x+y*y+z*z));
    zz=z/(sqrt(x*x+y*y+z*z));

    ball_right_dir.x=xx;
    ball_right_dir.y=yy;
    ball_right_dir.z=zz; 

    //collision_time();
}

void go_forward()
{
    while (rotate_own_axis_angle > 360)
    {
        rotate_own_axis_angle-=360;
    }
    rotate_own_axis_angle-=((0.1/ball_radius)*(180.0/M_PI));

    ball_pos.x+=(0.1*ball_right_dir.x);
    ball_pos.y+=(0.1*ball_right_dir.y);
    ball_pos.z+=(0.1*ball_right_dir.z);

    //cout<<ball_pos.x<<" "<<ball_pos.y<<" "<<ball_pos.z<<endl;

    ball_eye_dir.x=(ball_right_dir.y*ball_up_dir.z)-(ball_right_dir.z*ball_up_dir.y);
    ball_eye_dir.y=(ball_right_dir.z*ball_up_dir.x)-(ball_right_dir.x*ball_up_dir.z);
    ball_eye_dir.z=(ball_right_dir.x*ball_up_dir.y)-(ball_right_dir.y*ball_up_dir.x);

    double tmp=sqrt(ball_eye_dir.x*ball_eye_dir.x+ball_eye_dir.y*ball_eye_dir.y+ball_eye_dir.z*ball_eye_dir.z);
    ball_eye_dir.x/=tmp;
    ball_eye_dir.y/=tmp;
    ball_eye_dir.z/=tmp;

    if(sim_mode==0)
    {
        if(ball_pos.x>=2.4 || ball_pos.x<=-1.8)
        {
        ball_right_dir.x*=(-1);
        }
        if(ball_pos.y>=2.4 || ball_pos.y<=-1.8)
        {
            ball_right_dir.y*=(-1);
        }
    }
}

void go_backward()
{
    rotate_own_axis_angle+=((0.1/ball_radius)*(180.0/M_PI));
    while (rotate_own_axis_angle < -360)
    {
        rotate_own_axis_angle+=360;
    }

    ball_pos.x-=(0.1*ball_right_dir.x);
    ball_pos.y-=(0.1*ball_right_dir.y);
    ball_pos.z-=(0.1*ball_right_dir.z);


    ball_eye_dir.x=(ball_right_dir.y*ball_up_dir.z)-(ball_right_dir.z*ball_up_dir.y);
    ball_eye_dir.y=(ball_right_dir.z*ball_up_dir.x)-(ball_right_dir.x*ball_up_dir.z);
    ball_eye_dir.z=(ball_right_dir.x*ball_up_dir.y)-(ball_right_dir.y*ball_up_dir.x);

    double tmp=sqrt(ball_eye_dir.x*ball_eye_dir.x+ball_eye_dir.y*ball_eye_dir.y+ball_eye_dir.z*ball_eye_dir.z);
    ball_eye_dir.x/=tmp;
    ball_eye_dir.y/=tmp;
    ball_eye_dir.z/=tmp;

    if(sim_mode==0)
    {
        if(ball_pos.x>=2.4 || ball_pos.x<=-1.8)
        {
        ball_right_dir.x*=(-1);
        }
        if(ball_pos.y>=2.4 || ball_pos.y<=-1.8)
        {
            ball_right_dir.y*=(-1);
        }
    }
}

void event_driven(int value)
{
    //cout<<"hello"<<endl;
    if(sim_mode==1)
    {
        if(value==0)
        {
            ball_right_dir.x*=(-1);
        }  
        else if(value==1)
        {
            ball_right_dir.y*=(-1);
        }
        else if(value==2)
        {
            ball_right_dir.x*=(-1);
            ball_right_dir.y*=(-1);
        }

        collision_time();
    }
}

void collision_time()
{
    double tmp,tmp1,tmp2;
    //cout<<ball_right_dir.x<<endl;
    //cout<<ball_right_dir.y<<endl;
    int value;
    if(ball_right_dir.x>=0)
    {
        tmp1=(2.4-ball_pos.x)*(100/0.07071);
    }
    else
    {
        tmp1=(-1.8-ball_pos.x)*(100/0.07071);
        tmp1*=(-1);
    }
    if(ball_right_dir.y>=0)
    {
        tmp2=(2.4-ball_pos.y)*(100/0.07071);
    }
    else
    {
        tmp2=(-1.8-ball_pos.y)*(100/0.07071);
        tmp2*=(-1);
    }
    if(tmp1<tmp2)
    {
        tmp=tmp1;
        value=0;
    }
    else if(tmp1>tmp2)
    {
        tmp=tmp2;
        value=1;
    }
    else
    {
        tmp=tmp1;
        value=2;
    }
    event_time=tmp;
    //cout<<event_time<<endl;
    glutTimerFunc(event_time, event_driven,value);
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
        case 'j':
            direction_rotate_cc();
            collision_time();
            break;
        case 'l':
            direction_rotate_clockwise();
            collision_time();
            break;
        case 'i':
            go_forward();
            break;
        case 'k':
            go_backward();
            break;
        case ' ':
            sim_mode=(sim_mode==0)?1:0;
            collision_time();
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

void simulation_mode(int value) 
{
    if (sim_mode==1)
    {
        go_forward();
    } 
    glutTimerFunc(100, simulation_mode, 0);
}

int main(int argc,char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(640, 640);   // initial width & height of window is set
    glutInitWindowPosition(700, 200);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Rolling Ball");
    glutDisplayFunc(display);

    glutKeyboardFunc(keyboardListener); // Register for callback handler of normal key event
    glutSpecialFunc(specialKeyListener);// Register for callback handler of special key event
    glutIdleFunc(idle);
    init();

    glutTimerFunc(100, simulation_mode, 0);
    glutMainLoop();
    return 0;
}