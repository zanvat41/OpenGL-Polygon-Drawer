#include <gl/glut.h>
#include <stdio.h>
#include <array>
#include <list>
#include <iostream>
#include <algorithm>

#define ABS(x)	(x < 0 ? -x : x)
#define SIGN(x)	(x < 0 ? -1 : (x > 0 ? 1 : 0))

class Point
{
    int Xvalue, Yvalue;
public:
    void xy(int x, int y)
    {
        Xvalue = x;
        Yvalue = y;
    }

    //return individual x y value
    int x() { return Xvalue; }
    int y() { return Yvalue; }
};

int polygons[1024]; // keeps track of number of points in n'th polygon
bool pit[1024]; // keeps track if n'th polygon has self-intersection
bool printPol[1024]; // keeps track if n'th polygon should be hidden;
Point point[1024];
int pcount = 0; // count of polygons
int count = 0; // count of points in the current polygon

void Display()
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT); // clear display window

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const double w = glutGet( GLUT_WINDOW_WIDTH );
    const double h = glutGet( GLUT_WINDOW_HEIGHT );
    gluOrtho2D(0.0, w, 0.0, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1.0, 1.0, 1.0);

    glPointSize(3.0f);
    glBegin(GL_POINTS);
	// loop polygons
	int init = 0;
	for(int j = 0; j <= pcount; j++){
		//printf("%d, %d, %d\n", count, pcount, j);
		//init = count - polygons[j];
		//printf("-------\n");

		// loop points in that polygon
		for (int i = init; i < init + polygons[j]; i++)
		{
			//printf("%d, %d\n", point[i].x(), point[i].y());
			int x = point[i].x();
			int y = point[i].y();

			if(pit[j])
				glColor3f(1.0, 0, 0);
			else
				glColor3f(0.3, 0.3, 0.3);
			if(!printPol[j])
				glVertex2i(x, h - y);

			// draw line with midpoint algorithm
			if(i > init) {
				//printf("%d,\n", i);
				int	yi,xi,dy,dx,sx,sy;
				int	decision,incE,incNE;
 
				dx = point[i-1].x() - point[i].x();
				dy = (h-point[i-1].y()) - (h-point[i].y());
 
				sx = SIGN(dx);
				sy = SIGN(dy);
 
				dx = ABS(dx);
				dy = ABS(dy);
 
				if(dy > dx)
				{	
					incE = 2 * dx;
					incNE = 2 * dx - 2 * dy;
					decision = 2 * dy - dx;
 
					xi = point[i].x();
					yi = h-point[i].y();
					do{
						if(pit[j])
							glColor3f(1.0, 0, 0);
						else
							glColor3f(0.3, 0.3, 0.3);
						if(!printPol[j])
							glVertex2i(xi, yi);

						if(decision <= 0)
							decision += incE;
						else{
							decision += incNE;
							xi += sx;	
						}
						yi += sy;
					}while(yi != h-point[i-1].y());
				}else{
					incE = 2 * dy;
					incNE = 2 * dy - 2 * dx;
					decision = 2 * dy - dx;
 
					xi = point[i].x();
					yi = h-point[i].y();
					do{
						if(pit[j])
							glColor3f(1.0, 0, 0);
						else
							glColor3f(0.3, 0.3, 0.3);
						if(!printPol[j])
							glVertex2i(xi, yi);

						if(decision <= 0)
							decision += incE;
						else{
							decision += incNE;
							yi += sy;
						}
						xi += sx;
					}while(xi != point[i-1].x());
				}
				// end of draw line
			}
		}
		init += polygons[j];
	}
    glEnd();    
	
    glFlush();
}

int f(int x, int y, int x1, int y1, int x2, int y2) {
	return (x-x1)*(y2-y1)-(y-y1)*(x2-x1);
}

int g(int x, int y, int x3, int y3, int x4, int y4) {
	return (x-x3)*(y4-y3)-(y-y3)*(x4-x3);
}

// checks if the current polygon is simple
void checkSimple() {
	for(int i = count - polygons[pcount] + 1; i < count; i++) {
		// self-intersect occurs when there are more than 3 line segments
		for(int j = i + 1; j < count; j++) {
		    int x1, x2, x3, x4, y1, y2, y3, y4;
			const double h = glutGet( GLUT_WINDOW_HEIGHT );
			x1 = point[i-1].x();
			y1 = h-point[i-1].y();
			x2 = point[i].x();
			y2 = h-point[i].y();
			x3 = point[j-1].x();
			y3 = h-point[j-1].y();
			x4 = point[j].x();
			y4 = h-point[j].y();

			bool intersect = f(x3, y3, x1, y1, x2, y2)*f(x4, y4, x1, y1, x2, y2) < 0 && g(x1, y1, x3, y3, x4, y4)*g(x2, y2, x3, y3, x4, y4) < 0;
			pit[pcount] = pit[pcount]||intersect;
			//printf("%d, %d, ", i, j);
			//printf(intersect ? "true\n" : "false\n");
		}
	}

}

// sort by x
void sort(Point pts[], int count)
{
    for(int i = 0; i < count - 1; i++)
    {
        Point currentMin =pts[i];
        int currentMinIndex = i;

        for(int j = i + 1; j < count; j++)
        {
            if(currentMin.x() > pts[j].x())
            {
                currentMin = pts[j];
                currentMinIndex = j;
            }
        }
           
            if(currentMinIndex != i){
                pts[currentMinIndex] = pts[i];
                pts[i] = currentMin;

            }
        }
}

void scanConvert() {
    const double w = glutGet( GLUT_WINDOW_WIDTH );
    const double h = glutGet( GLUT_WINDOW_HEIGHT );
	int init = 0;
	for(int j = 0; j <= pcount; j++){
		// Ignore the non-simple polygons
		int y = (int)h;
		if(pit[j])
			y = -1;
		for(; y >= 0; y--) {
			//printf("%d-----\n", y);
			Point isPt[1024];
			int isCount = 0;
			for (int i = init + 1; i < init + polygons[j]; i++) {
					int	yi,xi,dy,dx,sx,sy;
					int	decision,incE,incNE;
					dx = point[i-1].x() - point[i].x();
					dy = (h-point[i-1].y()) - (h-point[i].y());
 
					sx = SIGN(dx);
					sy = SIGN(dy);
 
					dx = ABS(dx);
					dy = ABS(dy);
 
					//if(dy = 0 && h-point[i].y()==y) {
					//	isPt[isCount++].xy(point[i-1].x(), y);
					//	isPt[isCount++].xy(point[i].x(), y);
					//} else 
					if(dy > dx)
					{	
						incE = 2 * dx;
						incNE = 2 * dx - 2 * dy;
						decision = 2 * dy - dx;
 
						xi = point[i].x();
						yi = h-point[i].y();
						do{
							if(yi==y) {
								bool inArr = false;
								for(int k = 0; k < isCount; k++){
									inArr = inArr || (isPt[k].x() == xi);
								}
								if(!inArr){
									isPt[isCount++].xy(xi, y);
									//printf("hello2\n");
								}
								break;
							}

							if(decision <= 0)
								decision += incE;
							else{
								decision += incNE;
								xi += sx;	
							}
							yi += sy;
						}while(yi != h-point[i-1].y());
					}else{
						incE = 2 * dy;
						incNE = 2 * dy - 2 * dx;
						decision = 2 * dy - dx;
 
						xi = point[i].x();
						yi = h-point[i].y();
						do{
							if(yi==y) {
								bool inArr = false;
								for(int k = 0; k < isCount; k++){
									inArr = inArr || (isPt[k].x() == xi);
								}
								if(!inArr){
									isPt[isCount++].xy(xi, y);
									//printf("hello3\n");
								}
								break;
							}

							if(decision <= 0)
								decision += incE;
							else{
								decision += incNE;
								yi += sy;
							}
							xi += sx;
						}while(xi != point[i-1].x());
					}
			}
			// sort points by x
			sort(isPt, isCount);
			for(int k = 0; k <isCount; k+=2){
				if(k == isCount - 1) {
					// draw point
					glPointSize(3.0f);
					glBegin(GL_POINTS);
				    glColor3f(1,1,0);
				    glVertex2i(isPt[k].x(), y);
					glEnd();
					glFlush();
				} else {
					// fill span
					for(int xn = isPt[k].x(); xn <=isPt[k+1].x(); xn++){

						glPointSize(3.0f);
						glBegin(GL_POINTS);
						glColor3f(1,1,0);
						glVertex2i(xn, y);
						glEnd();
						glFlush();
					}
				}
			}
			//for(int k = 0; k <isCount; k++){
			//	isPt[k].xy(-1,-1);
			//}
		}
		init += polygons[j];
	}
}

void mouse(int button, int state, int x, int y)
{       
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {       
        point[count].xy(x, y);
		//printf("%d, %d, %d\n", point[count].x(), point[count].y(), count);
		polygons[pcount] ++;
        count++;    
		glutPostRedisplay();
	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// draw line enclosed the polygon
		if(count > 2) {
		    point[count] = point[count-polygons[pcount]];
			polygons[pcount]++;
			count++;
		}
		//printf("rightclick\n");
		checkSimple();
		glutPostRedisplay();
		pcount++;
	}
    //glutPostRedisplay();
}

void keyboard(unsigned char Key, int x, int y)
{
	switch(Key)
	{
		case 'd':
			for(int i = 0; i < 1024; i++) {
				printPol[i] = pit[i];
			}
			glutPostRedisplay();
			break;
		case 's':
			scanConvert();
			break;
		default:
			break;
	};
}

int main( int argc, char *argv[] )
{
    glutInit(&argc, argv);                       
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(1200, 800);               
    glutCreateWindow("Assigment1");             

    glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
    glutDisplayFunc(Display);

    glutMainLoop();
    return 0;
}