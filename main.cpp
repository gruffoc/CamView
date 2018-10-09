#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QChartView>

using namespace QtCharts;

MainWindow *ptr_mainwindow;


int  measure_number;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    ptr_mainwindow = new MainWindow;

    ptr_mainwindow->show();
    return a.exec();
}
