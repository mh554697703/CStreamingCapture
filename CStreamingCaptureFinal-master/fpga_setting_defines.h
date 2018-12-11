#ifndef FPGA_SETTING_DEFINES_H
#define FPGA_SETTING_DEFINES_H

#include <QString>
typedef struct
{
    int Command,Command_0x;                       //����
    int TrigLevel,TrigLevel_0x;                   //������ƽ
    int Nof_PulsesAcc,Nof_PulsesAcc_0x;           //�ۼ�������
    int Nof_PointsPerBin,Nof_PointsPerBin_0x;     //�������ڵ���
    int Nof_RangeBin,Nof_RangeBin_0x;             //��������
    int Overlap,Overlap_0x;                       //OverLap+Ur3/2
    int MirrorStart,MirrorStart_0x;               //MirrorStart
    int PointsOfProcess,PointsOfProcess_0x;       //�����������
}FPGA_SETTING_DEFINES;
#endif // FPGA_SETTING_DEFINES_H
