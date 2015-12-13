// Polygon triangulation
// 
// Klasse für eigene Events im View-Modus
//
// (c) Georg Umlauf, 2014

#ifndef _ViewEventHandler_H
#define _ViewEventHandler_H

#include <QtWidgets/QMainWindow>
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>

class ViewEventHandler 
{
private:
	QWidget *mainWindow;
	QKeyEvent event;

public:
	ViewEventHandler (QWidget *parent): event(QEvent::KeyPress, Qt::Key_W,0,0) { mainWindow=parent; }

	void triggerWait () {
		QApplication::sendEvent(mainWindow, &event);
	}
};

#endif