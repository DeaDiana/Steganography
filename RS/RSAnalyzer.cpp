#include "RSAnalyzer.h"
#include <fstream>

#define BUFFER_SZ 1024
#define MAGICNUM_SZ 2
#define MAGICNUM "P5"

RSAnalyzer::RSAnalyzer(void) {
}


RSAnalyzer::~RSAnalyzer(void) {
}

bool RSAnalyzer::IsStegoImage (const std::string & filename) {
	std::ifstream in(filename);
	if(!in.is_open()) {
		throw RSAnalyzerException ("\nCould not open the file:\n\n" + filename);
	}
	char buffer[BUFFER_SZ] = {0};
	in.read(buffer, MAGICNUM_SZ);
	buffer[MAGICNUM_SZ] = '\0';
	std::string magicnum(buffer);
	if (magicnum.compare(MAGICNUM)) {
		throw RSAnalyzerException("\nOnly PGM image format is supported.");
		exit(-1);
	}
	long width = 0;
	long height = 0;
	int gray_value = 0;
	in >> width >> height >> gray_value;
//	algorithm
return true;
}

long RSAnalyzer::lenghOfEmeddedMessge (const std::string & filename) {
return 0;
}

long RSAnalyzer::countDiscriminant(const GroupOfPixels & group) {
return 0;
}

GroupOfPixels RSAnalyzer::flipGruopWithMask(const GroupOfPixels & group, const bool * mask) {
return GroupOfPixels();
}

GroupOfPixels RSAnalyzer::flipGruop(const GroupOfPixels & group, const bool mask) {
return GroupOfPixels();
}
