#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QScrollArea>
#include <QDockWidget>
#include <QLabel>
#include <iostream>

class OutputWidget: public QScrollArea, std::basic_streambuf< char, std::char_traits<char> >
{
	Q_OBJECT

public:
	OutputWidget(QWidget *parent = nullptr);
	~OutputWidget();
	static void logHandler(QtMsgType type, const QMessageLogContext & context, const QString& message);
	int_type OutputWidget::overflow(int_type c)  
	{  
		if(ow)
		{
			QString messaged(std::char_traits<char>::to_char_type(c));
			messaged.replace('\n',"<br>");
			output->setText(ow->output->text()+messaged);
		}
		else
			printf("%s",std::char_traits<char>::to_char_type(c));

		return c;
	}  

	void setLabel(QLabel *lab);

public slots:
	void moveScrollBarToBottom(int min, int max);

private:
	QLabel *output;
	static OutputWidget *ow;

};

#endif // OUTPUTWIDGET_H
