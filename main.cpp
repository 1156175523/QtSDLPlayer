#include "QtSDLPlay.h"
#include <QtWidgets/QApplication>
#include <QWidget>
#include "MySDL.h"

char videoWidgetId[128] = { 0 };

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QtSDLPlay w;
	w.show();

	w.videoWidgetSetPos();
	w.showSDLVideoWidget();

	a.exec();
	return 0;
}

int main1(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtSDLPlay w;

    w.show();
	a.exec();

	return 0;
}
