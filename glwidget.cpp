// Convexe Hülle
// (c) Georg Umlauf, 2015

#include "glwidget.h"
#include <QtGui>
#include <GL/glu.h>
#include <algorithm>
#include "mainwindow.h"


QList<QPointF> pointList;
QList<QPointF> convexHull;
const QPointF ORIGIN = QPointF(0.0, 0.0);


GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{	
}

GLWidget::~GLWidget()
{
}

void GLWidget::paintGL()
{
    // clear
    glClear(GL_COLOR_BUFFER_BIT);

    // Koordinatensystem
    glColor3f(0.5,0.5,0.5);
    glBegin(GL_LINES);
    glVertex2f(-1.0, 0.0);
    glVertex2f( 1.0, 0.0);
    glVertex2f( 0.0,-1.0);
    glVertex2f( 0.0, 1.0);
    glEnd();

	// Konvexe Hülle zeichnen
	// TODO: draw convex hull using glBegin(GL_LINE_STRIP); ... glEnd();
	
	// set size and color
	glPointSize(3);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);

	// draw points
	glBegin(GL_POINTS);
	for each(QPointF i in pointList)
	{
		glVertex2d(i.x(), i.y());
	}
	glEnd();

	// set line color
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	// draw hull
	glBegin(GL_LINE_LOOP);
	for each (QPointF i in convexHull)
	{
		glVertex2d(i.x(), i.y());
	}
	glEnd();
}


void GLWidget::initializeGL()
{
    resizeGL(width(),height());
}

void GLWidget::resizeGL(int width, int height)
{
    aspectx=1.0;
    aspecty=1.0;
    if (width>height) aspectx = float(width) /height;
    else              aspecty = float(height)/ width;
    glViewport    (0,0,width,height);
    glMatrixMode  (GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D    (-aspectx,aspectx,-aspecty,aspecty);
    glMatrixMode  (GL_MODELVIEW);
    glLoadIdentity();
}

QPointF GLWidget::transformPosition(QPoint p)
{
    return QPointF( (2.0*p.x()/ width() - 1.0)*aspectx,
		           -(2.0*p.y()/height() - 1.0)*aspecty);
}

void GLWidget::keyPressEvent(QKeyEvent * event)
{
	switch (event->key()) {
	default:
		break;
	}
	update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	QPointF posF = transformPosition(event->pos());
	if (event->buttons() & Qt::LeftButton ) {
		// TODO: add point to point list and recompute convex hull
		pointList.append(posF);
	}
    update(); 
}

// called repeatedly by qsort to compare two elements
bool pointComparator(const QPointF &point1, const QPointF &point2)
{
	return point1.x() < point2.x();
}

// calulates the rectangles between the points (scalarprod.)
// returns < 0 if there is a rightturn
// returns > 0 if there is no rightturn
// returns = 0 if the points are collinear
double checkForRightTurn(QList<QPointF> list)
{
	QPointF p1 = list[list.length() - 3];
	QPointF p2 = list[list.length() - 2];
	QPointF p3 = list[list.length() - 1];
	return (p2.x() - p1.x())*(p3.y() - p1.y()) - (p2.y() - p1.y())*(p3.x() - p1.x());
}

// returns the point with the lowest Y-value
QPointF getMinY(QList<QPointF> list)
{
	QPointF tmp = list[0];
	for each(QPointF i in list)
	{
		if (i.y() < tmp.y())
		{
			tmp = i;
		}
	}
	return tmp;
}

// calculate the angle
double calculateAngle(QPointF p1, QPointF p2, QPointF i)
{
	// generate two vectors from the points
	QPointF v1 = QPointF(p2.x() - p1.x(), p2.y() - p1.y());
	QPointF v2 = QPointF(i.x() - p2.x(), i.y() - p2.y());

	// calculate and return the angle between the vectors
	return acos((v1.x() * v2.x() + v1.y() * v2.y())
		/ (sqrt(v1.x() * v1.x() + v1.y() * v1.y())
		*	sqrt(v2.x() * v2.x() + v2.y() * v2.y())));
}

// returns the point with the smallest angle
QPointF getNextPoint(QPointF p1, QPointF p2, QList<QPointF> list)
{
	QPointF tmp;
	double tmpAngle = 180; // angle should never be bigger than 180°
	for each(QPointF i in list)
	{
		// dont calculate if its the same point(that would be pointless hahaha)
		if (i == p1 || i == p2)
		{
			continue;
		}
		else
		{
			double angle = calculateAngle(p1, p2, i);
			// if new angle is smaller save it
			if (angle < tmpAngle)
			{
				tmp = i;
				tmpAngle = angle;
			}
		}
	}
	return tmp;
}



// jarvis scan
void GLWidget::radioButton1Clicked()
{
	// TODO: toggle to Jarvis' march

	// clear current hull
	convexHull.clear();

	// if there are less than 3 elements -> return all of them as hull
	if (pointList.length() < 3)
	{
		convexHull.append(pointList);
		return;
	}

	// get point with min. y-value
	QPointF yMin = getMinY(pointList);
	QList<QPointF> hull;
	hull.append(yMin);
	int i = 1;
	// calc 2. point before loop to avoid conflicts
	hull.append(getNextPoint(ORIGIN, hull[i-1], pointList));
	do
	{

		i++;
		hull.append(getNextPoint(hull[i-2], hull[i-1], pointList));
	} while (hull[i] != yMin);

	convexHull.append(hull);

    update();
}

// graham scan
void GLWidget::radioButton2Clicked()
{
	// TODO: toggle to Graham's scan

	// clear current hull
	convexHull.clear();

	// if there are less than 3 elements -> return all of them as hull
	if (pointList.length() < 3)
	{
		convexHull.append(pointList);
		return;
	}

	// sort points by their x-coords
	QList<QPointF> sortedPointList = pointList;
	qSort(sortedPointList.begin(), sortedPointList.end(), pointComparator);

	// add first two sorted elements to upper-list
	QList<QPointF> upperHullList;
	upperHullList.append(sortedPointList[0]);
	upperHullList.append(sortedPointList[1]);

	// get upper hull
	for (int i = 2; i < sortedPointList.length(); ++i)
	{
		upperHullList.append(sortedPointList[i]);
		while (upperHullList.length() > 2 && 0 <= checkForRightTurn(upperHullList))
		{
			upperHullList.removeAt(upperHullList.length() - 2);
		}
	}

	// add last two elements to lower-list
	QList<QPointF> lowerHullList;
	lowerHullList.append(sortedPointList[sortedPointList.length() - 1]);
	lowerHullList.append(sortedPointList[sortedPointList.length() - 2]);

	// get lower hull
	for (int i = sortedPointList.length() - 3; i >= 0; --i)
	{
		lowerHullList.append(sortedPointList[i]);
		while (lowerHullList.length() > 2 && 0 <= checkForRightTurn(lowerHullList))
		{
			lowerHullList.removeAt(lowerHullList.length() - 2);
		}
	}

	// remove first and last element to clear dublications
	lowerHullList.removeAt(0);
	lowerHullList.removeAt(lowerHullList.length() - 1);

	// merge both lists
	convexHull.append(upperHullList);
	convexHull.append(lowerHullList);

    update();
}

void GLWidget::radioButton3Clicked()
{
	// clear pointList and hull
	pointList.clear();
	convexHull.clear();
	update();
}

