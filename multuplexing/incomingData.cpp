#include "../include/mainHeader.hpp"
#include <exception>
#include <stdexcept>
/*-------------- yachaab code start ---------------*/
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
	if ( validateUriAndExtractQueries( rs ) == -1 )
		throw std::runtime_error( codeMsg.statMsg[rs.stat] );
	if ( extractHttpHeaders( rs ) )
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

int	validateUri( const std::string& uri )
{
    if ( uri.empty() )
        return ( -1 );
    if ( uri.front() != '/' )
        return ( -1 );
    if ( uri.find(' ') != std::string::npos )
        return ( -1 );
	if ( uri.length() > 2048 )
		return ( -1 );
	return ( 0 );
}

void	decomposeQueryParameters( const std::string& query, Request& rs )
{
    std::string param;
    std::stringstream ss( query );

    while ( std::getline(ss, param, '&') )
	{
        if ( param.empty() ) continue;
        size_t equalPos		=	param.find( '=' );
        std::string key		=	param.substr( 0, equalPos );
        std::string value	=	equalPos != std::string::npos ? param.substr( equalPos + 1 ) : "";
        rs.queries[key] = value;
    }
}

int validateUriAndExtractQueries( Request& rs )
{
	if ( validateUri( rs.headers["uri"] ) == -1 )
	{
		rs.stat = 400;
		return ( -1 );
	}
	size_t queryPos = rs.headers["uri"].find('?');
    if ( queryPos != std::string::npos )
	{
        std::string query = rs.headers["uri"].substr( queryPos + 1 );
        rs.headers["uri"] = rs.headers["uri"].substr( 0, queryPos );
        decomposeQueryParameters( query, rs );
    }
	return ( 0 );
}

int	extractHttpHeaders( Request& rs )
{
	std::string									line, first, second;
	std::vector<std::string>					header_lines;
	std::vector<std::string>::const_iterator	it;
	std::stringstream							headerStream ( rs.fullRequest );

	try
	{
		std::getline( headerStream, line );
		while ( std::getline( headerStream, line ) && line != "\r" )
		{
			size_t carriagepos = line.find("\r");
        	line.resize(carriagepos);

			first	=	it->substr( 0, it->find_first_of( ':' ) );
			second	=	it->substr( it->find_first_of( ':' ) + 2 );
			
			if ( !examinHeaders( first, second ) )
				throw std::invalid_argument( "bad request: Invalid header" );

			rs.headers[ first ] = second;
		}

		for ( it = header_lines.begin(); it != header_lines.end(); ++it )
		{
			first	=	upcase( it->substr( 0, it->find_first_of( ':' ) ) );
			second	=	upcase( it->substr( it->find_first_of( ':' ) + 2 ) );
			
			if ( !examinHeaders( first, second ) )
				throw std::invalid_argument( "bad request: Invalid header" );

			_e_.headers[ first ] = second;
		}

		_e_.initialProcessingDone = FALSE;
	}
	catch( const std::exception& e )
	{
		return ( -1 );
	}
	return ( 0 );
}

/*-------------- yachaab code end ---------------*/