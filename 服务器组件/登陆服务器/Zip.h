//
// classes CZip and CUnzip
// usage :
//    * construct a CZip/CUnzip object with an unzipped/zipped file name
//    * use SwapSize on that object to build a zipp/unzipped file 
//
// SwapSize raises CZipException. The GetString method retrieve the
// message bound to the exception.
//

#ifndef _CZIP_H_
#define _CZIP_H_

#include <fcntl.h>
#include <io.h>
#include <stdlib.h>
#include <windows.h>
#include <sys\stat.h>

#include <stdio.h>


#ifndef DLL_INTERNAL
#define DLL_INTERNAL __declspec( dllimport )
#endif 

class DLL_INTERNAL CZipException
{
public:
	CZipException( const char*fm, ... );
	const char* GetString() { return m_sMessage; }
protected:
	char m_sMessage[ 128 ];
};


// pure virtual
class DLL_INTERNAL CMamaZip
{
public:
	CMamaZip( const char *sSourceFileName );
	void SwapSize( const char *sTargetFileName );
protected:
	virtual int HardWork( int inFile, int outFile ) = 0;

	char m_sSourceFileName[ MAX_PATH ];
	char *m_sOperation;
};


class DLL_INTERNAL CZip : public CMamaZip
{
public:
	CZip( const char *sSourceFileName );
protected:
	virtual int HardWork( int inFile, int outFile );
};


class DLL_INTERNAL CUnzip : public CMamaZip
{
public:
	CUnzip( const char *sSourceFileName ) ;
protected:
	virtual int HardWork( int inFile, int outFile );
};


#endif // _CZIP_H_