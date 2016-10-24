#include "dockoutput.h"
#include <QDebug>

DockOutput::DockOutput(QWidget *parent): QDockWidget(parent)
{
	ui.setupUi(this);
	ui.scrollArea->setLabel(ui.label);
}

DockOutput::DockOutput(QString title, QWidget *parent): QDockWidget(title,parent)
{
	ui.setupUi(this);
	ui.scrollArea->setLabel(ui.label);
	setWindowTitle(title);
}

DockOutput::~DockOutput()
{

}
