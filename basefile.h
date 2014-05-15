#pragma once

// #include <iostream>
// #include <fstream>
// #include <string>
//
//class basefile
//{
//public:
//	basefile(){
//		mFilePath = "printsettings.ini";
//	}
//	~basefile(){
//		
//	}
//public:
//	bool read(){
//		std::ifstream is(mFilePath.c_str(), std::ifstream::in);
//		if (is){
//			is.seekg(0, is.end);
//			int len = is.tellg();
//			is.seekg(0, is.beg);
//
//			mStrBuf.resize(len, ' ');
//			char* begin = &*mStrBuf.begin();
//
//			is.read(begin, len);
//			is.close();
//
//			mIMEI = find("imei");
//			mPW = find("pw");
//			std::cout << mStrBuf << std::endl;
//			return true;
//		}
//		return false;
//	}
//// 	bool write(std::string& data){
//// 		std::ofstream os(mFilePath.c_str(), std::ifstream::out);
//// 		if (os){
//// 			os << mStrBuf << std::endl;
//// 		}
//// 	}
//	void setpath(std::string strpath){
//		mFilePath = strpath;
//	}
//	std::string getIMEI(){
//		return mIMEI;
//	}
//	std::string getPW(){
//		return mPW;
//	}
//private:
//	std::string find(std::string tag){
//		size_t pos = mStrBuf.find(tag);
//		if (pos != std::string::npos){
//			size_t beg = mStrBuf.find('[', pos);
//			size_t end = mStrBuf.find(']', beg);
//			return mStrBuf.substr(beg+1, end-beg);
//		}
//		return "";
//	}
//private:
//	std::string mPW;
//	std::string mIMEI;
//	
//	std::string mStrBuf;
//	std::string mFilePath;
//};
