#pragma once


// 把Unicode 转换成 GB2312 
void UnicodeToGB2312(char* pOut,unsigned short uData)
{
	WideCharToMultiByte(CP_ACP,NULL,(LPCWSTR)&uData,1,pOut,sizeof(WCHAR),NULL,NULL);
	return;
}     


// 把UTF-8转换成Unicode
void UTF_8ToUnicode(WCHAR* pOut,char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

//UTF-8 转为 GB2312
void UTF_8ToGB2312(std::string &pOut, char *pText, int pLen)
{
	char * newBuf = new char[pLen];
	char Ctemp[4];
	memset(Ctemp,0,4);

	int i =0;
	int j = 0;

	while(i < pLen)
	{
		if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];                       
		}
		else                 
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp,pText + i);

			UnicodeToGB2312(Ctemp,Wtemp);

			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	}
	newBuf[j] = 0;

	pOut = newBuf;
	delete []newBuf;

	return; 
}