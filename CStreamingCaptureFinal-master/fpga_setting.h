#ifndef FPGA_SETTING_H
#define FPGA_SETTING_H
#include "fpga_setting_defines.h"
#include <QString>

class FPGA_Setting
{
public:
    FPGA_Setting(const QString &path);

    void CreatSettingFile(const QString &path);
    void CreatFactorFile(const QString &path);

    void WriteSettingFile(const FPGA_SETTING_DEFINES &CurrentSetting,const QString &path);

    FPGA_SETTING_DEFINES ReadSettingFile(const QString &path);
    int *ReadFactorFile(const QString &path);

private:
    int factor[512]={0};
};

#endif // FPGA_SETTING_H
