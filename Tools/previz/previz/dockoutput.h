#ifndef DOCKOUTPUT_H
#define DOCKOUTPUT_H

#include <QDockWidget>
#include "ui_dockoutput.h"
#include <QString>

class DockOutput : public QDockWidget
{
	Q_OBJECT

public:
	DockOutput(QWidget *parent = 0);
	DockOutput(QString title, QWidget *parent = 0);
	~DockOutput();

private:
	Ui::DockOutput ui;
};

#endif // DOCKOUTPUT_H
