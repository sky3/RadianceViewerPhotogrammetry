#include "outputwidget.h"
#include <QScrollBar>
#include <QLayout>
#include <QVBoxLayout>
#include <iostream>
#include <QMessageBox>

OutputWidget *OutputWidget::ow=nullptr;

OutputWidget::OutputWidget(QWidget *parent): QScrollArea(parent)
{
	QObject::connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(moveScrollBarToBottom(int, int)));
}

OutputWidget::~OutputWidget()
{

}

void OutputWidget::setLabel(QLabel *lab)
{
	if(!ow)
	{
		ow=this;
		output=lab;
		std::cout.rdbuf(this);
	}
}

void OutputWidget::moveScrollBarToBottom(int min, int max)
{
    Q_UNUSED(min);
    verticalScrollBar()->setValue(max);
}

void OutputWidget::logHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
	if(ow)
	{
		bool isFatal=false;
		QByteArray localMsg = message.toLocal8Bit();
		QString nMessage=message;
		nMessage.replace('\n',"<br>");
		QString msg;
		switch (type) 
		{
			case QtDebugMsg:
				//if(QString(context.file)=="entete.cpp")
				msg="<span style=\"color:#B8D7A3\">Debug: "+nMessage+"</span>";
				fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				break;
			case QtWarningMsg:
				//if(QString(context.file)=="entete.cpp")
				msg="<span style=\"color:#CDBA3B\">Warning: "+nMessage+"</span>";
				fprintf(stderr, "Warning: %s (%s:%u, %s)</span>\n", localMsg.constData(), context.file, context.line, context.function);
				break;
			case QtCriticalMsg:
				msg="<span style=\"color:#C75670\">Critical: "+nMessage+"</span>";
				fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				break;
			case QtFatalMsg:
				msg="<span style=\"color:#C75670\">Fatal: "+nMessage+"</span>";
				fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
				QMessageBox::critical(ow,"",msg);
				isFatal=true;
				break;
		}

		if(msg.size()>0)
			ow->output->setText(ow->output->text()+msg+"<br>");
	}

	//std::cout<<ow->output->text().toStdString();
} 
