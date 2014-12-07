#include "parsing_cmd_line.h"

// function construct set of parameters from command line
std::map <ArgumentsFlags, std::string> parseCommandLineIntoTheMapOfParameters(int argc, char * argv[])
{
	std::map <ArgumentsFlags, std::string> set_of_parameters;
	int i = 1;
	int end_position = argc;
	while((i < argc) && ('-' != argv[i][0])) {
		i++;
	}
	while((i < argc)) {
		if('-' == argv[i][0]) {
			switch (argv[i][1]) {
			case 'f':
				if ((i + 1) < argc) {
					set_of_parameters[FILE_FLAG] = extractOneParameter(argc, argv, i + 1, &end_position);
					i = end_position;
					break;
				} else {
					throw ParseCmdLineException("\nAfter -f flag should be pointed a file path.");
				}
			case 'm':
				if ((i + 1) < argc) {
					set_of_parameters[MASK_FLAG] = argv[++i];
					break;
				} else {
					throw ParseCmdLineException("\nAfter -m flag should be pointed bool mask.");
				}
			case 'l':
				if ((i + 1) < argc) {
					set_of_parameters[LNGMSG_FLAG] = argv[++i];
					break;
				} else {
					throw ParseCmdLineException("\nAfter -m flag should be pointed bool mask.");
				}
			case 'h':
				set_of_parameters[HELP_FLAG] = "";
				break;
			default:
				break;
			}
			i++;
		} else {
			break;
		}
	}
return set_of_parameters;
}

// extractOneParameter is necessary for picking up the parameter, which contains space: -f Test 1.tst
std::string extractOneParameter(int argc, char * argv[], int start_position, int * end_position)
{
	int j = 0;
	std::string parameter = "";
	for(j = start_position; j < argc; j++) {
		*end_position = j - 1;
		if('-' == argv[j][0]) {
			break;
		}
	}
	j = start_position;
	for(; j < *end_position; j++) {
		parameter.append(argv[j]);
		parameter.append(" ");
	}
	parameter.append(argv[j]);
	return parameter;
}
