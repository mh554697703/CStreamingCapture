#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ADQAPI.h"
#include <QDebug>
#include <QFont>
#include <qbuttongroup.h>
#include <memory.h>
#include <stdio.h>
#include <QDataStream>
#include <QtMath>

//#define SUCCESS(f) = {if(!(f)){QMessageBox::critical(this, QString::fromStdString("提示"), QString::fromStdString("Error"));}}
static bool success = true;
const unsigned int adq_num = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDir dir;
    SettingFile = new FPGA_Setting(dir.currentPath());
    MySetting = SettingFile->ReadSettingFile(dir.currentPath());               //读取FPGA设置文件
    int *factor= SettingFile->ReadFactorFile(dir.currentPath());               //读取factor_fil
    for(int i=0;i<512;i++)                                                     //取出各系数存入数组
       MyFactor[i]=factor[i];

    setupadq.num_sample_skip = 128;        //设为128，省去sample_decimation

    ButtonClassify();                      //Group里RadioButton分类
    on_radioButton_customize_clicked();
    Create_statusbar();

    num_of_devices = 0;
    num_of_failed = 0;
    num_of_ADQ14 = 0;
    adq_cu = CreateADQControlUnit();
    qDebug() << "adq_cu = " << adq_cu;
    connectADQDevice();                                      //连接采集卡

    setupadq.apirev = ADQAPI_GetRevision();                 //获取API版本
    qDebug() << IS_VALID_DLL_REVISION(setupadq.apirev);
    qDebug() << "ADQAPI Example";
    qDebug() << "API Revision:" << setupadq.apirev;

    drawLayoutCHA=ui->verticalLayout_CHA;
    drawLayoutCHB=ui->verticalLayout_CHB;

    onRadioChannels();
    onRadioTrigger();
    update_Hex();
    setupadq.num_buffers = 256;
    setupadq.size_buffers = 1024;
    ui->lineEdit_BufferNum->setText(QString::number(setupadq.num_buffers));
    ui->lineEdit_BufferSize->setText(QString::number(setupadq.size_buffers/512));

    on_checkBox_Overlap_clicked(ui->checkBox_Overlap->isChecked());
    psd_res = nullptr;
}

void MainWindow::Create_statusbar()                   //创建状态栏
{
    bar = ui->statusBar;
    QFont font("Microsoft YaHei UI",10);
    bar->setFont(font);

    ADQ_state = new QLabel;
    ADQ_state->setMinimumSize(115,22);
    ADQ_state->setAlignment(Qt::AlignLeft);
    bar->addWidget(ADQ_state);
}

void MainWindow::on_actionSearch_triggered()          //search
{
    connectADQDevice();
}

void MainWindow::connectADQDevice()
{
    num_of_devices = ADQControlUnit_FindDevices(adq_cu);			//找到所有与电脑连接的ADQ，并创建一个指针列表，返回找到设备的总数
    num_of_failed = ADQControlUnit_GetFailedDeviceCount(adq_cu);
    num_of_ADQ14 = ADQControlUnit_NofADQ14(adq_cu);				//返回找到ADQ214设备的数量
    if((num_of_failed > 0)||(num_of_devices == 0))
    {
        ADQ_state->setText(QString::fromLocal8Bit("采集卡未连接"));
        isADQ14Connected = false;
    }
    else if (num_of_ADQ14 != 0)
    {
        ADQ_state->setText(QString::fromLocal8Bit("采集卡已连接"));
        isADQ14Connected = true;
    }
}

MainWindow::~MainWindow()
{
    DeleteADQControlUnit(adq_cu);
    delete SettingFile;
    delete ui;
    if (psd_res != nullptr)
        delete psd_res;
    if (losVelocity != nullptr)
        delete losVelocity;
}

//默认buffers
void MainWindow::on_radioButton_default_clicked()
{
    ui->lineEdit_BufferNum->setEnabled(false);
    ui->lineEdit_BufferSize->setEnabled(false);
}

//自定义buffers
void MainWindow::on_radioButton_customize_clicked()
{
    ui->lineEdit_BufferNum->setEnabled(true);
    ui->lineEdit_BufferSize->setEnabled(true);
    ui->lineEdit_BufferNum->setText(QString::number(setupadq.num_buffers));
    ui->lineEdit_BufferSize->setText(QString::number(setupadq.size_buffers/512));
}

//进制自动转换
void MainWindow::on_lineEdit_toFPGA_0_textChanged(const QString &arg0)             //30
{
    QString lineEdit_toFPGA0x = QString::number(arg0.toInt(), 16).toUpper();  //输入的字符串先转成16进制整形，再转回字符串，再将小写改成大写，填入对应的lineedit
    ui->lineEdit_toFPGA_0x->setText(lineEdit_toFPGA0x);
}

void MainWindow::on_lineEdit_toFPGA_0x_textChanged(const QString &arg0x)
{
    int lineEdit_toFPGA0 = arg0x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_0->setText(QString::number(lineEdit_toFPGA0));
}

void MainWindow::on_lineEdit_toFPGA_1_textChanged(const QString &arg1)            //31
{
    QString lineEdit_toFPGA1x = QString::number(arg1.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_1x->setText(lineEdit_toFPGA1x);
    ui->lineEdit_LevelDisp->setText(QString::number(arg1.toDouble()*1100/8192));
}

void MainWindow::on_lineEdit_toFPGA_1x_textChanged(const QString &arg1x)
{
    int lineEdit_toFPGA1 = arg1x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_1->setText(QString::number(lineEdit_toFPGA1));
}

void MainWindow::on_lineEdit_toFPGA_2_textChanged(const QString &arg2)            //32
{
    QString lineEdit_toFPGA2x = QString::number(arg2.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_2x->setText(lineEdit_toFPGA2x);
}

void MainWindow::on_lineEdit_toFPGA_2x_textChanged(const QString &arg2x)
{
    int lineEdit_toFPGA2 = arg2x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_2->setText(QString::number(lineEdit_toFPGA2));
}

void MainWindow::on_lineEdit_toFPGA_3_textChanged(const QString &arg3)            //33
{
    QString lineEdit_toFPGA3x = QString::number(arg3.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_3x->setText(lineEdit_toFPGA3x);
    int nPoints = arg3.toInt();
    int nBins = ui->lineEdit_toFPGA_4->text().toInt();
    ui->lineEdit_toFPGA_7->setText(QString::number(nPoints*nBins));
    //    ui->lineEdit_SampTotNum->setText(QString::number(nPoints*(nBins-1)*4));
    on_checkBox_Overlap_clicked(ui->checkBox_Overlap->isChecked());
}

void MainWindow::on_lineEdit_toFPGA_3x_textChanged(const QString &arg3x)
{
    int lineEdit_toFPGA3 = arg3x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_3->setText(QString::number(lineEdit_toFPGA3));
}

void MainWindow::on_lineEdit_toFPGA_4_textChanged(const QString &arg4)             //34
{
    QString lineEdit_toFPGA4x = QString::number(arg4.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_4x->setText(lineEdit_toFPGA4x);
    int nBins = arg4.toInt();
    int nPoints = ui->lineEdit_toFPGA_3->text().toInt();
    ui->lineEdit_toFPGA_7->setText(QString::number(nPoints*nBins));
    ui->lineEdit_SampTotNum->setText(QString::number(512*(nBins-1)*4));

    on_checkBox_Overlap_clicked(ui->checkBox_Overlap->isChecked());

}

void MainWindow::on_lineEdit_toFPGA_4x_textChanged(const QString &arg4x)
{
    int lineEdit_toFPGA4 = arg4x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_4->setText(QString::number(lineEdit_toFPGA4));
}

void MainWindow::on_lineEdit_toFPGA_5_textChanged(const QString &arg5)             //35
{
    QString lineEdit_toFPGA5x = QString::number(arg5.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_5x->setText(lineEdit_toFPGA5x);
}

void MainWindow::on_lineEdit_toFPGA_5x_textChanged(const QString &arg5x)
{
    int lineEdit_toFPGA5 = arg5x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_5->setText(QString::number(lineEdit_toFPGA5));
}

void MainWindow::on_lineEdit_toFPGA_6_textChanged(const QString &arg6)             //36
{
    QString lineEdit_toFPGA6x = QString::number(arg6.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_6x->setText(lineEdit_toFPGA6x);
}

void MainWindow::on_lineEdit_toFPGA_6x_textChanged(const QString &arg6x)
{
    int lineEdit_toFPGA6 = arg6x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_6->setText(QString::number(lineEdit_toFPGA6));
}

void MainWindow::on_lineEdit_toFPGA_7_textChanged(const QString &arg7)             //37
{
    QString lineEdit_toFPGA7x = QString::number(arg7.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_7x->setText(lineEdit_toFPGA7x);

    int nPointsPerRB = ui->lineEdit_toFPGA_3->text().toInt();
    int nRangeBin = ui->lineEdit_toFPGA_4->text().toInt();
    int nTotalPoints =arg7.toInt();
    int MirrorLength = ui->lineEdit_MirrorLength->text().toInt();

    int UR_EndPosition = (nRangeBin-1)*nPointsPerRB + MirrorLength;     // 设置FPGA数据采样长度

    if (UR_EndPosition != nTotalPoints)
        ui->lineEdit_toFPGA_7->setStyleSheet("color: red;");
    else
        ui->lineEdit_toFPGA_7->setStyleSheet("color: black;");

}

void MainWindow::on_lineEdit_toFPGA_7x_textChanged(const QString &arg7x)
{
    int lineEdit_toFPGA7 = arg7x.toInt(nullptr,16);
    ui->lineEdit_toFPGA_7->setText(QString::number(lineEdit_toFPGA7));
}


void MainWindow::on_pushButton_input_clicked()
{
    if(num_of_ADQ14 != 0)            //
    {
        read_datay0 = ADQ_ReadAlgoRegister(adq_cu,1,0x20);    //return the read data
        qDebug() << "read_datay0 = " << read_datay0;
        ui->lineEdit_fromFPGA_0->setText(QString("%1").arg(read_datay0,0,10));
        ui->lineEdit_fromFPGA_0x->setText(QString("0x%1").arg(read_datay0,0,16).toUpper());

        read_datay1 = ADQ_ReadAlgoRegister(adq_cu,1,0x21);
        qDebug() << "read_datay1 = " << read_datay1;
        ui->lineEdit_fromFPGA_1->setText(QString("%1").arg(read_datay1,0,10));
        ui->lineEdit_fromFPGA_1x->setText(QString("0x%1").arg(read_datay1,0,16).toUpper());

        read_datay2 = ADQ_ReadAlgoRegister(adq_cu,1,0x22);
        qDebug() << "read_datay2 = " << read_datay2;
        ui->lineEdit_fromFPGA_2->setText(QString("%1").arg(read_datay2,0,10));
        ui->lineEdit_fromFPGA_2x->setText(QString("0x%1").arg(read_datay2,0,16).toUpper());

        read_datay3 = ADQ_ReadAlgoRegister(adq_cu,1,0x23);
        qDebug() << "read_datay3 = " << read_datay3;
        ui->lineEdit_fromFPGA_3->setText(QString("%1").arg(read_datay3,0,10));
        ui->lineEdit_fromFPGA_3x->setText(QString("0x%1").arg(read_datay3,0,16).toUpper());

    }
    else
        qDebug() << "ADQ14 device unconnected";
}

void MainWindow::on_pushButton_output_clicked()
{
    if(num_of_ADQ14 != 0)
    {
        write_data0 = MySetting.Command;
        int x0 = ADQ_WriteUserRegister(adq_cu,1,2,0x30,0,write_data0,nullptr);      //adq_cu：返回控制单元的指针
        qDebug() << "x0 = " << x0;

        write_data1 = MySetting.TrigLevel;
        int x1 = ADQ_WriteUserRegister(adq_cu,1,2,0x31,0,write_data1,nullptr);      //adq_cu：返回控制单元的指针
        qDebug() << "x1 = " << x1;

        write_data2 = MySetting.Nof_PulsesAcc;
        int x2 = ADQ_WriteUserRegister(adq_cu,1,2,0x32,0,write_data1,nullptr);      //adq_cu：返回控制单元的指针
        qDebug() << "x2 = " << x2;

        write_data3 = MySetting.Nof_PointsPerBin;
        int x3 = ADQ_WriteUserRegister(adq_cu,1,2,0x33,0,write_data1,nullptr);      //adq_cu：返回控制单元的指针
        qDebug() << "x3 = " << x3;

        write_data4 = MySetting.Nof_RangeBin;
        int x4 = ADQ_WriteUserRegister(adq_cu,1,2,0x34,0,write_data1,nullptr);      //adq_cu：返回控制单元的指针
        qDebug() << "x4 = " << x4;

        write_data5 = MySetting.Overlap;
        int x5 = ADQ_WriteUserRegister(adq_cu,1,2,0x35,0,write_data1,nullptr);     //adq_cu：返回控制单元的指针
        qDebug() << "x5 = " << x5;

        write_data6 = MySetting.MirrorStart;
        int x6 = ADQ_WriteUserRegister(adq_cu,1,2,0x36,0,write_data1,nullptr);     //adq_cu：返回控制单元的指针
        qDebug() << "x6 = " << x6;

        write_data7 = MySetting.PointsOfProcess;
        int x7 = ADQ_WriteUserRegister(adq_cu,1,2,0x37,0,write_data1,nullptr);     //adq_cu：返回控制单元的指针
        qDebug() << "x7 = " << x7;

    }
    else
        qDebug() << "ADQ14 device unconnected";
}

//Group内RadioButton分组互斥分配ID
void MainWindow::ButtonClassify()
{
    ButtonChannel = new QButtonGroup(this);
    ButtonChannel->addButton(ui->radioButton_channelA, 0);
    ButtonChannel->addButton(ui->radioButton_channelB, 1);
    ButtonChannel->addButton(ui->radioButton_channelBo, 2);

    ButtonTrigger = new QButtonGroup(this);
    ButtonTrigger->addButton(ui->radioButton_noTrig, 0);
    ButtonTrigger->addButton(ui->radioButton_softwareTrig, 1);
    ButtonTrigger->addButton(ui->radioButton_externalTrig, 2);

    connect(ui->radioButton_channelA, SIGNAL(clicked()), this, SLOT(onRadioChannels()));
    connect(ui->radioButton_channelB, SIGNAL(clicked()), this, SLOT(onRadioChannels()));
    connect(ui->radioButton_channelBo, SIGNAL(clicked()), this, SLOT(onRadioChannels()));
    connect(ui->radioButton_noTrig, SIGNAL(clicked()), this, SLOT(onRadioTrigger()));
    connect(ui->radioButton_softwareTrig, SIGNAL(clicked()), this, SLOT(onRadioTrigger()));
    connect(ui->radioButton_externalTrig, SIGNAL(clicked()), this, SLOT(onRadioTrigger()));
}

void MainWindow::onRadioChannels()
{
    setupadq.stream_ch = ButtonChannel->checkedId();
}

void MainWindow::onRadioTrigger()
{
    setupadq.trig_mode = ButtonTrigger->checkedId();
}

void MainWindow::on_lineEdit_BufferNum_textChanged(const QString &arg1)
{
    setupadq.num_buffers = arg1.toInt();
}

void MainWindow::on_lineEdit_BufferSize_textChanged(const QString &arg1)
{
    setupadq.size_buffers = arg1.toInt()*512;
}

void MainWindow::update_Hex()
{
    QString str;
    str = ui->lineEdit_toFPGA_0->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_0x->setText(str);

    str = ui->lineEdit_toFPGA_1->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_1x->setText(str);

    str = ui->lineEdit_toFPGA_2->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_2x->setText(str);

    str = ui->lineEdit_toFPGA_3->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_3x->setText(str);

    str = ui->lineEdit_toFPGA_4->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_4x->setText(str);

    str = ui->lineEdit_toFPGA_5->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_5x->setText(str);

    str = ui->lineEdit_toFPGA_6->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_6x->setText(str);

    str = ui->lineEdit_toFPGA_7->text();
    str = QString::number(str.toInt(), 16).toUpper();
    ui->lineEdit_toFPGA_7x->setText(str);

}

void MainWindow::on_pushButton_Magnify_clicked() //放大按钮
{
    if(!drawLayoutCHB->isEmpty())  //图像放大
    {
        CHB = this->drawLayoutCHB->itemAt(0)->widget();
        if(CHB->isWidgetType())
        {
            CHB->setWindowFlags(Qt::Window);
            CHB->showMaximized();
        }
    }
    else
    {
        if(!CHB)
            CHB = nullptr;
        else                 //再次点击取消放大
        {
            CHB->setWindowFlags(Qt::Widget);
            drawLayoutCHB->addWidget(CHB);
        }
    }

    if(!drawLayoutCHA->isEmpty())
    {
        CHA = this->drawLayoutCHA->itemAt(0)->widget();
        if(CHA->isWidgetType())
        {
            CHA->setWindowFlags(Qt::Window);
            CHA->showMaximized();
        }
    }
    else
    {
        if(!CHA)
            CHA = nullptr;
        else
        {
            CHA->setWindowFlags(Qt::Widget);
            drawLayoutCHA->addWidget(CHA);
        }
    }
}

void MainWindow::Clear_Dispaly()                   // 清除数据绘图显示
{
    while(drawLayoutCHA->count())                  // 删除布局中的所有控件
    {
        QWidget *p = this->drawLayoutCHA->itemAt(0)->widget();
        p->setParent (nullptr);
        this->drawLayoutCHA->removeWidget(p);
        delete p;
    }
    while(drawLayoutCHB->count())                  // 删除布局中的所有控件
    {
        QWidget *p = this->drawLayoutCHB->itemAt(0)->widget();
        p->setParent (nullptr);
        this->drawLayoutCHB->removeWidget(p);
        delete p;
    }
    CHA = nullptr;
    CHB = nullptr;
}

//开始采集
void MainWindow::on_pushButton_CaptureStart_clicked()
{
    if(!Config_ADQ214())
        return;

    Clear_Dispaly();

    setupadq.num_samples_collect = ui->lineEdit_SampTotNum->text().toInt();  //设置采样点数
    setupadq.data_stream_target = new qint16[setupadq.num_samples_collect];
    memset(setupadq.data_stream_target, 0, setupadq.num_samples_collect* sizeof(signed short));

    if(!CaptureData2Buffer())
        return;

    WriteData2disk();
    WriteSpecData2disk();

    qDebug() << "Start Converting";
    ConvertPSDUnionToArray(psd_res);
    qDebug() << "Convert success!";
    double *freqAxis = new double[512];
    for (int i = 0; i < 512; i++) {
        freqAxis[i] = 200*(i+1)/512;
    }
    int heightNum = ui->lineEdit_toFPGA_4->text().toInt()-1;
    qDebug() << "Height num = " << heightNum;
    qDebug() << "Start cal losVelcity!!!";
    LOSVelocityCal(heightNum, 512, 20, 1.55, freqAxis, psd_array);
    qDebug() << "Cal finished!!!!";
    for (int i = 0; i < heightNum-2; i++) {
        qDebug() << losVelocity[i];
    }
    Display_Data();

    qDebug() << ("Collect finished!");
    delete setupadq.data_stream_target;
    if(success == 0)
    {
        qDebug() << "Error!";
        DeleteADQControlUnit(adq_cu);
    }
}

bool MainWindow::Config_ADQ214()                   // 配置采集卡
{
    success = false;
    if (!isADQ14Connected)
    {
        // 应添加自动连接的功能
        QMessageBox::critical(this, QString::fromLocal8Bit("采集卡未连接！！"), QString::fromLocal8Bit("采集卡未连接"));
    }
    else
    {
        if(ui->radioButton_customize->isChecked())
        {
            success = ADQ_SetTransferBuffers(adq_cu, adq_num, setupadq.num_buffers, setupadq.size_buffers);
            qDebug() << "num_buffer = " << setupadq.num_buffers;
            qDebug() << "size_buffer = " << setupadq.size_buffers;
            ADQ_SetTestPatternMode(adq_cu,adq_num, 0);
        }
        //设置数据简化方案
        if(ADQ_SetSampleSkip(adq_cu, adq_num, setupadq.num_sample_skip) == 0)
        {
            qDebug() << "Error";
            DeleteADQControlUnit(adq_cu);
        }

        // 设置采集通道
        qDebug() << "stream_ch=" << setupadq.stream_ch;
        switch(setupadq.stream_ch) {
        case 0:
            setupadq.stream_ch = ADQ214_STREAM_ENABLED_A;
            qDebug() << "A";
            break;
        case 1:
            setupadq.stream_ch = ADQ214_STREAM_ENABLED_B;
            qDebug() << "B";
            break;
        case 2:
            setupadq.stream_ch = ADQ214_STREAM_ENABLED_BOTH;
            qDebug() << "Both";
            break;
        }
        nofchannels = ADQ_GetNofChannels(adq_cu, adq_num);
          // 为所有数据通道分配流数据的临时缓冲区
          data_stream_target = (signed short*)malloc(samples_per_waveform*nofchannels*sizeof(signed short));

          // 分配通道缓冲区以分离数据
          for(ch = 0; ch < nofchannels; ch++)
            data_channel_target[ch] = (signed short*)malloc(samples_per_waveform*sizeof(signed short*));
        // 设置触发方式：无触发、软件触发、外触发

        qDebug() << "tri_mode=" << setupadq.trig_mode;
        switch(setupadq.trig_mode)
        {
        case 0:
            setupadq.stream_ch &= 0x7;
            qDebug() << "no_trigger";
            break;
        case 1:
        {
            ADQ_SetTriggerMode(adq_cu, adq_num,setupadq.trig_mode);
            setupadq.stream_ch |= 0x8;
            qDebug() << "soft_trigger";
        }
            break;
        case 2:
        {
            ADQ_SetTriggerMode(adq_cu, adq_num,setupadq.trig_mode);
            setupadq.stream_ch |= 0x8;
            qDebug() << "ext_trigger";
        }
            break;
        }
        ADQ_SetTriggerEdge(adq_cu, adq_num, setupadq.trig_mode, 0);
        ADQ_SetLvlTrigLevel(adq_cu, adq_num, 0);

//        setupadq.clock_source = 0;            //0 = Internal clock
//        success = ADQ_SetClockSource(adq_cu, adq_num, setupadq.clock_source);

//        setupadq.pll_divider = 2;            //在Internal clock=0时，设置，f_clk = 800MHz/divider
//      success = success && ADQ214_SetPllFreqDivider(adq_cu, adq_num, setupadq.pll_divider); //ADQ14没有这函数
    }
    return success;
}

bool MainWindow::CaptureData2Buffer()                   // 采集数据到缓存
{
//    success = ADQ_DisarmTrigger(adq_cu, adq_num);
    success = success && ADQ_SetStreamStatus(adq_cu, adq_num,setupadq.stream_ch); //ADQ14没有这个函数
//    success = success && ADQ_ArmTrigger(adq_cu, adq_num);
    ADQ_SetStreamConfig(adq_cu, adq_num, 1, 0); //USe DRAM as giant FIFO
    ADQ_SetStreamConfig(adq_cu, adq_num, 2, 1); //RAW mode
    ADQ_SetStreamConfig(adq_cu, adq_num, 3,  1*1 + 2*1 + 4*1+8*1); //mask

    success = success && ADQ_StopStreaming(adq_cu, adq_num);
    success = success && ADQ_StartStreaming(adq_cu, adq_num);
    if (setupadq.trig_mode == 1)	    // 如果触发模式为sofware
    {
        ADQ_SWTrig(adq_cu, adq_num);
         qDebug() <<"trig_mode = "<<setupadq.trig_mode;
    }

    unsigned int samples_to_collect;
    samples_to_collect = setupadq.num_samples_collect;

    int nloops = 0;
    while (samples_to_collect > 0)
    {
        nloops ++;
        qDebug() << "Loops:" << nloops;
        if (setupadq.trig_mode == 1)        //If trigger mode is sofware
        {
            ADQ_SWTrig(adq_cu, adq_num);
        }

        //ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,write_data0&0xFF7F);   // bit[7]置0
        //ADQ214_WriteAlgoRegister(adq_cu,1,0x30,0,write_data0|0x0080);   // bit[7]置1
        ADQ_WriteUserRegister(adq_cu,1,2,0x30,0,write_data0&0xFFFE,nullptr);   // bit[0]置0
        ADQ_WriteUserRegister(adq_cu,1,2,0x30,0,write_data0|0x0001,nullptr);   // bit[0]置1

        do
        {
            setupadq.collect_result = ADQ_GetTransferBufferStatus(adq_cu, adq_num, &setupadq.buffers_filled);
            qDebug() << ("Filled: ") << setupadq.buffers_filled;
            qDebug()<<"success = "<<success;
        } while ((setupadq.buffers_filled == 0) && (setupadq.collect_result));

        setupadq.collect_result = ADQ_CollectDataNextPage(adq_cu, adq_num);  //ADQ14没有这个函数
        qDebug() << "setupadq.collect_result = " << setupadq.collect_result;

        int samples_in_buffer = qMin(ADQ_GetSamplesPerPage(adq_cu, adq_num), samples_to_collect); //ADQ14没有这个函数
        qDebug() << "samples_in_buffer = " << samples_in_buffer;

        if (ADQ_GetStreamOverflow(adq_cu, adq_num))
        {
            qDebug() << ("Warning: Streaming Overflow!");
            setupadq.collect_result = 0;
        }

        if (setupadq.collect_result)
        {
            // Buffer all data in RAM before writing to disk, if streaming to disk is need a high performance
            // procedure could be implemented here.
            // Data format is set to 16 bits, so buffer size is Samples*2 bytes
            memcpy((void*)&setupadq.data_stream_target[setupadq.num_samples_collect - samples_to_collect],
                    ADQ_GetPtrStream(adq_cu, adq_num), samples_in_buffer* sizeof(signed short));  //ADQ14没有这个函数 GetPtrStream()
            samples_to_collect -= samples_in_buffer;
            qDebug() << " AA= "<<samples_to_collect;
        }
        else
        {
            qDebug() << ("Collect next data page failed!");
            samples_to_collect = 0;
        }
    }

    success = success && ADQ_DisarmTrigger(adq_cu, adq_num);
           ADQ_StopStreaming(adq_cu, adq_num);
    success = success && ADQ_SetStreamStatus(adq_cu, adq_num,0); //ADQ14没有这个函数
    return success;
}

void MainWindow::WriteData2disk()                   // 将数据直接写入文件
{
    // Write to data to file after streaming to RAM, because ASCII output is too slow for realtime.
    qDebug() << "Writing stream data in RAM to disk" ;

    setupadq.stream_ch &= 0x07;
    QFile fileA("dataA.txt");
    QFile fileB("dataB.txt");

    switch(setupadq.stream_ch)
    {
    case ADQ214_STREAM_ENABLED_BOTH:
    {
        QTextStream out(&fileA);
        QTextStream out2(&fileB);

        unsigned int samples_to_collect = setupadq.num_samples_collect;
        if(fileA.open(QFile::WriteOnly)&&fileB.open(QFile::WriteOnly))
        {
            while (samples_to_collect > 0)
            {
                for (int i=0; (i<4) && (samples_to_collect>0); i++)
                {
                    out << setupadq.data_stream_target[setupadq.num_samples_collect-samples_to_collect] << endl;
                    qDebug()<<"CHA -- "<<setupadq.num_samples_collect-samples_to_collect;
                    samples_to_collect--;
                }

                for (int i=0; (i<4) && (samples_to_collect>0); i++)
                {
                    out2 << setupadq.data_stream_target[setupadq.num_samples_collect-samples_to_collect] << endl;
                    qDebug()<<"CHB -- "<<setupadq.num_samples_collect-samples_to_collect;
                    samples_to_collect--;
                }
            }
        }
        fileA.close();
        fileB.close();
        break;
    }
    case ADQ214_STREAM_ENABLED_A:
    {
        if(fileA.open(QFile::WriteOnly))
        {
            QTextStream out(&fileA);
            for (int i=0; i<setupadq.num_samples_collect; i++)
            {
                out<<setupadq.data_stream_target[i]<<endl;
            }
        }
        fileA.close();
        break;
    }
    case ADQ214_STREAM_ENABLED_B:
    {
        if(fileB.open(QFile::WriteOnly))
        {
            QTextStream out(&fileB);
            for (int i=0; i<setupadq.num_samples_collect; i++)
            {
                out<<setupadq.data_stream_target[i]<<endl;
            }
        }
        fileB.close();
        break;
    }
    default:
        break;
    }
}

void MainWindow::WriteSpecData2disk()                   // 将数据转换成功率谱，写入到文件
{
    // Write to data to file after streaming to RAM, because ASCII output is too slow for realtime.

    if(setupadq.num_samples_collect % 2048 != 0)
        return;
    int nLoops = setupadq.num_samples_collect/2048;


    setupadq.stream_ch &= 0x07;

    if(setupadq.stream_ch == ADQ214_STREAM_ENABLED_BOTH)
    {
        if (psd_res != nullptr)
            delete psd_res;
        int psd_datanum = 512*nLoops;        //功率谱长度
        psd_res = new PSD_DATA[psd_datanum];

        int i = 0, k = 0, l = 0;
        for (l=0;l<nLoops;l++)
            for (k=0,i=0; (k<512); k++,k++)
            {
                if(ui->checkBox_Order->isChecked())
                {
                    psd_res[512*l + BitReverseIndex[k]].pos[3] = setupadq.data_stream_target[2048*l + i];
                    psd_res[512*l + BitReverseIndex[k]].pos[2] = setupadq.data_stream_target[2048*l + i+1];
                    psd_res[512*l + BitReverseIndex[k]].pos[1] = setupadq.data_stream_target[2048*l + i+4];
                    psd_res[512*l + BitReverseIndex[k]].pos[0] = setupadq.data_stream_target[2048*l + i+5];
                    psd_res[512*l + BitReverseIndex[k+1]].pos[3] = setupadq.data_stream_target[2048*l + i+2];
                    psd_res[512*l + BitReverseIndex[k+1]].pos[2] = setupadq.data_stream_target[2048*l + i+3];
                    psd_res[512*l + BitReverseIndex[k+1]].pos[1] = setupadq.data_stream_target[2048*l + i+6];
                    psd_res[512*l + BitReverseIndex[k+1]].pos[0] = setupadq.data_stream_target[2048*l + i+7];
                }
                else
                {
                    psd_res[512*l + 511 - k].pos[3] = setupadq.data_stream_target[2048*l + i];
                    psd_res[512*l + 511 - k].pos[2] = setupadq.data_stream_target[2048*l + i+1];
                    psd_res[512*l + 511 - k].pos[1] = setupadq.data_stream_target[2048*l + i+4];
                    psd_res[512*l + 511 - k].pos[0] = setupadq.data_stream_target[2048*l + i+5];
                    psd_res[512*l + 511 - k-1].pos[3] = setupadq.data_stream_target[2048*l + i+2];
                    psd_res[512*l + 511 - k-1].pos[2] = setupadq.data_stream_target[2048*l + i+3];
                    psd_res[512*l + 511 - k-1].pos[1] = setupadq.data_stream_target[2048*l + i+6];
                    psd_res[512*l + 511 - k-1].pos[0] = setupadq.data_stream_target[2048*l + i+7];
                }

                i = i + 8;
                qDebug()<<"Union.Spec["<<BitReverseIndex[k]<<"] = "<<psd_res[512*l + BitReverseIndex[k]].data64;
                qDebug()<<"Union.Spec["<<BitReverseIndex[k+1]<<"] = "<<psd_res[512*l + BitReverseIndex[k+1]].data64;
            }
        qDebug() << "Writing streamed Spectrum data in RAM to disk" ;
        QFile Specfile("data_Spec.txt");
        if(Specfile.open(QFile::WriteOnly))
        {
            QTextStream out(&Specfile);
            for (k=0; (k<psd_datanum); k++)
                out <<psd_res[k].data64 << endl;
        }
        Specfile.close();
    }
}

void MainWindow::Display_Data()                   // 显示数据
{
    switch(setupadq.stream_ch)
    {
    case ADQ214_STREAM_ENABLED_BOTH:
    {
        unsigned int samples_to_collect;
        samples_to_collect = setupadq.num_samples_collect;
        rowCHA.resize((setupadq.num_samples_collect));      //设置数组大小为采集点数
        rowCHB.resize((setupadq.num_samples_collect));
        int j=0,k=0;         //用于双通道计数
        while (samples_to_collect > 0)
        {
            for (int i=0; (i<4) && (samples_to_collect>0); i++)
            {
                rowCHA[j] = setupadq.data_stream_target[setupadq.num_samples_collect - samples_to_collect];
                j++;
                samples_to_collect--;
            }

            for (int i=0; (i<4) && (samples_to_collect>0); i++)
            {
                rowCHB[k] = setupadq.data_stream_target[setupadq.num_samples_collect - samples_to_collect];
                k++;
                samples_to_collect--;
            }
        }

        lineChart.line(rowCHA,(setupadq.num_samples_collect)/2);  //数组传给linechart
        drawLayoutCHA->addWidget(lineChart.chartView);
        lineChart.line(rowCHB,(setupadq.num_samples_collect)/2);
        drawLayoutCHB->addWidget(lineChart.chartView);

        break;
    }
    case ADQ214_STREAM_ENABLED_A:
    {
        rowCHA.resize(setupadq.num_samples_collect);

        for (int i=0; i<setupadq.num_samples_collect; i++)
        {
            rowCHA[i] = setupadq.data_stream_target[i];
        }

        lineChart.line(rowCHA,setupadq.num_samples_collect);  //数组传给linechart
        drawLayoutCHA->addWidget(lineChart.chartView);
        // b.chartView->setWindowFlags(Qt::Window|Qt::WindowMinimizeButtonHint
        //               |Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);

        break;
    }
    case ADQ214_STREAM_ENABLED_B:
    {
        rowCHB.resize(setupadq.num_samples_collect);

        for (int i=0; i<setupadq.num_samples_collect; i++)
        {
            rowCHB[i] = setupadq.data_stream_target[i];
        }

        lineChart.line(rowCHB,setupadq.num_samples_collect);  //数组传给linechart
        drawLayoutCHB->addWidget(lineChart.chartView);

        break;
    }
    default:
        break;
    }
}

void MainWindow::ConvertPSDUnionToArray(PSD_DATA *psd_res)
{
    int psd_num = setupadq.num_samples_collect/4;
    qDebug() << "psd Num = " << psd_num;
    psd_array = new double[psd_num];
    memset(psd_array,0,psd_num*sizeof(double));
    for (int k=0; k<psd_num; k++) {
        qDebug() << double(psd_res[k].data64);
        psd_array[k] = double(psd_res[k].data64);
    }
}

void MainWindow::LOSVelocityCal(const int heightNum, const int totalSpecPoints,
                                const int objSpecPoints, const double lambda,
                                const double *freqAxis, const double *specData)
{
    double *aomSpec = new double[totalSpecPoints];
    double *specArray = new double[(heightNum-2)*totalSpecPoints];
    for (int k = 0; k < totalSpecPoints; k++) {
        aomSpec[k] = specData[totalSpecPoints+k] - specData[k];
        for (int l = 0; l < heightNum - 2; l++){
            specArray[l*totalSpecPoints+k] = specData[totalSpecPoints*(l+2) + k] - specData[k];
        }
    }

    for (int i=0; i<totalSpecPoints; i++) {
        qDebug() << aomSpec[i];
    }
    int aomIndex = 0;
    double temp = aomSpec[0];
    for (int k = 1; k < totalSpecPoints; k++) {
        if (aomSpec[k] > temp) {
            temp = aomSpec[k];
            aomIndex = k;
        }
    }

    qDebug() << aomIndex;
    int startIndex = aomIndex - objSpecPoints;
    int endIndex = aomIndex + objSpecPoints;

    int *losVelocityIndex = new int[heightNum - 2];
    temp = 0;
    for (int l = 0; l < heightNum -2; l++) {
        losVelocityIndex[l] = startIndex;
        temp = specArray[l*totalSpecPoints+ startIndex];
        for (int k = startIndex + 1; k <= endIndex; k++) {
            if (specArray[l*totalSpecPoints+ k] >temp) {
                temp = specArray[l*totalSpecPoints+ k];
                losVelocityIndex[l] = k;
            }
        }
    }

    for (int l = 0; l < heightNum -2; l++) {
        qDebug() << losVelocityIndex[l];
    }

    losVelocity = new double[heightNum-2];
    memset(losVelocity, 0, sizeof(double)*(heightNum-2));
    for(int i=0; i<heightNum-2; i++) {
        losVelocity[i] = (freqAxis[losVelocityIndex[i]] - freqAxis[aomIndex])*lambda/2;
    }
    delete[] aomSpec;
    delete[] specArray;
    delete[] losVelocityIndex;
}

// 镜面回波长度改变
void MainWindow::on_lineEdit_MirrorLength_textChanged(const QString &arg1)
{
    int MirrorLength = arg1.toInt();
    int nPointsPerRB = ui->lineEdit_toFPGA_3->text().toInt();
    int UR_MirrorStart = 500 + MirrorLength - nPointsPerRB;

    ui->lineEdit_toFPGA_6->setText(QString::number(UR_MirrorStart));
    ui->lineEdit_MirrorLength_ns->setText(QString::number(MirrorLength*2.5));
    on_checkBox_Overlap_clicked(ui->checkBox_Overlap->isChecked());
}

// OverLap 是否勾选
void MainWindow::on_checkBox_Overlap_clicked(bool checked)
{

    int nPointsPerRB = ui->lineEdit_toFPGA_3->text().toInt();
    int nRangeBin = ui->lineEdit_toFPGA_4->text().toInt();
    int MirrorLength = ui->lineEdit_MirrorLength->text().toInt();
    int nSamples2Collect;

    if(checked)
    {
        int Overlap_startPosition = (nRangeBin-3.5)*nPointsPerRB;
        ui->lineEdit_toFPGA_5->setText(QString::number(Overlap_startPosition));

        nSamples2Collect = 512*4*(nRangeBin * 2 -5);        // 设置采集点数
        ui->lineEdit_SampTotNum->setText(QString::number(nSamples2Collect));

    }
    else
    {
        ui->lineEdit_toFPGA_5->setText("0");

        nSamples2Collect = 512*4*(nRangeBin - 1);        // 设置采集点数
        ui->lineEdit_SampTotNum->setText(QString::number(nSamples2Collect));
    }

    int UR_EndPosition = (nRangeBin-3)*nPointsPerRB + 500+ MirrorLength;     // 设置FPGA数据采样长度
    ui->lineEdit_toFPGA_7->setText(QString::number(UR_EndPosition));

}

void MainWindow::on_pushButton_ReadFile_clicked()
{
    ui->lineEdit_toFPGA_0->setText(QString::number(MySetting.Command));
    ui->lineEdit_toFPGA_1->setText(QString::number(MySetting.TrigLevel));
    ui->lineEdit_toFPGA_2->setText(QString::number(MySetting.Nof_PulsesAcc));
    ui->lineEdit_toFPGA_3->setText(QString::number(MySetting.Nof_PointsPerBin));
    ui->lineEdit_toFPGA_4->setText(QString::number(MySetting.Nof_RangeBin));
    ui->lineEdit_toFPGA_5->setText(QString::number(MySetting.Overlap));
    ui->lineEdit_toFPGA_6->setText(QString::number(MySetting.MirrorStart));
    ui->lineEdit_toFPGA_7->setText(QString::number(MySetting.PointsOfProcess));
}
