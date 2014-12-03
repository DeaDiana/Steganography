#ifndef _RS_ANALYZER_H_
#define _RS_ANALYZER_H_

#include <string>
#include <exception>

typedef std::string GroupOfPixels;

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
	bool IsStegoImage (const std::string & filename);
	long lenghOfEmeddedMessge (const std::string & filename);
private:

	long countDiscriminant(const GroupOfPixels & group);
	GroupOfPixels flipGruopWithMask(const GroupOfPixels & group, const bool * mask);
	GroupOfPixels flipGruop(const GroupOfPixels & group, const bool mask);
};

/* specification for PGM format
*  http: //netpbm.sourceforge.net/doc/pgm.html */

#endif /*_RS_ANALYZER_H_*/
