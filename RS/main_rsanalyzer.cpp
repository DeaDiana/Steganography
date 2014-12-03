#include <iostream>
#include <fstream>
#include <time.h>
#include "parsing_cmd_line.h"
#include "RSAnalyzer.h"

#define LINE "\n_________________________________________"
#define TIMER_MSG "\nElapsed time: "
#define MS_IN_SEC 1000
#define IMG_IS_STEGO "\nGiven image probably contains embedded message."
#define IMG_IS_ORIGINAL "\nGiven image is probably orginal."

void showElapsedTime (long start_time);

int main(int argc, char * argv[]) {
	if (1 == argc) {
		//std::cout << NFLGS_MSG << std::endl;
		return 0;
	}
	RSAnalyzer rsanalyzer;
	long start_time = 0;
	try {
		std::map <ArgumentsFlags, std::string> paramters = parseCommandLineIntoTheMapOfParameters(argc, argv);
		if (paramters.empty()) {
			//std::cout << NFLGS_MSG << std::endl;
			return 0;
		}
		std::map <ArgumentsFlags, std::string>::iterator it_sub = paramters.begin();
		start_time = clock();
		for( std::map <ArgumentsFlags, std::string>::iterator it = paramters.begin(); 
			 it != paramters.end(); 
			 it++ ) {
			switch (it->first) {
				case FILE_FLAG:
					rsanalyzer.IsStegoImage(paramters.at(FILE_FLAG)) ?
						std::cout << IMG_IS_STEGO << std::endl :
						std::cout << IMG_IS_STEGO << std::endl ;
					
				case HELP_FLAG:
					//Helper::printHelp();
					showElapsedTime(start_time);
					return 0;
				default:
					break;
			}
		}
	} catch (ParseCmdLineException & e) {
		//std::cerr << e.getMessage() <<std::endl;
	} catch (RSAnalyzerException & e) {
		//std::cerr << e.getMessage() <<std::endl;
	} catch (std::exception & e) {
		//std::cerr << ERROR_MSG_COMMON << std::endl << e.what() << std::endl;
	}
	showElapsedTime(start_time);
	return 0;
}

void showElapsedTime (long start_time) {
	std::cout << LINE << TIMER_MSG << (clock() - start_time) * MS_IN_SEC / (double) CLOCKS_PER_SEC << " ms"<< std::endl;
}
