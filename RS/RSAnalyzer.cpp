#include <fstream>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>
#include <time.h>
#include <Windows.h>
#include "RSAnalyzer.h"

#define BUFFER_SZ 1024
#define MAGICNUM_SZ 2
#define MAGICNUM "P5"
#define PMAGICNUM "P2"
#define GROUP_SZ 4
#define MSG_LEN 231000
#define CMMNT_SIGN '#'
#define NUM_FLINFO 3
#define OVRWRT false
#define HNDR_PERC 100

RSAnalyzer::RSAnalyzer(void) {
	mask_length = GROUP_SZ;
	mask = new MASK_VALUE[GROUP_SZ]; 
	mask[0] = ZERO;
	mask[1] = PLUS_ONE;
	mask[2] = PLUS_ONE;
	mask[3] = ZERO;
	copyfilename = "copy_";
	icopyfilename = "i";
	header_sz = 0;
	message_sz = 0;
	msg_percent = 50.0;
}

RSAnalyzer::~RSAnalyzer(void) {
	delete mask;
}

void RSAnalyzer::setMask(const std::string & mask_str) {
	std::stringstream ss(mask_str);
	mask_length = mask_str.length();
	mask = new MASK_VALUE[mask_length]; 
	char val = 0;
	for(int i = 0; i < mask_length; i++) {
		ss >> val;
		switch (val) {
			case '1' : 
				mask[i] = PLUS_ONE;
				break;
			case '0' : 
				mask[i] = ZERO;
				break;
			default : 
				throw RSAnalyzerException ("\nWrong mask. 1 or 0 should be in mask.");
		}
	}
}

float RSAnalyzer::lenghOfEmeddedMessge (const std::string & flname) {
	filename = flname;
	checkFilePGMHeader(filename);
	embedLSBMessage();
	std::map<GROUPS, float> cfs = countRSGroups(copyfilename);
	float d0_coef = cfs[REGULAR_M] - cfs[SINGULAR_M];
	float dm0_coef = cfs[REGULAR_MM] - cfs[SINGULAR_MM];
	invertAllLSBs(copyfilename);
	cfs = countRSGroups(icopyfilename);
	float d1_coef = cfs[REGULAR_M] - cfs[SINGULAR_M];
	float dm1_coef = cfs[REGULAR_MM] - cfs[SINGULAR_MM];
	float a_cf = 2 * (d1_coef + d0_coef);
	float b_cf = dm0_coef - dm1_coef - d1_coef - 3 * d0_coef;
	float c_cf = d0_coef - dm0_coef;
	float discr = b_cf * b_cf - 4 * a_cf * c_cf;
	float res1 = (-b_cf + sqrt(discr)) / (2 * a_cf);
	float res2 = (-b_cf - sqrt(discr)) / (2 * a_cf);
	abs(res1) > abs(res2) ? res1 = res2 : res1 = res1;
	float length_message = res1 / (res1 - 0.5);
	std::cout << "\n\nDetected" << std::endl;
	std::cout << "Length message in percents:\t" << length_message * HNDR_PERC << std::endl;
	return length_message * width * height;
}

void RSAnalyzer::checkFilePGMHeader(const std::string & flname) {
	filename = flname;
	std::ifstream in(filename);
	if(!in.is_open()) {
		throw RSAnalyzerException ("\nCould not open the file:\n\n" + filename);
	}
	std::vector<std::string> file_info;
	std::string row;
	in >> row;
	if(row.compare(MAGICNUM) && row.compare(PMAGICNUM)) {
		throw RSAnalyzerException("\nOnly PGM image format is supported.");
		exit(-1);
	}
	header_sz += row.length() + 1;
	do {
		in >> row;
		if(CMMNT_SIGN != row.at(0)) {
			file_info.push_back(row);
		} else {
			header_sz += row.length() + 1;
			std::getline(in, row, '\n');
		}
		header_sz += row.length() + 1;
	} while (NUM_FLINFO != file_info.size());
	gray_value = stringToNumber<long>(file_info.back());
	file_info.pop_back();
	height = stringToNumber<long>(file_info.back());
	file_info.pop_back();
	width = stringToNumber<long>(file_info.back());
	if ((width * height) % mask_length) {
		throw RSAnalyzerException ("\nLength of mask should devide the image size.");
	}
	in.close();
	message_sz = width * height * msg_percent / HNDR_PERC;
	HANDLE hF = CreateFile(filename.c_str(), GENERIC_READ, 0,                           
						NULL, OPEN_EXISTING, 0, NULL);
	
	file_sz = GetFileSize(hF, NULL);
	CloseHandle(hF);
	std::cout << "File:\t" << filename <<std::endl;
	std::cout << "Size:\t" << width << "x" << height <<"   " << width * height  << " bytes" <<std::endl;
}

void RSAnalyzer::embedLSBMessage() {
	copyfilename.append(filename);
	if (! CopyFile(filename.c_str(), copyfilename.c_str(), OVRWRT)) {
		throw RSAnalyzerException ("\nCould not copy original file to embed message. File:\n\n" + filename);
	}
	//std::ifstream in(filename)
	std::fstream img(copyfilename, std::ios::binary | std::ios::in | std::ios::out);
	if(!img.is_open()) {
		throw RSAnalyzerException ("\nCould not open the file:\n\n" + filename);
	}
	srand(time(NULL));
//	embed
	char byte = 0;
	char byte1 = 0;
	int counter = 0;
	int pos = header_sz + 1;
	for (int i = 0; i < message_sz; i++) {
		pos += rand() % 10;//(file_sz - header_sz);
		img.seekg(pos);
		img.read(&byte, sizeof(char));
		byte1 = byte | rand() % 2;//1
		img.seekg(pos);
		img.write(&byte1, sizeof(char));
		if (byte != byte1) {
			counter++;
		}
		byte = 0;
	}
	img.close();
	std::cout << "Message length in percents:\t" << msg_percent << std::endl;
	std::cout << "Message length in bytes:\t" << message_sz << std::endl;
	std::cout << "Number of bytes were changed:\t" << counter << std::endl;
}

std::map<GROUPS, float> RSAnalyzer::countRSGroups(const std::string & flname) {
	std::map<GROUPS, float> result; 
	result.emplace(REGULAR_M, 0.0);
	result.emplace(SINGULAR_M, 0.0);
	result.emplace(REGULAR_MM, 0.0);
	result.emplace(SINGULAR_MM, 0.0);
	char buffer [BUFFER_SZ] = {0};
	std::ifstream in(flname, std::ifstream::binary);
	if (!in.is_open()) {
		throw RSAnalyzerException("\nCan't open file:\n\n" + flname);
	}
	in.seekg(header_sz);
	in.read(buffer, BUFFER_SZ);
	if (!in.good() && !in.eof()) {
		std::cout << GetLastError() << std::endl;
		throw RSAnalyzerException("\nError with reading. The file:\n\n" + flname);
	}
	int rnum = in.gcount();
	char * group = new char[mask_length];
	short * fgroup = new short[mask_length];
	long discriminant = 0;
	long flipped_discriminant = 0;
	long flipped_d_minus_mask = 0;
//	reading bytes of image
	while (BUFFER_SZ >= rnum) {
		for (int i = 0; i < rnum; i += mask_length) {
			memcpy (group, buffer + i, mask_length * sizeof (char));
			discriminant = countDiscriminant<char>(group);
			flipped_discriminant = countDiscriminant<short>(flipGruopWithMask(group, fgroup, PLUS_MASK));
			flipped_d_minus_mask = countDiscriminant<short>(flipGruopWithMask(group, fgroup, MINUS_MASK));
			if (discriminant < flipped_discriminant ) {
				result[REGULAR_M] ++;
			}
			if (discriminant > flipped_discriminant ) {
				result[SINGULAR_M] ++;
			}
			if (discriminant < flipped_d_minus_mask ) {
				result[REGULAR_MM] ++;
			}
			if (discriminant > flipped_d_minus_mask ) {
				result[SINGULAR_MM] ++;
			}
		}
		memset(buffer, 0, sizeof(char) * BUFFER_SZ);
		if (in.eof()) {
			break;
		}
		in.read (buffer, BUFFER_SZ);
		if (!in.good() && !in.eof()) {
			throw RSAnalyzerException("\nError with reading. The file:\n\n" + filename);
		}
		rnum = in.gcount();
	}
	float num_groups = (file_sz - header_sz) / static_cast<float>(GROUP_SZ);
	result[REGULAR_M] = result[REGULAR_M] * HNDR_PERC / num_groups;
	result[SINGULAR_M] = result[SINGULAR_M] * HNDR_PERC / num_groups;
	result[REGULAR_MM] = result[REGULAR_MM] * HNDR_PERC / num_groups;
	result[SINGULAR_MM] = result[SINGULAR_MM] * HNDR_PERC / num_groups;
	delete group;
	delete fgroup;
	return result;
}

template <typename T> 
long RSAnalyzer::countDiscriminant(const T * group) {
	long result = 0;
	for (int i = 0; i < mask_length - 1; i++) {
		result += abs(group[i + 1] - group[i]);
	}
return result;
}

short * RSAnalyzer::flipGruopWithMask(char * group, short * fgroup, SIGN_MASK sign) {
	MASK_VALUE * mask_apply = new MASK_VALUE[mask_length];
	memcpy (mask_apply, mask, sizeof(MASK_VALUE) * mask_length);
	if (MINUS_MASK == sign) {
		for (int i = 0; i < mask_length; i++) {
			if (MINUS_ONE == mask_apply[i]) {
				mask_apply[i] = PLUS_ONE;
			}
			if (PLUS_ONE == mask_apply[i]) {
				mask_apply[i] = MINUS_ONE;
			}
		}
	}
	for (int i = 0; i < mask_length; i++) {
		fgroup[i] = flipByte(group[i], mask_apply[i]);
	}
	delete mask_apply;
return fgroup;
}

short RSAnalyzer::flipByte(const char & byte, const MASK_VALUE mask_value) {
	short result = 0;
	if (PLUS_ONE == mask_value) {
		(byte % 2) ? result = byte - 1 : result = byte + 1;
		return result;
	}
	if (MINUS_ONE == mask_value) {
		(byte % 2) ? result = byte + 1 : result = byte - 1;
		return result;
	}
	result = static_cast<short>(byte);
return result;
}

template <typename T> 
T RSAnalyzer::stringToNumber(const std::string& text)
{
	std::stringstream ss(text);
	T result;
	return ss >> result ? result : T(NULL);
}

void RSAnalyzer::invertAllLSBs(const std::string & flname) {
	icopyfilename.append(flname);
	std::ifstream in(copyfilename, std::ios::binary);
	std::ofstream out(icopyfilename, std::ios::binary);
	if(!in.is_open()) {
		throw RSAnalyzerException ("\nCould not open the file:\n\n" + icopyfilename);
	}
	if(!out.is_open()) {
		throw RSAnalyzerException ("\nCould not open the file:\n\n" + icopyfilename);
	}
	char buffer [BUFFER_SZ] = {0};
	in.read(buffer, header_sz);
	out.write(buffer, header_sz);
	memset(buffer, 0, sizeof(char) * BUFFER_SZ);
	in.read(buffer, BUFFER_SZ);
	if (!in.good() && !in.eof()) {
		throw RSAnalyzerException("\nError with reading. The file:\n\n" + copyfilename);
	}	
	int rnum = in.gcount();
//	reading bytes of image
	while (BUFFER_SZ >= rnum) {
		for (int i = 0; i < rnum; i ++) {
			buffer[i] % 2 == 0 ? buffer[i] |= 1 : buffer[i] &= 0xFE;
		}
		out.write(buffer, BUFFER_SZ);
		memset(buffer, 0, sizeof(char) * BUFFER_SZ);
		if (in.eof()) {
			break;
		}
		in.read (buffer, BUFFER_SZ);
		if (!in.good() && !in.eof()) {
			throw RSAnalyzerException("\nError with reading. The file:\n\n" + filename);
		}
		rnum = in.gcount();
	}
	in.close();
	out.close();
}

void RSAnalyzer::setLengthOfMessage(const std::string & percent) {
	msg_percent = stringToNumber<float>(percent);
	if ((msg_percent < 0) || (msg_percent > 100)) {
		throw ("\nAfter flag -l should be pointed length of message in percents of image size.");
	}
}

void RSAnalyzer::help() {
	std::cout << 
		"RSAnalyzer tries detect embedded message in image.\n\
		It returns supposed length of message in percents.\n\n\
		You may run it with next flags:\n\
		-f : file path\n\
		-m : mask {0, 1}^n\n\
		-l : length of message to embed in percents of image\n\n\
		Current version of program takes image, embeds message into it, then tries detect embedding.\n\n\
		Implementation: Diana Anisutina."
		<< std::endl;
}
