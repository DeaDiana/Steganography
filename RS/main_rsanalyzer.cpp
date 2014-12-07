#include <iostream>
#include <fstream>
#include <time.h>
#include <Windows.h>
#include "parsing_cmd_line.h"
#include "RSAnalyzer.h"

#define LINE "\n_________________________________________"
#define TIMER_MSG "\nElapsed time: "
#define MS_IN_SEC 1000
#define IMG_IS_STEGO "\nGiven image probably contains embedded message."
#define IMG_IS_ORIGINAL "\nGiven image is probably orginal."
#define ERROR_MSG_COMMON "\nSomething goes wrong."
#define NFLGS_MSG "\nPlease look at the help (TestTask2GIS.exe -h)."

void showElapsedTime (long start_time);

int main(int argc, char * argv[]) {
	if (1 == argc) {
		std::cout << NFLGS_MSG << std::endl;
		return 0;
	}
	RSAnalyzer rsanalyzer;
	long start_time = 0;
	try {
		std::map <ArgumentsFlags, std::string> paramters = parseCommandLineIntoTheMapOfParameters(argc, argv);
		if (paramters.empty()) {
			std::cout << NFLGS_MSG << std::endl;
			return 0;
		}
		std::map <ArgumentsFlags, std::string>::iterator it_sub = paramters.find(MASK_FLAG);
		if(it_sub != paramters.end()) {
			rsanalyzer.setMask(it_sub->second);
		}
		it_sub = paramters.find(LNGMSG_FLAG);
		if(it_sub != paramters.end()) {
			rsanalyzer.setLengthOfMessage(it_sub->second);
		}
		start_time = clock();
		for( std::map <ArgumentsFlags, std::string>::iterator it = paramters.begin(); 
			 it != paramters.end(); 
			 it++ ) {
			switch (it->first) {
				case FILE_FLAG:
					std::cout << "Message length in bytes:\t" << rsanalyzer.lenghOfEmeddedMessge(paramters.at(FILE_FLAG)) << std::endl;
					showElapsedTime(start_time);
					return 0;
				case HELP_FLAG:
					RSAnalyzer::help();
					showElapsedTime(start_time);
					return 0;
				default:
					break;
			}
		}
	} catch (ParseCmdLineException & e) {
		std::cerr << e.getMessage() <<std::endl;
	} catch (RSAnalyzerException & e) {
		std::cerr << e.getMessage() <<std::endl;
	} catch (std::exception & e) {
		std::cerr << ERROR_MSG_COMMON << std::endl << e.what() << std::endl;
	} catch (...) {
		std::cerr << GetLastError() << std::endl;
	}
	showElapsedTime(start_time);
	return 0;
}

void showElapsedTime (long start_time) {
	std::cout << LINE << TIMER_MSG << (clock() - start_time) * MS_IN_SEC / (double) CLOCKS_PER_SEC << " ms"<< std::endl;
}
