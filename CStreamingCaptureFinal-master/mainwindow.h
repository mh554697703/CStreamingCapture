#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QStatusBar>
#include <QButtonGroup>
#include <QFile>
#include <QMessageBox>
#include <QFile>
#include <stdio.h>
#include <QDebug>
#include "linechart.h"
#include "global_defines.h"
#include <QtWidgets/qlayout.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void ButtonClassify();
//Group里面RadioButton分组
public slots:
        void onRadioChannels();
        void onRadioTrigger();
        void update_Hex();

private slots:

    void on_lineEdit_toFPGA_0_textChanged(const QString &arg0);

    void on_lineEdit_toFPGA_0x_textChanged(const QString &arg0x);

    void on_lineEdit_toFPGA_1_textChanged(const QString &arg1);

    void on_lineEdit_toFPGA_1x_textChanged(const QString &arg1x);

    void on_lineEdit_toFPGA_2_textChanged(const QString &arg2);

    void on_lineEdit_toFPGA_2x_textChanged(const QString &arg2x);

    void on_lineEdit_toFPGA_3_textChanged(const QString &arg3);

    void on_lineEdit_toFPGA_3x_textChanged(const QString &arg3x);

    void on_lineEdit_toFPGA_4_textChanged(const QString &arg4);

    void on_lineEdit_toFPGA_4x_textChanged(const QString &arg4x);

    void on_lineEdit_toFPGA_5_textChanged(const QString &arg5);

    void on_lineEdit_toFPGA_5x_textChanged(const QString &arg5x);

    void on_lineEdit_toFPGA_6_textChanged(const QString &arg6);

    void on_lineEdit_toFPGA_6x_textChanged(const QString &arg6x);

    void on_lineEdit_toFPGA_7_textChanged(const QString &arg7);

    void on_lineEdit_toFPGA_7x_textChanged(const QString &arg7x);

    void on_pushButton_input_clicked();

    void on_pushButton_output_clicked();

    void on_actionSearch_triggered();

    void on_radioButton_default_clicked();

    void on_radioButton_customize_clicked();

    void on_pushButton_CaptureStart_clicked();

    void on_pushButton_Magnify_clicked();

    void Clear_Dispaly();           // 清除数据绘图显示

    bool Config_ADQ214();           // 配置采集卡
    bool CaptureData2Buffer();      // 采集数据到缓存
    void WriteData2disk();          // 写入采集数据到文件
    void WriteSpecData2disk();      // 写入采集数据到文件
    void Display_Data();            // 显示数据曲线

    void ConvertPSDUnionToArray(PSD_DATA *psd_res);
    void LOSVelocityCal(const int heightNum, const int totalSpecPoints,
                        const int objSpecPoints, const double lambda,
                        const double *freqAxis, const double *specData);

    void on_lineEdit_BufferNum_textChanged(const QString &arg1);

    void on_lineEdit_BufferSize_textChanged(const QString &arg1);

    void on_lineEdit_MirrorLength_textChanged(const QString &arg1);

    void on_checkBox_Overlap_clicked(bool checked);

private:

    Ui::MainWindow *ui;

    void *adq_cu;
    int num_of_devices;
    int num_of_failed;
    int num_of_ADQ14;
    unsigned int nofchannels;
    unsigned int ch;
    signed short* data_stream_target = nullptr;
    signed short* data_channel_target[4] ={nullptr, nullptr, nullptr, nullptr};
    unsigned int samples_per_waveform;
    void connectADQDevice(void);

    QButtonGroup *ButtonChannel;
    QButtonGroup *ButtonTrigger;

    QStatusBar *bar;
    QLabel *ADQ_state;
    void Create_statusbar(void);          //创建状态栏

    quint16 write_data0;
    quint16 write_data1;
    quint16 write_data2;
    quint16 write_data3;
    quint16 write_data4;
    quint16 write_data5;
    quint16 write_data6;
    quint16 write_data7;

    quint16 read_addry0;
    quint16 read_datay0;
    quint16 read_addry1;
    quint16 read_datay1;
    quint16 read_addry2;
    quint16 read_datay2;
    quint16 read_addry3;
    quint16 read_datay3;

    bool isADQ14Connected;

    setupADQ setupadq;

    linechart lineChart;
    QVBoxLayout *drawLayoutCHA,*drawLayoutCHB;
    QVector<float> rowCHA, rowCHB;  //新建动态数组
    QWidget *CHA, *CHB;  //用于管理图像的放大与取消
    PSD_DATA *psd_res;
    double *psd_array;
    double *losVelocity;
};

#endif // MAINWINDOW_H
