#include "fpga_setting.h"
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QDataStream>
#include <QTextStream>
#include <QFileDialog>

FPGA_Setting::FPGA_Setting(const QString &path)
{
    CreatSettingFile(path);
}

FPGA_Setting::FPGA_Setting()
{

}

void FPGA_Setting::CreatSettingFile(const QString &path)
{
    QFile settingfile(path+"/"+"FPGA_SettingFile.ini");           //创建FPGA设置文件
    QSettings setting(path+"/"+"FPGA_SettingFile.ini",QSettings::IniFormat);
    if(settingfile.exists())
    {
        qDebug()<<"Setting file is existing";
    }
    else
    {        
        setting.beginGroup("FPGA_Setting");
        setting.setValue("command"             ,"0");
        setting.setValue("TrigLevel"           ,"500");
        setting.setValue("Nof_PulsesAcc"       ,"5000");
        setting.setValue("Nof_PointsPerBin"    ,"250");
        setting.setValue("Nof_RangeBin"        ,"14");
        setting.setValue("Overlap"             ,"0");
        setting.setValue("MirrorStart"         ,"430");
        setting.setValue("PointsOfProcess"     ,"3430");
        setting.setValue("address"             ,"010");
        setting.endGroup();

        setting.beginGroup("FPGA_Setting_0x");
        setting.setValue("command_0x"          ,"0");
        setting.setValue("TrigLevel_0x"        ,"1F4");
        setting.setValue("Nof_PulsesAcc_0x"    ,"1388");
        setting.setValue("Nof_PointsPerBin_0x" ,"FA");
        setting.setValue("Nof_RangeBin_0x"     ,"E");
        setting.setValue("Overlap_0x"          ,"0");
        setting.setValue("MirrorStart_0x"      ,"1AE");
        setting.setValue("PointsOfProcess_0x"  ,"D66");
        setting.endGroup();
        qDebug()<<"Creat defult setting file successfully!!!";
    }
}

FPGA_SETTING_DEFINES FPGA_Setting::ReadSettingFile(const QString &path)
{
    qDebug()<<"Read setting file!!";
    FPGA_SETTING_DEFINES FSetting;
    QSettings setting(path+"/"+"FPGA_SettingFile.ini",QSettings::IniFormat);

    setting.beginGroup("FPGA_Setting");
    FSetting.Command = setting.value("command").toInt();
    FSetting.TrigLevel = setting.value("TrigLevel").toInt();
    FSetting.Nof_PulsesAcc = setting.value("Nof_PulsesAcc").toInt();
    FSetting.Nof_PointsPerBin = setting.value("Nof_PointsPerBin").toInt();
    FSetting.Nof_RangeBin = setting.value("Nof_RangeBin").toInt();
    FSetting.Overlap = setting.value("Overlap").toInt();
    FSetting.MirrorStart = setting.value("MirrorStart").toInt();
    FSetting.PointsOfProcess = setting.value("PointsOfProcess").toInt();
    FSetting.address = setting.value("address").toInt();
    setting.endGroup();

    setting.beginGroup("FPGA_Setting_0x");
    FSetting.Command_0x = setting.value("command_0x").toInt();
    FSetting.TrigLevel_0x = setting.value("TrigLevel_0x").toInt();
    FSetting.Nof_PulsesAcc_0x = setting.value("Nof_PulsesAcc_0x").toInt();
    FSetting.Nof_PointsPerBin_0x = setting.value("Nof_PointsPerBin_0x").toInt();
    FSetting.Nof_RangeBin_0x = setting.value("Nof_RangeBin_0x").toInt();
    FSetting.Overlap_0x = setting.value("Overlap_0x").toInt();
    FSetting.MirrorStart_0x = setting.value("MirrorStart_0x").toInt();
    FSetting.PointsOfProcess_0x = setting.value("PointsOfProcess_0x").toInt();
    setting.endGroup();

    return FSetting;
}

void FPGA_Setting::WriteSettingFile(const FPGA_SETTING_DEFINES &CurrentSetting,const QString &path)
{
    QSettings setting(path+"/"+"FPGA_SettingFile.ini",QSettings::IniFormat);
    setting.beginGroup("FPGA_Setting");
    setting.setValue("command",CurrentSetting.Command);
    setting.setValue("TrigLevel",CurrentSetting.TrigLevel);
    setting.setValue("Nof_PulsesAcc",CurrentSetting.Nof_PulsesAcc);
    setting.setValue("Nof_PointsPerBin",CurrentSetting.Nof_PointsPerBin);
    setting.setValue("Nof_RangeBin",CurrentSetting.Nof_RangeBin);
    setting.setValue("Overlap",CurrentSetting.Overlap);
    setting.setValue("MirrorStart",CurrentSetting.MirrorStart);
    setting.setValue("PointsOfProcess",CurrentSetting.PointsOfProcess);
    setting.setValue("address",CurrentSetting.address);
    setting.endGroup();

    setting.beginGroup("FPGA_Setting_0x");
    setting.setValue("command_0x",CurrentSetting.Command_0x);
    setting.setValue("TrigLevel_0x",CurrentSetting.TrigLevel_0x);
    setting.setValue("Nof_PulsesAcc_0x",CurrentSetting.Nof_PulsesAcc_0x);
    setting.setValue("Nof_PointsPerBin_0x",CurrentSetting.Nof_PointsPerBin_0x);
    setting.setValue("Nof_RangeBin_0x",CurrentSetting.Nof_RangeBin_0x);
    setting.setValue("Overlap_0x",CurrentSetting.Overlap_0x);
    setting.setValue("MirrorStart_0x",CurrentSetting.MirrorStart_0x);
    setting.setValue("PointsOfProcess_0x",CurrentSetting.PointsOfProcess_0x);
    setting.endGroup();
    qDebug()<<"Current setting has been save";
}

void FPGA_Setting::CreatFactorFile(unsigned int *Hfactor)        //创建factor_file用于对比
{
    QDir dir;
    QFile factorFile(dir.currentPath()+"/"+"factor_file.txt");
    factorFile.open( QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream data(&factorFile);
    for(int i=0;i<512;i++)
    {
        data<<Hfactor[i];
        data<<endl;
    }
    factorFile.close();
    qDebug()<<"Creat factor_file successfully!";
}

unsigned int *FPGA_Setting::ReadFactorFile( QString &path)         //读取目标路径下的factor_file
{
    qDebug()<<"Read factor_file!!";

    QFile factorFile(path);
    factorFile.open(QIODevice::ReadOnly);
    QTextStream data(&factorFile);
    for(int i=0;i<512;i++)
    {
        data>>factor[i];
    }
    factorFile.close();

    return factor;
}
