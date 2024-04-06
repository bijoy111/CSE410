#include <bits/stdc++.h>
#include "1905052_bitmap_image.hpp"
#ifdef __linux__
#include <GL/glut.h>
#elif WIN64
#include <GL/glut.h>
#include <windows.h>
#endif
using namespace std;

class Vector3D
{
public:
    double x, y, z, w;
    Vector3D()
    {
        x = 0, y = 0, z = 0, w = 1;
    }
    Vector3D(double a, double b, double c)
    {
        x = a, y = b, z = c, w = 1;
    }
    Vector3D(double a, double b, double c, double d)
    {
        x = a, y = b, z = c, w = d;
    }
    Vector3D(const Vector3D &v)
    {
        x = v.x, y = v.y, z = v.z, w = v.w;
    }
    Vector3D operator+(Vector3D v)
    {
        return Vector3D(this->x + v.x, this->y + v.y, this->z + v.z);
    }
    Vector3D operator-(Vector3D v)
    {
        return Vector3D(this->x - v.x, this->y - v.y, this->z - v.z);
    }
    Vector3D operator-()
    {
        return Vector3D(-this->x, -this->y, -this->z);
    }
    Vector3D operator*(double c)
    {
        return Vector3D(this->x * c, this->y * c, this->z * c);
    }
    Vector3D operator/(double c)
    {
        return Vector3D(this->x / c, this->y / c, this->z / c);
    }
    double operator*(Vector3D v)
    {
        return (this->x * v.x + this->y * v.y + this->z * v.z);
    }
    Vector3D operator^(Vector3D v)
    {
        return Vector3D(this->y * v.z - this->z * v.y, this->z * v.x - this->x * v.z, this->x * v.y - this->y * v.x);
    }
    void normalize()
    {
        double d = sqrt(x * x + y * y + z * z);
        x = x / d, y = y / d, z = z / d;
    }
    double length()
    {
        return sqrt(x * x + y * y + z * z);
    }

    friend ostream &operator<<(ostream &output, Vector3D v)
    {
        output << "(" << v.x << "," << v.y << "," << v.z << ")"
               << " : " << v.w;
        return output;
    }
    friend istream &operator>>(istream &input, Vector3D &v)
    {
        input >> v.x >> v.y >> v.z;
        return input;
    }
    friend ofstream &operator<<(ofstream &output, Vector3D &v)
    {
        output << fixed << setprecision(7) << v.x << " " << v.y << " " << v.z;
        return output;
    }
};

class Color
{
public:
    double r, g, b;
    Color()
    {
        r = 0, g = 0, b = 0;
    }
    Color(double x, double y, double z)
    {
        r = x, g = y, b = z;
    }
};

class PointLight
{
public:
    Vector3D lightPos;
    Color color;
    PointLight()
    {
        lightPos = Vector3D(0, 0, 0);
    }
    PointLight(Vector3D p)
    {
        lightPos = p;
    }
    void setColor(Color c)
    {
        color = c;
    }
    void draw()
    {
        glPointSize(8);
        glBegin(GL_POINTS);
        glColor3f(color.r, color.g, color.b);
        glVertex3f(lightPos.x, lightPos.y, lightPos.z);
        glEnd();
    }

    friend istream &operator>>(istream &input, PointLight &pl)
    {
        input >> pl.lightPos.x >> pl.lightPos.y >> pl.lightPos.z;
        input >> pl.color.r >> pl.color.g >> pl.color.b;
        return input;
    }
};

class SpotLight
{
public:
    PointLight point_light;
    Vector3D lightDir;
    double cutOffAngle;
    SpotLight()
    {
        point_light = PointLight();
        lightDir = Vector3D(0, 0, 0);
        cutOffAngle = 0;
    }
    SpotLight(PointLight pl, Vector3D d, double angle)
    {
        point_light = pl;
        lightDir = d;
        cutOffAngle = angle;
    }
    void draw()
    {
        glPointSize(16);
        glBegin(GL_POINTS);
        glColor3f(point_light.color.r, point_light.color.g, point_light.color.b);
        glVertex3f(point_light.lightPos.x, point_light.lightPos.y, point_light.lightPos.z);
        glEnd();
    }

    friend istream &operator>>(istream &input, SpotLight &sl)
    {
        input >> sl.point_light.lightPos;
        input >> sl.point_light.color.r >> sl.point_light.color.g >> sl.point_light.color.b;
        input >> sl.lightDir;
        input >> sl.cutOffAngle;
        return input;
    }
};

class Ray
{
public:
    Vector3D start, dir;
    Ray()
    {
        start = Vector3D(0, 0, 0);
        dir = Vector3D(0, 0, 0);
    }
    Ray(Vector3D s, Vector3D d)
    {
        start = s;
        // normalize for easier calculations
        d.normalize();
        dir = d;
    }

    friend ostream &operator<<(ostream &output, Ray ray)
    {
        output << "Origin : " << ray.start << ", Direction : " << ray.dir;
        return output;
    }
};

class Object;
extern vector<Object *> objects;
extern vector<PointLight *> point_lights;
extern vector<SpotLight *> spot_lights;
extern int recursion_level;
static int refrection;
class Object
{
public:
    Vector3D reference_point; // reference_point should have x,y,z
    double height, width, length;
    Color color;                 // color should have r,g,b values
    vector<double> coEfficients; // coEfficients should have 4 values for ambient, diffuse, specular and reflection coefficients
    int shine;                   // exponent term for specular component

    Object()
    {
        for (int i = 1; i <= 4; i++)
        {
            coEfficients.push_back(0.0);
        }
    }
    virtual void draw() = 0;
    void setColor(Color c)
    {
        color = c;
    }
    void setShine(int s)
    {
        shine = s;
    }
    void setCoEfficients(vector<double> co)
    {
        coEfficients = co;
    }

    virtual Color getColoratpnt(Vector3D p)
    {
        return Color(color.r, color.g, color.b);
    }
    virtual double getIntersectingTValue(Ray r, Color &c, int l) = 0;
    virtual Ray getNormal(Vector3D p, Ray r) = 0;
    double intersect(Ray r, Color &c, int l)
    {
        double t = getIntersectingTValue(r, c, l);
        if (t < 0)
        {
            return -1;
        }
        if (l == 0)
        {
            return t;
        }
        Vector3D ip = r.start + r.dir * t;
        Color ic = getColoratpnt(ip);
        c.r = ic.r * coEfficients[0];
        c.g = ic.g * coEfficients[0];
        c.b = ic.b * coEfficients[0];

        for (int i = 0; i < point_lights.size(); i++)
        {
            Vector3D lp = point_lights[i]->lightPos;
            Vector3D ld = ip - lp;
            ld.normalize();
            Ray r1 = Ray(lp, ld);
            Ray n1 = getNormal(ip, r1);
            double tmp1 = (ip - ld).length();
            if (tmp1 < 1e-5)
            {
                continue;
            }
            bool flag = false;
            for (int j = 0; j < objects.size(); j++)
            {
                if (objects[j]->getIntersectingTValue(r1, c, 0) > 0 && objects[j]->getIntersectingTValue(r1, c, 0) + 1e-5 < tmp1)
                {
                    flag = true;
                    break;
                }
            }
            if (!flag)
            {
                double lambert = max(0.0, -r1.dir * n1.dir);
                Ray reflection = Ray(ip, r1.dir - n1.dir * 2 * (r1.dir * n1.dir));

                double phong = max(0.0, -r.dir * reflection.dir);

                c.r += lambert * ic.r * coEfficients[1] * point_lights[i]->color.r;
                c.g += lambert * ic.g * coEfficients[1] * point_lights[i]->color.g;
                c.b += lambert * ic.b * coEfficients[1] * point_lights[i]->color.b;

                c.r += pow(phong, shine) * coEfficients[2] * point_lights[i]->color.r * ic.r;
                c.g += pow(phong, shine) * coEfficients[2] * point_lights[i]->color.g * ic.g;
                c.b += pow(phong, shine) * coEfficients[2] * point_lights[i]->color.b * ic.b;
            }
        }

        for (int i = 0; i < spot_lights.size(); i++)
        {
            Vector3D lp = spot_lights[i]->point_light.lightPos;
            Vector3D ld = ip - lp;
            ld.normalize();
            double tmp0 = ld * spot_lights[i]->lightDir;
            double angle = acos(tmp0 / (ld.length() * spot_lights[i]->lightDir.length())) * 180 / acos(-1);
            if (fabs(angle) < spot_lights[i]->cutOffAngle)
            {
                continue;
            }
            Ray r1 = Ray(lp, ld);
            Ray n1 = getNormal(ip, r1);
            double tmp1 = (ip - ld).length();
            if (tmp1 < 1e-5)
            {
                continue;
            }
            bool flag = false;
            for (int j = 0; j < objects.size(); j++)
            {
                if (objects[j]->getIntersectingTValue(r1, c, 0) > 0 && objects[j]->getIntersectingTValue(r1, c, 0) + 1e-5 < tmp1)
                {
                    flag = true;
                    break;
                }
            }
            if (!flag)
            {
                double lambert = max(0.0, -r1.dir * n1.dir);
                Ray reflection = Ray(ip, r1.dir - n1.dir * 2 * (r1.dir * n1.dir));
                double phong = max(0.0, -r.dir * reflection.dir);

                c.r += lambert * ic.r * coEfficients[1] * spot_lights[i]->point_light.color.r;
                c.g += lambert * ic.g * coEfficients[1] * spot_lights[i]->point_light.color.g;
                c.b += lambert * ic.b * coEfficients[1] * spot_lights[i]->point_light.color.b;

                c.r += pow(phong, shine) * coEfficients[2] * spot_lights[i]->point_light.color.r * ic.r;
                c.g += pow(phong, shine) * coEfficients[2] * spot_lights[i]->point_light.color.g * ic.g;
                c.b += pow(phong, shine) * coEfficients[2] * spot_lights[i]->point_light.color.b * ic.b;
            }
        }
        if (l < recursion_level)
        {
            Ray n1 = getNormal(ip, r);
            Ray reflection = Ray(ip, r.dir - n1.dir * 2 * (r.dir * n1.dir));
            reflection.start = reflection.start + reflection.dir * 1e-5;

            int tmp1 = -1;
            double tmp2 = -1;
            double mint = 1e9;

            for (int j = 0; j < objects.size(); j++)
            {
                tmp2 = objects[j]->intersect(reflection, c, 0);
                if (tmp2 > 0 && tmp2 < mint)
                {
                    mint = tmp2;
                    tmp1 = j;
                }
            }
            if (tmp1 != -1)
            {
                Color color_tmp(0, 0, 0);
                double tmp3 = objects[tmp1]->intersect(reflection, color_tmp, l + 1);
                c.r += color_tmp.r * coEfficients[3];
                c.g += color_tmp.g * coEfficients[3];
                c.b += color_tmp.b * coEfficients[3];
            }

            // bonus code starts
            // https://stackoverflow.com/questions/29758545/how-to-find-refraction-vector-from-incoming-vector-and-surface-normal/58676386#58676386
            if (refrection)
            {
                double ri = 1.5;
                double cosI = -(n1.dir * r.dir);
                double sinT2 = ri * ri * (1.0 - cosI * cosI);
                if (sinT2 <= 1.0)
                {
                    double cosT = sqrt(1.0 - sinT2);
                    Vector3D refrection_dir = r.dir * ri + n1.dir * (ri * cosI - cosT);
                    Ray refrection = Ray(ip, refrection_dir);
                    refrection.start = refrection.start + refrection.dir * 1e-5;

                    tmp1 = -1;
                    tmp2 = -1;
                    mint = 1e9;

                    for (int j = 0; j < objects.size(); j++)
                    {
                        tmp2 = objects[j]->intersect(refrection, c, 0);
                        if (tmp2 > 0 && tmp2 < mint)
                        {
                            mint = tmp2;
                            tmp1 = j;
                        }
                    }
                    if (tmp1 != -1)
                    {
                        Color color_tmp(0, 0, 0);
                        double tmp3 = objects[tmp1]->intersect(refrection, color_tmp, l + 1);
                        c.r += color_tmp.r * coEfficients[3];
                        c.g += color_tmp.g * coEfficients[3];
                        c.b += color_tmp.b * coEfficients[3];
                    }
                }
            }
            // bonus code ends
        }
        return t;
    }
};

class Triangle : public Object
{
public:
    Vector3D a, b, c;
    Triangle() {}
    Triangle(Vector3D x, Vector3D y, Vector3D z)
    {
        a = x;
        b = y;
        c = z;
    }
    void draw()
    {
        glBegin(GL_TRIANGLES);
        glColor3f(color.r, color.g, color.b);
        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);
        glEnd();
    }
    double getIntersectingTValue(Ray r, Color &col, int l)
    {
        double A_Matrix[3][3] =
            {
                {a.x - b.x, a.x - c.x, r.dir.x},
                {a.y - b.y, a.y - c.y, r.dir.y},
                {a.z - b.z, a.z - c.z, r.dir.z}};

        double Beta_Matrix[3][3] =
            {
                {a.x - r.start.x, a.x - c.x, r.dir.x},
                {a.y - r.start.y, a.y - c.y, r.dir.y},
                {a.z - r.start.z, a.z - c.z, r.dir.z}};
        double Gamma_Matrix[3][3] =
            {
                {a.x - b.x, a.x - r.start.x, r.dir.x},
                {a.y - b.y, a.y - r.start.y, r.dir.y},
                {a.z - b.z, a.z - r.start.z, r.dir.z}};
        double T_Matrix[3][3] =
            {
                {a.x - b.x, a.x - c.x, a.x - r.start.x},
                {a.y - b.y, a.y - c.y, a.y - r.start.y},
                {a.z - b.z, a.z - c.z, a.z - r.start.z}};

        double A_det = A_Matrix[0][0] * (A_Matrix[1][1] * A_Matrix[2][2] - A_Matrix[1][2] * A_Matrix[2][1]) - A_Matrix[0][1] * (A_Matrix[1][0] * A_Matrix[2][2] - A_Matrix[1][2] * A_Matrix[2][0]) + A_Matrix[0][2] * (A_Matrix[1][0] * A_Matrix[2][1] - A_Matrix[1][1] * A_Matrix[2][0]);

        double Beta_det = Beta_Matrix[0][0] * (Beta_Matrix[1][1] * Beta_Matrix[2][2] - Beta_Matrix[1][2] * Beta_Matrix[2][1]) - Beta_Matrix[0][1] * (Beta_Matrix[1][0] * Beta_Matrix[2][2] - Beta_Matrix[1][2] * Beta_Matrix[2][0]) + Beta_Matrix[0][2] * (Beta_Matrix[1][0] * Beta_Matrix[2][1] - Beta_Matrix[1][1] * Beta_Matrix[2][0]);

        double Gamma_det = Gamma_Matrix[0][0] * (Gamma_Matrix[1][1] * Gamma_Matrix[2][2] - Gamma_Matrix[1][2] * Gamma_Matrix[2][1]) - Gamma_Matrix[0][1] * (Gamma_Matrix[1][0] * Gamma_Matrix[2][2] - Gamma_Matrix[1][2] * Gamma_Matrix[2][0]) + Gamma_Matrix[0][2] * (Gamma_Matrix[1][0] * Gamma_Matrix[2][1] - Gamma_Matrix[1][1] * Gamma_Matrix[2][0]);

        double T_det = T_Matrix[0][0] * (T_Matrix[1][1] * T_Matrix[2][2] - T_Matrix[1][2] * T_Matrix[2][1]) - T_Matrix[0][1] * (T_Matrix[1][0] * T_Matrix[2][2] - T_Matrix[1][2] * T_Matrix[2][0]) + T_Matrix[0][2] * (T_Matrix[1][0] * T_Matrix[2][1] - T_Matrix[1][1] * T_Matrix[2][0]);

        double beta = Beta_det / A_det;
        double gamma = Gamma_det / A_det;
        double t = T_det / A_det;

        if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0)
        {
            return t;
        }
        else
        {
            return -1;
        }
    }
    Ray getNormal(Vector3D p, Ray r)
    {
        Vector3D normal = (b - a) ^ (c - a);
        normal.normalize();
        if (r.dir * normal < 0)
        {
            return Ray(p, -normal);
        }
        else
        {
            return Ray(p, normal);
        }
    }

    friend istream &operator>>(istream &input, Triangle &triangle)
    {
        input >> triangle.a >> triangle.b >> triangle.c;
        input >> triangle.color.r >> triangle.color.g >> triangle.color.b;
        for (int i = 1; i <= 4; i++)
        {
            input >> triangle.coEfficients[i - 1];
        }
        input >> triangle.shine;
        return input;
    }
};

class Sphere : public Object
{
public:
    Sphere() {}
    Sphere(Vector3D center, double radius)
    {
        reference_point = center;
        length = radius;
    }
    void draw()
    {
        int stacks = 60;
        int sectors = 60;

        Vector3D points[80][80];
        const double pi = acos(-1.0);
        // getting points
        for (int i = 0; i <= stacks; i++)
        {
            double h = length * sin(((double)i / (double)stacks) * (pi / 2));
            double r = length * cos(((double)i / (double)stacks) * (pi / 2));
            for (int j = 0; j <= sectors; j++)
            {
                points[i][j].x = r * cos(((double)j / (double)sectors) * 2 * pi);
                points[i][j].y = r * sin(((double)j / (double)sectors) * 2 * pi);
                points[i][j].z = h;
            }
        }
        // quads drawing using points
        for (int i = 0; i < stacks; i++)
        {
            glPushMatrix();
            glTranslatef(reference_point.x, reference_point.y, reference_point.z);
            glColor3f(color.r, color.g, color.b);
            for (int j = 0; j < sectors; j++)
            {
                glBegin(GL_QUADS);
                {
                    // upper side hemisphere
                    glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
                    // lower side hemisphere
                    glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
                    glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
                    glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
                    glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
                }
                glEnd();
            }
            glPopMatrix();
        }
    }
    double getIntersectingTValue(Ray r, Color &col, int l)
    {
        r.start = r.start - reference_point;
        double a = 1;
        double b = 2 * (r.dir * r.start);
        double c = (r.start * r.start) - pow(length, 2);
        double d = sqrt(pow(b, 2) - 4 * a * c);
        double tmp = -1;
        if (d < 0)
        {
            tmp = -1;
        }
        else
        {
            if (fabs(a) < 1e-5)
            {
                tmp = -c / b;
                return tmp;
            }
            double t1 = (-b + d) / (2 * a);
            double t2 = (-b - d) / (2 * a);
            if (t1 < t2)
            {
                swap(t1, t2);
            }
            if (t2 > 0)
            {
                tmp = t2;
            }
            else if (t1 > 0)
            {
                tmp = t1;
            }
            else
            {
                tmp = -1;
            }
        }
        return tmp;
    }
    Ray getNormal(Vector3D p, Ray r)
    {
        return Ray(p, p - reference_point);
    }

    friend std::istream &operator>>(std::istream &input, Sphere &sphere)
    {
        input >> sphere.reference_point >> sphere.length;
        input >> sphere.color.r >> sphere.color.g >> sphere.color.b;
        for (int i = 1; i <= 4; i++)
        {
            input >> sphere.coEfficients[i - 1];
        }
        input >> sphere.shine;
        return input;
    }
};

class GeneralQuadratic : public Object
{
public:
    double A, B, C, D, E, F, G, H, I, J;
    GeneralQuadratic() {}
    void draw() {}
    double getIntersectingTValue(Ray r, Color &col, int l)
    {
        double sx = r.start.x;
        double sy = r.start.y;
        double sz = r.start.z;
        double dx = r.dir.x;
        double dy = r.dir.y;
        double dz = r.dir.z;
        double a = A * dx * dx + B * dy * dy + C * dz * dz + D * dx * dy + E * dy * dz + F * dz * dx;
        double b = 2 * (A * sx * dx + B * sy * dy + C * sz * dz + D * (sx * dy + sy * dx) + E * (sy * dz + sz * dy) + F * (sz * dx + sx * dz)) + G * dx + H * dy + I * dz;
        double c = A * sx * sx + B * sy * sy + C * sz * sz + D * sx * sy + E * sy * sz + F * sz * sx + G * sx + H * sy + I * sz + J;
        double d = sqrt(pow(b, 2) - 4 * a * c);
        if (d < 0)
        {
            return -1;
        }
        if (fabs(a) < 1e-5)
        {
            return -c / b;
        }
        double t1 = (-b + d) / (2 * a);
        double t2 = (-b - d) / (2 * a);
        if (t1 < 0 && t2 < 0)
        {
            return -1;
        }
        if (t1 < t2)
        {
            swap(t1, t2);
        }
        if (t2 > 0)
        {
            Vector3D ip = r.start + r.dir * t2;
            if (!(
                    (fabs(length) > 1e-5 && (ip.x < reference_point.x || ip.x > (reference_point.x + length))) ||
                    (fabs(width) > 1e-5 && (ip.y < reference_point.y || ip.y > (reference_point.y + width))) ||
                    (fabs(height) > 1e-5 && (ip.z < reference_point.z || ip.z > (reference_point.z + height)))))
            {
                return t2;
            }
        }
        if (t1 > 0)
        {
            Vector3D ip = r.start + r.dir * t1;
            if (!(
                    (fabs(length) > 1e-5 && (ip.x < reference_point.x || ip.x > (reference_point.x + length))) ||
                    (fabs(width) > 1e-5 && (ip.y < reference_point.y || ip.y > (reference_point.y + width))) ||
                    (fabs(height) > 1e-5 && (ip.z < reference_point.z || ip.z > (reference_point.z + height)))))
            {
                return t1;
            }
        }
        return -1;
    }
    virtual Ray getNormal(Vector3D p, Ray r)
    {
        Vector3D dir(
            2 * A * p.x + D * p.y + F * p.z + G,
            2 * B * p.y + D * p.x + E * p.z + H,
            2 * C * p.z + E * p.y + F * p.x + I);
        return Ray(p, dir);
    }

    friend istream &operator>>(istream &input, GeneralQuadratic &gq)
    {
        input >> gq.A >> gq.B >> gq.C >> gq.D >> gq.E >> gq.F >> gq.G >> gq.H >> gq.I >> gq.J;
        input >> gq.reference_point >> gq.length >> gq.width >> gq.height;
        input >> gq.color.r >> gq.color.g >> gq.color.b;
        for (int i = 1; i <= 4; i++)
        {
            input >> gq.coEfficients[i - 1];
        }
        input >> gq.shine;
        return input;
    }
};

class Floor : public Object
{
public:
    int no_of_tiles;
    Floor(int floorWidth, int tileWidth)
    {
        reference_point = Vector3D(-floorWidth / 2, -floorWidth / 2, 0);
        length = tileWidth;
        no_of_tiles = floorWidth / tileWidth;
    }
    void draw()
    {
        for (int i = 0; i < no_of_tiles; i++)
        {
            for (int j = 0; j < no_of_tiles; j++)
            {
                if ((i + j) % 2 != 0)
                {
                    glColor3f(0, 0, 0);
                }
                else
                {
                    glColor3f(1, 1, 1);
                }
                glBegin(GL_QUADS);
                glVertex3f(reference_point.x + i * length, reference_point.y + j * length, 0);
                glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, 0);
                glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, 0);
                glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, 0);
                glEnd();
            }
        }
    }
    double getIntersectingTValue(Ray r, Color &c, int l)
    {
        Vector3D normal = Vector3D(0.0, 0.0, 1.0);
        if ((normal * r.dir) == 0.0)
        {
            return -1.0;
        }
        double t = -(normal * r.start) / (normal * r.dir);
        Vector3D p = r.start + r.dir * t;
        if (p.x <= reference_point.x || p.x >= abs(reference_point.x) && p.y <= reference_point.y && p.y >= abs(reference_point.y))
        {
            return -1;
        }
        return t;
    }
    Color getColoratpnt(Vector3D p)
    {
        int x = (p.x - reference_point.x) / length;
        int y = (p.y - reference_point.y) / length;
        if (x < 0 || x >= no_of_tiles || y < 0 || y >= no_of_tiles)
        {
            return Color(0, 0, 0);
        }
        if ((x + y) % 2 != 0)
        {
            return Color(0, 0, 0);
        }
        else
        {
            return Color(1, 1, 1);
        }
    }
    Ray getNormal(Vector3D p, Ray r)
    {
        if (r.dir.z > 0.0)
        {
            return Ray(p, Vector3D(0.0, 0.0, 1.0));
        }
        else
        {
            return Ray(p, Vector3D(0.0, 0.0, -1.0));
        }
    }
};