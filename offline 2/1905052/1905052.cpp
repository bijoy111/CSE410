#include<bits/stdc++.h>
#include "bitmap_image.hpp"
using namespace std;

FILE *scene,*stage1,*stage2,*stage3,*config,*zbuffer;

static unsigned long int g_seed = 1; 
inline int random()
{
g_seed = (214013 * g_seed + 2531011); 
return (g_seed >> 16) & 0x7FFF;
}

class point
{
public:
    double x,y,z,w;
    point()
    {
        x=0 , y=0 , z=0 , w=1;
    }
    point(double a,double b,double c)
    {
        x=a , y=b , z=c , w=1;
    }
    void normalize()
    {
        double d=sqrt(x*x+y*y+z*z);
        x=x/d , y=y/d , z=z/d;
    }
};

class matrix
{
public:
    double mat[4][4];
    matrix()
    {
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
                mat[i][j]=0;
        }
    }
    void identity()
    {
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                if(i==j)
                    mat[i][j]=1;
                else
                    mat[i][j]=0;
            }
        }
    }
    matrix multiply(matrix a)
    {
        matrix c;
        for(int i=0;i<4;i++)
        {
            for(int j=0;j<4;j++)
            {
                c.mat[i][j]=0;
                for(int k=0;k<4;k++)
                    c.mat[i][j]+=mat[i][k]*a.mat[k][j];
            }
        }
        return c;
    }
    point multiply(point a)
    {
        point c;
        c.x=mat[0][0]*a.x + mat[0][1]*a.y + mat[0][2]*a.z + mat[0][3]*a.w;
        c.y=mat[1][0]*a.x + mat[1][1]*a.y + mat[1][2]*a.z + mat[1][3]*a.w;
        c.z=mat[2][0]*a.x + mat[2][1]*a.y + mat[2][2]*a.z + mat[2][3]*a.w;
        c.w=mat[3][0]*a.x + mat[3][1]*a.y + mat[3][2]*a.z + mat[3][3]*a.w;
        return c;
    }
};

point add(point a,point b)
{
    point c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return c;
}

point sub(point a,point b)
{
    point c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return c;
}

point mul(point a,double b)
{
    point c;
    c.x = a.x * b;
    c.y = a.y * b;
    c.z = a.z * b;
    return c;
}

double dot(point a,point b)
{
    double c=0;
    c += a.x * b.x;
    c += a.y * b.y;
    c += a.z * b.z;
    return c;
}

point cross(point a,point b)
{
    point c;
    c.x = a.y*b.z - a.z*b.y;
    c.y = a.z*b.x - a.x*b.z;
    c.z = a.x*b.y - a.y*b.x;
    return c;
}

point eye,up,look;
double fovY,aspectRatio,near,far;

void Stage1()
{
    scene=fopen("scene.txt","r");
    stage1=fopen("stage1.txt","w");
    fscanf(scene,"%lf%lf%lf",&eye.x,&eye.y,&eye.z);
    fscanf(scene,"%lf%lf%lf",&look.x,&look.y,&look.z);
    fscanf(scene,"%lf%lf%lf",&up.x,&up.y,&up.z);
    fscanf(scene,"%lf%lf%lf%lf",&fovY,&aspectRatio,&near,&far);
    
    stack<matrix> stk;
    
    matrix transformation_matrix;
    transformation_matrix.identity();
    while(1)
    {
        char command[30];
        fscanf(scene,"%s",command);
        if(strcmp(command,"triangle")==0)
        {
            point a,b,c;
            fscanf(scene,"%lf%lf%lf",&a.x,&a.y,&a.z);
            fscanf(scene,"%lf%lf%lf",&b.x,&b.y,&b.z);
            fscanf(scene,"%lf%lf%lf",&c.x,&c.y,&c.z);
            a.w=1 , b.w=1 , c.w=1;

            matrix m=transformation_matrix;
            a=m.multiply(a);
            b=m.multiply(b);
            c=m.multiply(c);
            a.x=a.x/a.w , a.y=a.y/a.w , a.z=a.z/a.w , a.w=1;
            b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
            c.x=c.x/c.w , c.y=c.y/c.w , c.z=c.z/c.w , c.w=1;

            fprintf(stage1,"%lf %lf %lf\n",a.x,a.y,a.z);
            fprintf(stage1,"%lf %lf %lf\n",b.x,b.y,b.z);
            fprintf(stage1,"%lf %lf %lf\n",c.x,c.y,c.z);
            fprintf(stage1,"\n");
        }
        else if(strcmp(command,"translate")==0)
        {
            double tx,ty,tz;
            fscanf(scene,"%lf%lf%lf",&tx,&ty,&tz);
            matrix m;
            m.identity();
            m.mat[0][3]=tx , m.mat[1][3]=ty , m.mat[2][3]=tz;

            transformation_matrix=transformation_matrix.multiply(m);
        }
        else if(strcmp(command,"scale")==0)
        {
            double sx,sy,sz;
            fscanf(scene,"%lf%lf%lf",&sx,&sy,&sz);
            matrix m;
            m.identity();
            m.mat[0][0]=sx , m.mat[1][1]=sy , m.mat[2][2]=sz;

            transformation_matrix=transformation_matrix.multiply(m);
        }
        else if(strcmp(command,"rotate")==0)
        {
            double angle,ux,uy,uz;
            fscanf(scene,"%lf%lf%lf%lf",&angle,&ux,&uy,&uz);
            point p=point(ux,uy,uz);
            p.normalize();
            point i=point(1,0,0);
            point j=point(0,1,0);
            point k=point(0,0,1);
            double ang=((acos(-1.0)*angle)/180.0);

            point tmp1=mul(i,cos(ang));
            point tmp2=mul(p,((1-cos(ang))*dot(p,i)));
            point tmp3=cross(p,i);
            tmp3=mul(tmp3,sin(ang));

            point c1=add(tmp1,tmp2);
            c1=add(c1,tmp3);

            tmp1=mul(j,cos(ang));
            tmp2=mul(p,((1-cos(ang))*dot(p,j)));
            tmp3=cross(p,j);
            tmp3=mul(tmp3,sin(ang));

            point c2=add(tmp1,tmp2);
            c2=add(c2,tmp3);

            tmp1=mul(k,cos(ang));
            tmp2=mul(p,((1-cos(ang))*dot(p,k)));
            tmp3=cross(p,k);
            tmp3=mul(tmp3,sin(ang));

            point c3=add(tmp1,tmp2);
            c3=add(c3,tmp3);

            matrix m;
            m.identity();

            m.mat[0][0]=c1.x , m.mat[0][1]=c2.x , m.mat[0][2]=c3.x;
            m.mat[1][0]=c1.y , m.mat[1][1]=c2.y , m.mat[1][2]=c3.y;
            m.mat[2][0]=c1.z , m.mat[2][1]=c2.z , m.mat[2][2]=c3.z;

            transformation_matrix=transformation_matrix.multiply(m);
        }
        else if(strcmp(command,"push")==0)
        {
            stk.push(transformation_matrix);
        }
        else if(strcmp(command,"pop")==0)
        {
            transformation_matrix=stk.top();
            stk.pop();
        }
        else if(strcmp(command,"end")==0)
        {
            break;
        }
    }
    fclose(scene);
    fclose(stage1);
}

void Stage2()
{
    //cout<<eye.x<<" "<<eye.y<<" "<<eye.z<<endl;
    //cout<<look.x<<" "<<look.y<<" "<<look.z<<endl;
    //cout<<up.x<<" "<<up.y<<" "<<up.z<<endl;

    stage1=fopen("stage1.txt","r");
    stage2=fopen("stage2.txt","w");

    point l=sub(look,eye);

    if(l.x!=0||l.y!=0||l.z!=0)
        l.normalize();
    //cout<<l.x<<" "<<l.y<<" "<<l.z<<endl;
    point r=cross(l,up);
    //cout<<r.x<<" "<<r.y<<" "<<r.z<<endl;
    if(r.x!=0||r.y!=0||r.z!=0)
        r.normalize();
    //cout<<r.x<<" "<<r.y<<" "<<r.z<<endl;
    point u=cross(r,l);

    matrix T;
    T.identity();
    T.mat[0][3]=-eye.x , T.mat[1][3]=-eye.y , T.mat[2][3]=-eye.z;

    matrix R;
    R.identity();
    R.mat[0][0]=r.x , R.mat[0][1]=r.y , R.mat[0][2]=r.z , R.mat[0][3]=0;
    R.mat[1][0]=u.x , R.mat[1][1]=u.y , R.mat[1][2]=u.z , R.mat[1][3]=0;
    R.mat[2][0]=-l.x , R.mat[2][1]=-l.y , R.mat[2][2]=-l.z , R.mat[2][3]=0;
    R.mat[3][0]=0 , R.mat[3][1]=0 , R.mat[3][2]=0 , R.mat[3][3]=1;

    matrix view=R.multiply(T);
   
    point a;
    while(fscanf(stage1,"%lf%lf%lf",&a.x,&a.y,&a.z)!=EOF)
    {
        a.w=1;
        point b=view.multiply(a);
        b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
        fprintf(stage2,"%lf %lf %lf\n",b.x,b.y,b.z);

        fscanf(stage1,"%lf%lf%lf",&a.x,&a.y,&a.z);
        a.w=1;
        b=view.multiply(a);
        b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
        fprintf(stage2,"%lf %lf %lf\n",b.x,b.y,b.z);

        fscanf(stage1,"%lf%lf%lf",&a.x,&a.y,&a.z);
        a.w=1;
        b=view.multiply(a);
        b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
        fprintf(stage2,"%lf %lf %lf\n",b.x,b.y,b.z);
        fprintf(stage2,"\n");
    }

    fclose(stage1);
    fclose(stage2);
}

void Stage3()
{
    stage2=fopen("stage2.txt","r");
    stage3=fopen("stage3.txt","w");
    double fovX=fovY*aspectRatio;

    double angY=((acos(-1.0)*fovY)/180.0);
    double angX=((acos(-1.0)*fovX)/180.0);

    double t=near*tan(angY/2);
    double r=near*tan(angX/2);
    
    matrix projection;
    projection.mat[0][0]=near/r , projection.mat[0][1]=0 , projection.mat[0][2]=0 , projection.mat[0][3]=0;
    projection.mat[1][0]=0 , projection.mat[1][1]=near/t , projection.mat[1][2]=0 , projection.mat[1][3]=0;
    projection.mat[2][0]=0 , projection.mat[2][1]=0 , projection.mat[2][2]=(-(far+near))/(far-near) , projection.mat[2][3]=(-(2*far*near))/(far-near);
    projection.mat[3][0]=0 , projection.mat[3][1]=0 , projection.mat[3][2]=-1 , projection.mat[3][3]=0;

    point a;
    while(fscanf(stage2,"%lf%lf%lf",&a.x,&a.y,&a.z)!=EOF)
    {
        a.w=1;
        point b=projection.multiply(a);
        b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
        fprintf(stage3,"%.7lf %.7lf %.7lf\n",b.x,b.y,b.z);

        fscanf(stage2,"%lf%lf%lf",&a.x,&a.y,&a.z);
        a.w=1;
        b=projection.multiply(a);
        b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
        fprintf(stage3,"%.7lf %.7lf %.7lf\n",b.x,b.y,b.z);

        fscanf(stage2,"%lf%lf%lf",&a.x,&a.y,&a.z);
        a.w=1;
        b=projection.multiply(a);
        b.x=b.x/b.w , b.y=b.y/b.w , b.z=b.z/b.w , b.w=1;
        fprintf(stage3,"%.7lf %.7lf %.7lf\n",b.x,b.y,b.z);
        fprintf(stage3,"\n");
    }
    fclose(stage2);
    fclose(stage3);
}

class color
{
public:
    int r,g,b;
    color()
    {
        r=0 , g=0 , b=0;
    }
    color(int x,int y,int z)
    {
        r=x , g=y , b=z;
    }
};

void Stage4()
{
    stage3=fopen("stage3.txt","r");
    config=fopen("config.txt","r");
    zbuffer=fopen("z_buffer.txt","w");

    int width,height;
    fscanf(config,"%d%d",&width,&height);
    fclose(config);

    int leftLimit=-1;
    int rightLimit=1;
    int bottomLimit=-1;
    int topLimit=1;

    double dx=(rightLimit-leftLimit)*1.0/width;
    double dy=(topLimit-bottomLimit)*1.0/height;

    double topY=topLimit-dy/2.0;
    double bottomY=bottomLimit+dy/2.0;
    double leftX=leftLimit+dx/2.0;
    double rightX=rightLimit-dx/2.0;

    int zMin=-1;
    int zMax=1;

    double** zBuffer=new double*[width];
    color** frameBuffer=new color*[width];
    for(int i=0;i<width;i++)
    {
        zBuffer[i]=new double[height];
        frameBuffer[i]=new color[height];
    }
    for(int i=0;i<width;i++)
    {
        for(int j=0;j<height;j++)
        {
            zBuffer[i][j]=zMax;
            frameBuffer[i][j]=color();
        }
    }

    bitmap_image img(width,height);

    point a,b,c;
    while(fscanf(stage3,"%lf%lf%lf",&a.x,&a.y,&a.z)!=EOF)
    {
        fscanf(stage3,"%lf%lf%lf",&b.x,&b.y,&b.z);
        fscanf(stage3,"%lf%lf%lf",&c.x,&c.y,&c.z);

        color col = color(abs(random()) % 255, abs(random()) % 255, abs(random()) % 255);

        double maxY=max(a.y,max(b.y,c.y));
        double minY=min(a.y,min(b.y,c.y));
        int topScanline=(int)((topY-min(topY,maxY))/dy);
        int bottomScanline=height-(int)((max(bottomY,minY)-bottomY)/dy);

        for(int i=topScanline;i<=bottomScanline;i++)
        {
            double yp=topY-i*dy;
            double xa,xb,za,zb;

            int cont=0;
            if(yp>min(a.y,b.y)&&yp<max(a.y,b.y))
            {
                za=a.z+(yp-a.y)*(b.z-a.z)/(b.y-a.y);
                xa=a.x+(yp-a.y)*(b.x-a.x)/(b.y-a.y);
                if(yp>min(a.y,c.y)&&yp<max(a.y,c.y))
                {
                    zb=a.z+(yp-a.y)*(c.z-a.z)/(c.y-a.y);
                    xb=a.x+(yp-a.y)*(c.x-a.x)/(c.y-a.y);
                }
                else if(yp>min(b.y,c.y)&&yp<max(b.y,c.y))
                {
                    zb=b.z+(yp-b.y)*(c.z-b.z)/(c.y-b.y);
                    xb=b.x+(yp-b.y)*(c.x-b.x)/(c.y-b.y);
                }
                else
                {
                    cont=1;
                }
            }
            else if(yp>min(a.y,c.y)&&yp<max(a.y,c.y))
            {
                zb=a.z+(yp-a.y)*(c.z-a.z)/(c.y-a.y);
                xb=a.x+(yp-a.y)*(c.x-a.x)/(c.y-a.y);
                if(yp>min(b.y,c.y)&&yp<max(b.y,c.y))
                {
                    za=b.z+(yp-b.y)*(c.z-b.z)/(c.y-b.y);
                    xa=b.x+(yp-b.y)*(c.x-b.x)/(c.y-b.y);
                }
                else if(yp>min(a.y,b.y)&&yp<max(a.y,b.y))
                {
                    za=a.z+(yp-a.y)*(b.z-a.z)/(b.y-a.y);
                    xa=a.x+(yp-a.y)*(b.x-a.x)/(b.y-a.y);
                }
                else
                {
                    cont=1;
                }
            }
            else
            {
                continue;
            }
            if(cont==1)
            {
                continue;
            }
            
            if (xa > xb)
            {
                xa=xa+xb;
                xb=xa-xb;
                xa=xa-xb;

                za=za+zb;
                zb=za-zb;
                za=za-zb;
            }

            int left_intersecting_column=(int)((max(xa,leftX)-leftX)/dx);
            int right_intersecting_column=width-(int)((rightX-min(xb,rightX))/dx);
           
            for(int j=left_intersecting_column;j<=right_intersecting_column;j++)
            {
                double xp=leftX+j*dx;
                double zp=za+(xp-xa)*(zb-za)/(xb-xa);
                if(zp>zMin&&zp<zMax&&zp<zBuffer[i][j])
                {
                    zBuffer[i][j]=zp;
                    frameBuffer[i][j]=col;
                }
            }
        }     
    }
    fclose(stage3);

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            if(zBuffer[i][j]<zMax)
            {
                fprintf(zbuffer,"%0.6lf\t",zBuffer[i][j]);
            }
        }
        fprintf(zbuffer,"\n");
    }
    fclose(zbuffer);

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            img.set_pixel(j,i,frameBuffer[i][j].r,frameBuffer[i][j].g,frameBuffer[i][j].b);
        }
    }

    img.save_image("out.bmp");

    //free memory of zBuffer and frameBuffer
    for(int i=0;i<width;i++)
    {
        delete[] zBuffer[i];
        delete[] frameBuffer[i];
    }
    delete[] zBuffer;
    delete[] frameBuffer;

}

int main() 
{   
    Stage1();
    Stage2();
    Stage3();
    Stage4();
    return 0;
}