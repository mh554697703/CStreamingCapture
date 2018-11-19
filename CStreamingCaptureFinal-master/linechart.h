#ifndef LINECHART_H
#define LINECHART_H

//#include <QtWidgets/QApplication>
//#include <QtWidgets/QMainWindow>
//#include <QtCharts/QChartView>
//#include <QtCharts/QBarSeries>
//#include <QtCharts/QBarSet>
//#include <QtCharts/QLegend>
//#include <QtCharts/QBarCategoryAxis>
#include <QtCharts>
#include <QHorizontalBarSeries>
#include <QValueAxis>
#include <QDebug>
//#include <QtCharts/QLineSeries>

#include <QObject>
QT_CHARTS_USE_NAMESPACE

class linechart : public QObject
{
    Q_OBJECT
public:
    explicit linechart(QObject *parent = nullptr);
    void line(QVector<float> vectorA, int clectNumber);
    QChartView *chartView;

signals:

public slots:
};

#endif // LINECHART_H
