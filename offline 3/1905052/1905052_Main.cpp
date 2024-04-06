#include <bits/stdc++.h>
#include "1905052_Camera.h"
#ifdef __linux__
#include <GL/glut.h>
#elif WIN64
#include <GL/glut.h>
#include <windows.h>
#endif
using namespace std;

FILE *scene;
int image_width, image_height;
// declaration
vector<Object *> objects;
vector<PointLight *> point_lights;
vector<SpotLight *> spot_lights;
int recursion_level;

// populating in the loadData() function
void loadData()
{
    ifstream input("scene.txt");
    input >> recursion_level >> image_height;
    image_width = image_height;

    int num_objects;
    input >> num_objects;

    for (int i = 1; i <= num_objects; i++)
    {
        string shape;
        input >> shape;

        Object *temp;

        if (shape == "sphere")
        {
            temp = new Sphere();
            input >> *((Sphere *)temp);
        }
        else if (shape == "triangle")
        {
            temp = new Triangle();
            input >> *((Triangle *)temp);
        }
        else if (shape == "general")
        {
            temp = new GeneralQuadratic();
            input >> *((GeneralQuadratic *)temp);
        }
        objects.push_back(temp);
    }

    // construct a point light object, say, pl
    // pointLights.push_back(pl)
    int num_plights;
    input >> num_plights;
    for (int i = 0; i < num_plights; i++)
    {
        PointLight *pl = new PointLight();
        input >> *pl;
        point_lights.push_back(pl);
    }

    // construct a spot light object, say, sl
    // spotLights.push_back(sl)
    int num_slights;
    input >> num_slights;
    for (int i = 0; i < num_slights; i++)
    {
        SpotLight *sl = new SpotLight();
        input >> *sl;
        spot_lights.push_back(sl);
    }

    // Besides the objects given in the input file,you need to drawa floor.
    Object *temp;
    // temp = new Floor(400, 10);
    temp = new Floor(1000, 20);
    temp->setColor(Color(0.5, 0.5, 0.5));
    vector<double> coEfficients = {0.4, 0.2, 0.2, 0.2};
    temp->setCoEfficients(coEfficients);
    objects.push_back(temp);
}

double windowWidth = 500, windowHeight = 500, viewAngle = 80;
int no_of_img = 1;
double pi = acos(-1.0);
void capture()
{
    cout << "image capturing starts" << endl;
    // initialize bitmap image and set background color to black
    bitmap_image img;
    img = bitmap_image(image_width, image_height);
    for (int i = 1; i <= image_width; i++)
    {
        for (int j = 1; j <= image_height; j++)
        {
            img.set_pixel(i - 1, j - 1, 0, 0, 0);
        }
    }

    double planeDistance = (windowHeight / 2.0) / tan((viewAngle / 2.0) * (pi / 180.0));

    Vector3D topleft = camera_pos + (camera_eye_dir * planeDistance) - (camera_right_dir * (windowWidth / 2.0)) + (camera_up_dir * (windowHeight / 2.0));

    double du = windowWidth / (image_width * 1.0);
    double dv = windowHeight / (image_height * 1.0);
    // Choose middle of the grid cell
    topleft = topleft + camera_right_dir * (0.5 * du) - camera_up_dir * (0.5 * dv);

    int nearest;
    double t, tMin;

    for (int i = 0; i < image_width; i++)
    {
        for (int j = 0; j < image_height; j++)
        {
            // calculate curPixel using topleft,r,u,i,j,du,dv
            Vector3D curPixel = topleft + (camera_right_dir * du * i) - (camera_up_dir * dv * j);
            // cast ray from eye to (curPixel-eye) direction
            Ray ray(camera_pos, curPixel - camera_pos);
            Color color;

            // finding the nearest object
            tMin = -1;
            nearest = -1;
            for (int k = 0; k < (int)objects.size(); k++)
            {
                t = objects[k]->intersect(ray, color, 0);
                // update t so that it stores min +ve value
                // save the nearest object, on
                if (t > 0 && (t < tMin || nearest == -1))
                {
                    tMin = t, nearest = k;
                }
            }

            // the pixel should be colored when the nearest object is found
            if (nearest != -1)
            {
                color = Color(0, 0, 0);
                double t = objects[nearest]->intersect(ray, color, 1);

                color.r = std::clamp(color.r, 0.0, 1.0);
                color.g = std::clamp(color.g, 0.0, 1.0);
                color.b = std::clamp(color.b, 0.0, 1.0);

                img.set_pixel(i, j, 255 * color.r, 255 * color.g, 255 * color.b);
            }
        }
    }

    img.save_image("1905052_Output_" + to_string(no_of_img) + ".bmp");
    cout << "image capturing ends" << endl;
    cout << "1905052_Output_" + to_string(no_of_img) + ".bmp"
         << " is created" << endl;
    no_of_img++;
}

void keyboardListener(unsigned char key, int x, int y)
{
    switch (key)
    {
    case '0':
        capture();
        break;
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
    case '7':
        cout << "Refrection mode On" << endl;
        refrection = 1;
        break;
    case '8':
        cout << "Refrection mode Off" << endl;
        refrection = 0;
        break;
    default:
        return;
    }
}

void specialKeyListener(int key, int x, int y)
{
    switch (key)
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
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z, camera_pos.x + camera_eye_dir.x, camera_pos.y + camera_eye_dir.y, camera_pos.z + camera_eye_dir.z, camera_up_dir.x, camera_up_dir.y, camera_up_dir.z);
    glMatrixMode(GL_MODELVIEW);
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw();
    }
    for (int i = 0; i < point_lights.size(); i++)
    {
        point_lights[i]->draw();
    }
    for (int i = 0; i < spot_lights.size(); i++)
    {
        spot_lights[i]->draw();
    }
    glutSwapBuffers();
}

void animate()
{
    glutPostRedisplay();
}

void init()
{
    refrection = 0;
    camera_pos = {0, 0, 100};
    camera_eye_dir = {0, 0, -1};
    camera_up_dir = {0, 1, 0};
    camera_right_dir = {1, 0, 0};
    loadData();
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80, 1, 1, 1000.0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Offline 3");
    init();
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutIdleFunc(animate);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMainLoop();

    for (auto obj : objects)
    {
        delete obj;
    }
    objects.clear();

    for (auto pl : point_lights)
    {
        delete pl;
    }
    point_lights.clear();

       for (auto sl : spot_lights)
    {
        delete sl;
    }
    spot_lights.clear();
    return 0;
}