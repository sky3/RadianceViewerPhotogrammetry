#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget *parent, unsigned int camera_id): QLabel(parent),m_camera_id(camera_id)
{

}

ClickableLabel::~ClickableLabel()
{

}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	emit click(this);
}


unsigned int ClickableLabel::getCamId(void)
{
	return m_camera_id;
}