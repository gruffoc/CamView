#include "waveformgraph.h"
#include "unistd.h"
#include <iostream>

#include <QString>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include <QtCharts/QChartView>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QtCharts;

extern MainWindow *ptr_mainwindow;

extern int measure_number;

WaveformGraph::WaveformGraph()
{

}

WaveformGraph::~WaveformGraph()
{
    delete this;
}


void WaveformGraph::do_something()
{
    measure_number=10;
    for (int k = 0; k < measure_number; ++k) {
        std::cout << k << std::endl;
        std::cout << "coiao" << std::endl;
    }

    emit change();
}



