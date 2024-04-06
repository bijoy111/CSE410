#include "1905052_Header.h"

Vector3D camera_pos, camera_eye_dir, camera_up_dir, camera_right_dir;

void look_left()
{
    camera_right_dir.x = (camera_right_dir.x * cos(0.01) + camera_eye_dir.x * sin(0.01));
    camera_right_dir.y = (camera_right_dir.y * cos(0.01) + camera_eye_dir.y * sin(0.01));
    camera_right_dir.z = (camera_right_dir.z * cos(0.01) + camera_eye_dir.z * sin(0.01));

    camera_eye_dir.x = (camera_eye_dir.x * cos(0.01) - camera_right_dir.x * sin(0.01));
    camera_eye_dir.y = (camera_eye_dir.y * cos(0.01) - camera_right_dir.y * sin(0.01));
    camera_eye_dir.z = (camera_eye_dir.z * cos(0.01) - camera_right_dir.z * sin(0.01));
}

void look_right()
{
    camera_right_dir.x = (camera_right_dir.x * cos(-0.01) + camera_eye_dir.x * sin(-0.01));
    camera_right_dir.y = (camera_right_dir.y * cos(-0.01) + camera_eye_dir.y * sin(-0.01));
    camera_right_dir.z = (camera_right_dir.z * cos(-0.01) + camera_eye_dir.z * sin(-0.01));

    camera_eye_dir.x = (camera_eye_dir.x * cos(-0.01) - camera_right_dir.x * sin(-0.01));
    camera_eye_dir.y = (camera_eye_dir.y * cos(-0.01) - camera_right_dir.y * sin(-0.01));
    camera_eye_dir.z = (camera_eye_dir.z * cos(-0.01) - camera_right_dir.z * sin(-0.01));
}

void look_up()
{
    camera_eye_dir.x = (camera_eye_dir.x * cos(0.01) + camera_up_dir.x * sin(0.01));
    camera_eye_dir.y = (camera_eye_dir.y * cos(0.01) + camera_up_dir.y * sin(0.01));
    camera_eye_dir.z = (camera_eye_dir.z * cos(0.01) + camera_up_dir.z * sin(0.01));

    camera_up_dir.x = (camera_up_dir.x * cos(0.01) - camera_eye_dir.x * sin(0.01));
    camera_up_dir.y = (camera_up_dir.y * cos(0.01) - camera_eye_dir.y * sin(0.01));
    camera_up_dir.z = (camera_up_dir.z * cos(0.01) - camera_eye_dir.z * sin(0.01));
}

void look_down()
{
    camera_eye_dir.x = (camera_eye_dir.x * cos(-0.01) + camera_up_dir.x * sin(-0.01));
    camera_eye_dir.y = (camera_eye_dir.y * cos(-0.01) + camera_up_dir.y * sin(-0.01));
    camera_eye_dir.z = (camera_eye_dir.z * cos(-0.01) + camera_up_dir.z * sin(-0.01));

    camera_up_dir.x = (camera_up_dir.x * cos(-0.01) - camera_eye_dir.x * sin(-0.01));
    camera_up_dir.y = (camera_up_dir.y * cos(-0.01) - camera_eye_dir.y * sin(-0.01));
    camera_up_dir.z = (camera_up_dir.z * cos(-0.01) - camera_eye_dir.z * sin(-0.01));
}

void tilt_counterclockwise()
{
    camera_up_dir.x = (camera_up_dir.x * cos(0.01) - camera_right_dir.x * sin(0.01));
    camera_up_dir.y = (camera_up_dir.y * cos(0.01) - camera_right_dir.y * sin(0.01));
    camera_up_dir.z = (camera_up_dir.z * cos(0.01) - camera_right_dir.z * sin(0.01));

    camera_right_dir.x = (camera_right_dir.x * cos(0.01) - camera_up_dir.x * sin(0.01));
    camera_right_dir.y = (camera_right_dir.y * cos(0.01) - camera_up_dir.y * sin(0.01));
    camera_right_dir.z = (camera_right_dir.z * cos(0.01) - camera_up_dir.z * sin(0.01));
}

void tilt_clockwise()
{
    camera_up_dir.x = (camera_up_dir.x * cos(-0.01) - camera_right_dir.x * sin(-0.01));
    camera_up_dir.y = (camera_up_dir.y * cos(-0.01) - camera_right_dir.y * sin(-0.01));
    camera_up_dir.z = (camera_up_dir.z * cos(-0.01) - camera_right_dir.z * sin(-0.01));

    camera_right_dir.x = (camera_right_dir.x * cos(-0.01) - camera_up_dir.x * sin(-0.01));
    camera_right_dir.y = (camera_right_dir.y * cos(-0.01) - camera_up_dir.y * sin(-0.01));
    camera_right_dir.z = (camera_right_dir.z * cos(-0.01) - camera_up_dir.z * sin(-0.01));
}

void move_forward()
{
    camera_pos.x += camera_eye_dir.x * 1.2;
    camera_pos.y += camera_eye_dir.y * 1.2;
    camera_pos.z += camera_eye_dir.z * 1.2;
}

void move_backward()
{
    camera_pos.x -= camera_eye_dir.x * 1.2;
    camera_pos.y -= camera_eye_dir.y * 1.2;
    camera_pos.z -= camera_eye_dir.z * 1.2;
}

void move_right()
{
    camera_pos.x += camera_right_dir.x * 1.2;
    camera_pos.y += camera_right_dir.y * 1.2;
    camera_pos.z += camera_right_dir.z * 1.2;
}

void move_left()
{
    camera_pos.x -= camera_right_dir.x * 1.2;
    camera_pos.y -= camera_right_dir.y * 1.2;
    camera_pos.z -= camera_right_dir.z * 1.2;
}

void move_up()
{
    camera_pos.x += camera_up_dir.x * 1.2;
    camera_pos.y += camera_up_dir.y * 1.2;
    camera_pos.z += camera_up_dir.z * 1.2;
}

void move_down()
{
    camera_pos.x -= camera_up_dir.x * 1.2;
    camera_pos.y -= camera_up_dir.y * 1.2;
    camera_pos.z -= camera_up_dir.z * 1.2;
}

/*
scene.txt file's information to draw a prism for bonus tasks. Here the prism is drawn by 8 triangles. We know a prism is enclosed with two parallel equilateral triangular and three rectangular surfaces. Here, two triangle is drawn in red color for the two triangular surfaces. And two rectangular surfaces are drawn in green color using 6 triangle(3 for each). And the floor is used for the other rectangular surface.



4
768

8
triangle
50 0 60
-50 0 60
50 -30 10
0.0 1.0 0.0
0.4 0.2 1.0 1.0
5

triangle
50 -30 10
-50 0 60
-50 -30 10
0.0 1.0 0.0
0.4 0.2 1.0 1.0
5

triangle
50 0 60
-50 0 60
50 30 10
0.0 1.0 0.0
0.4 0.2 1.0 1.0
5

triangle
50 30 10
-50 0 60
-50 30 10
0.0 1.0 0.0
0.4 0.2 1.0 1.0
5

triangle
50 30 10
50 -30 10
50 0 60
1.0 0.0 0.0
0.4 0.2 1.0 1.0
5

triangle
-50 30 10
-50 -30 10
-50 0 60
1.0 0.0 0.0
0.4 0.2 1.0 1.0
5

triangle
50 30 10
50 -30 10
-50 -30 10
0.0 1.0 0.0
0.4 0.2 1.0 1.0
5

triangle
50 30 10
-50 30 10
-50 -30 10
0.0 1.0 0.0
0.4 0.2 1.0 1.0
5

1
70.0 70.0 70.0
1.0 0.1 0.1

1
0 -50 45
1.0 1.0 1.0
0 1 0
12

*/

/*
given scene file:

4
768

8
sphere
40.0 0.0 10.0
10.0
0.0 1.0 0.0
0.4 0.2 0.2 0.2
10

sphere
-30.0 60.0 20.0
20.0
0.0 0.0 1.0
0.2 0.2 0.4 0.2
15

sphere
-15.0 15.0 45.0
15.0
1.0 1.0 0.0
0.4 0.3 0.1 0.2
5

triangle
50 30 0
70 60 0
50 45 50
1.0 0.0 0.0
0.4 0.2 0.1 0.3
5

triangle
70 60 0
30 60 0
50 45 50
0.0 1.0 0.0
0.4 0.2 0.1 0.3
5

triangle
30 60 0
50 30 0
50 45 50
0.0 0.0 1.0
0.4 0.2 0.1 0.3
5

general
1 1 1 0 0 0 0 0 0 -100
0 0 0 0 0 20
0.0 1.0 0.0
0.4 0.2 0.1 0.3
10

general
0.0625 0.04 0.04 0 0 0 0 0 0 -36
0 0 0 0 0 15
1.0 0.0 0.0
0.4 0.2 0.1 0.3
15

4
70.0 70.0 70.0
1.0 0.0 0.0
-70 70 70
0.0 0.0 1.0
70 -70 70
1 0 0.0
-70 -70 70
0 1.0 0

1
100 100 -200
0 1.0 0.0
0 0 1
12

*/