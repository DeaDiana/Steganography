#ifndef _PARSING_CMD_LINE_H_
#define _PARSING_CMD_LINE_H_

#include <map>
#include <string>

enum ArgumentsFlags { 
	FILE_FLAG, 
	MASK_FLAG,
	LNGMSG_FLAG,
	HELP_FLAG
};

class ParseCmdLineException : public std::exception
{
public:
	ParseCmdLineException ( const std::string& message ) throw() : errorMessage ( message ) {}
	virtual std::string getMessage() {
		return errorMessage;
	}
	virtual ~ParseCmdLineException() throw() {}
private:
	std::string errorMessage;
};

std::map <ArgumentsFlags, std::string> parseCommandLineIntoTheMapOfParameters(int argc, char * argv[]);
std::string extractOneParameter(const int argc, char * argv[], const int start_position, int * end_position);

#endif /*_PARSING_CMD_LINE_H_*/
