#ifndef FPGA_SETTING_DEFINES_H
#define FPGA_SETTING_DEFINES_H

#include <QString>
typedef struct
{
    int Command,Command_0x;                       //命令
    int TrigLevel,TrigLevel_0x;                   //触发电平
    int Nof_PulsesAcc,Nof_PulsesAcc_0x;           //累加脉冲数
    int Nof_PointsPerBin,Nof_PointsPerBin_0x;     //距离门内点数
    int Nof_RangeBin,Nof_RangeBin_0x;             //距离门数
    int Overlap,Overlap_0x;                       //OverLap+Ur3/2
    int MirrorStart,MirrorStart_0x;               //MirrorStart
    int PointsOfProcess,PointsOfProcess_0x;       //处理结束点数
}FPGA_SETTING_DEFINES;
#endif // FPGA_SETTING_DEFINES_H
