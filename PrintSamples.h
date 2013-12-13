#ifndef PRINT_SAMPLES_H
#define PRINT_SAMPLES_H
#pragma once

// -----------------------------------------------------------------------------
// ��ӡͷ���ԼΪ 80mm �Ĵ�ӡ����

BOOL PrintInStandardMode80(void);
BOOL PrintInPageMode80(void);

// -----------------------------------------------------------------------------
// ��ӡͷ���ԼΪ 56mm �Ĵ�ӡ����

BOOL PrintInStandardMode56(char *print_str);
BOOL PrintInPageMode56(void);
BOOL startPrinterPOS();
BOOL endPrinterPOS();

#endif