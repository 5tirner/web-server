#include "../include/mainHeader.hpp"
#include <exception>
#include <stdexcept>
/*-------------- yachaab code ---------------*/
void    connection::fetchRequestHeader( Request& rs, char* buffer )
{
    rs.fullRequest.append( buffer );
    if ( rs.fullRequest.find("\r\n\r\n") != std::string::npos )
    {
        rs.fetchHeaderDone = true;
        rs.remainingBody = rs.fullRequest.substr( rs.fullRequest.find( "\r\n\r\n" ) + 4 );
    }
}

int connection::processingHeader( Request& rs )
{
    if ( extractMethodAndUri( rs ) == -1 )
        throw std::runtime_error( codeMsg.statMsg[rs.stat] );
    return ( 0 );
}

int extractMethodAndUri( Request& rs )
{
	std::string startLine;
    size_t      carriagepos;
	try
	{
		startLine = rs.fullRequest.substr( 0, rs.fullRequest.find( "\n" ) );
        carriagepos = startLine.find("\r");
        startLine.resize(carriagepos);
		rs.headers["mathod"]    =	startLine.substr( 0, startLine.find( ' ' )  );
		rs.headers["uri"]       =	startLine.substr( rs.headers["mathod"].length() + 1, startLine.find_last_of( ' ' ) -  rs.headers["mathod"].length() - 1 );
		rs.headers["version"]   =	startLine.substr( rs.headers["uri"].length() + rs.headers["uri"].length() + 2 );
	}catch( const std::exception& e )
	{
		rs.stat = 400;
		return ( -1 );
	}
	return ( 0 );
}
/*-------------- yachaab code ---------------*/