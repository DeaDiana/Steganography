#ifndef _RS_ANALYZER_H_
#define _RS_ANALYZER_H_

#include <string>
#include <exception>
#include <map>

enum SIGN_MASK {
	PLUS_MASK,
	MINUS_MASK
};

enum MASK_VALUE {
	MINUS_ONE,
	ZERO,
	PLUS_ONE
};

enum GROUPS {
	REGULAR_M,
	SINGULAR_M,
	REGULAR_MM,
	SINGULAR_MM
};

class RSAnalyzerException : public std::exception
{
public:
	RSAnalyzerException ( const std::string& message ) throw() : errorMessage ( message ) {}
	virtual std::string getMessage() {
		return errorMessage;
	}
	virtual ~RSAnalyzerException() throw() {}
private:
	std::string errorMessage;
};


class RSAnalyzer {
public:
	RSAnalyzer(void);
	~RSAnalyzer(void);
	void setMask(const std::string & mask);
	void setLengthOfMessage(const std::string & percent);
	float lenghOfEmeddedMessge (const std::string & filename);
	static void help();
private:
	int header_sz;
	long width;
	long height;
	int gray_value;
	MASK_VALUE * mask;
	int mask_length;
	std::string filename;
	std::string copyfilename;
	std::string icopyfilename;
	long file_sz;
	float msg_percent;
	long message_sz;
	template <typename T> long countDiscriminant(const T * group);
	short * flipGruopWithMask(char *  group, short *  fgroup, SIGN_MASK);
	short flipByte(const char & byte, const MASK_VALUE mask_value);
	void embedLSBMessage();
	void checkFilePGMHeader(const std::string & filename);
	template <typename T> 
	T stringToNumber(const std::string& text);
	std::map<GROUPS, float> countRSGroups(const std::string & filename);
	void invertAllLSBs(const std::string & filename);
};

/* specification for PGM format
*  http: //netpbm.sourceforge.net/doc/pgm.html */

#endif /*_RS_ANALYZER_H_*/
