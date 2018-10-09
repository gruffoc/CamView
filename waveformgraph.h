#ifndef WAVEFORMGRAPH_H
#define WAVEFORMGRAPH_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>


class WaveformGraph: public QObject
{
    Q_OBJECT

public:
    explicit WaveformGraph();
    ~WaveformGraph();


public slots:
    void do_something();
};

#endif // WAVEFORMGRAPH_H

