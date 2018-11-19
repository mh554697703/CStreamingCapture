#include "linechart.h"

linechart::linechart(QObject *parent) : QObject(parent)
{
}

void linechart::line(QVector<float> vectorA, int clectNumber)
{

    QLineSeries *series = new QLineSeries();     //新建折线图类

    for(int i=0;i<clectNumber;)               //加入填充点
    {
        *series << QPointF(i+1, vectorA[i]);
        i++;
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    //series->setUseOpenGL(true);//加速？
    chart->createDefaultAxes();    //先创建默认坐标轴

    QValueAxis *axisX= new QValueAxis;

    //  axisX->setTickCount(series->count());// setTickCount导致卡顿
    axisX->setMinorTickCount(10);
    axisX->setLabelFormat("%.i");
    axisX->applyNiceNumbers();
    axisX->setTitleText("点数");
    axisX->setTickCount(10);
    chart->setAxisX(axisX, series);
    chart->setTitle("Line Chart");

    QValueAxis *axisY= new QValueAxis;
    axisY->setMinorTickCount(10);
    axisY->setLabelFormat("%.1f");
    chart->setAxisY(axisY, series);
    axisY->applyNiceNumbers();
    axisY->setTitleText(tr("信号"));
    chart->setAnimationOptions(QChart::NoAnimation);  //动画选项

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand(QChartView::RectangleRubberBand);  //矩形框 放大、缩小

}





