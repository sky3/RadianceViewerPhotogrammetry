#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
	Q_OBJECT

public:
	ClickableLabel(QWidget *parent,  unsigned int camera_id);
	~ClickableLabel();

	 unsigned int getCamId(void);

signals:
	void click(ClickableLabel*);

protected:

	virtual void mouseReleaseEvent(QMouseEvent *ev);

private:

	unsigned int m_camera_id;
	
};

#endif // CLICKABLELABEL_H
