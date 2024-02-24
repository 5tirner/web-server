#include "../include/mainHeader.hpp"
#include <cstddef>
#include <stdexcept>

void    connection::processingBody( Request& rs, char* buffer, int& rc )
{
    if ( rs.headers["method"] == "get" )
        throw std::invalid_argument( "OK" );
    if ( rs.headers["method"] == "post" )
    {
        if ( !rs.bodyStream->is_open() )
            generateRandomFileName( rs );
        if ( rs.transferEncoding == "chunked" )
            processChunkedRequestBody( rs, buffer, rc );
        if ( rs.transferEncoding == "content-length" )
			processRegularRequestBody( rs, buffer );
    }
}

void generateRandomFileName( Request& rs )
{
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );
	std::string	filename( "./data/" );

	for ( int i = 0; i < 25; i++ )
		filename.push_back( CHARACTERS[ rand() % CHARACTERS.length() ] );

    rs.bodyStream->open( filename.c_str(), std::ios::out | std::ios::trunc );
}

long    parseChunkHeader( Request& rs, std::string& buffer )
{
	std::string	chunkHead;

	chunkHead = buffer.substr( 0, buffer.find( "\r\n" ) );
	if ( !chunkHead.empty() )
		rs.currentChunkSize = std::strtol( chunkHead.c_str(), NULL, 16 );
	else
		throw std::invalid_argument( "Bad request: invalid chunk size header" );
	if ( rs.currentChunkSize == LONG_MAX || rs.currentChunkSize == LONG_MIN )
		throw std::invalid_argument( "Bad request: invalid chunk size header" );

	buffer = buffer.substr( chunkHead.length() + 2 );
	return ( rs.currentChunkSize );
}

bool    chunkedComplete( Request& rs,  std::string& buffer )
{
	size_t	bufflen ( buffer.length() );

	while ( bufflen != 0 )
	{
		if ( rs.isChunkHeader == true )
		{
			rs.currentChunkSize = parseChunkHeader( rs, buffer );
			// rs.chunkSizeSum += rs.currentChunkSize;
			// if ( rs.chunkSizeSum > rs.maxBodySize )
			// {
			// 	rs.stat = 413;
			// 	throw std::invalid_argument( "client body size > allowed body size" );
			// }
			if ( rs.currentChunkSize == 0 )
				return true;
			bufflen = buffer.length();
		}
		if ( rs.currentChunkSize > static_cast<long>( buffer.length() ) )
		{
			rs.bodyStream->write( buffer.c_str(),  buffer.length() );
			rs.bodyStream->flush();
			if ( !rs.bodyStream->good() )
			{
				rs.stat = 400; // not 400 for sure;
				throw std::runtime_error( "Error: incomplete data trensfer" );
			}
			rs.currentChunkSize -= buffer.length();
			rs.isChunkHeader = false;
			return ( false );
		}
		else if ( rs.currentChunkSize <= static_cast<long>( buffer.length() ) ) // add equal check it again
		{
			rs.bodyStream->write( buffer.c_str(),  rs.currentChunkSize );
			rs.bodyStream->flush();
			if ( !rs.bodyStream->good() )
			{
				rs.stat = 400; // not 400 for sure;
				throw std::runtime_error( "Error: incomplete data trensfer" );
			}
			bufflen -= rs.currentChunkSize + 2;
			buffer = buffer.substr( rs.currentChunkSize + 2 );
			rs.isChunkHeader = true;
		}
	}
	return true;
}

void    processChunkedRequestBody( Request& rs, char* buffer, int& rc )
{
    if ( !rs.remainingBody.empty() )
    {
        if ( chunkedComplete( rs, rs.remainingBody ) )
            throw std::invalid_argument( "CHUNK LI DAZ 3LA WJEH HEADER WAS PROCESSED" );
        rs.remainingBody.clear();
    }
    else
    {
        std::string receivedData( buffer, rc );
        if ( chunkedComplete( rs, receivedData ) )
            throw std::invalid_argument( "SARF LI KATSAL HANTA KHDITIH" );
    }
}

size_t	ft_strlen( const char* str )
{
	size_t i ( 0 );

	for ( ; str[ i ]; i++ );
	return ( i );
}

void	processRegularRequestBody( Request& rs, char* buffer )
{
	size_t	bufferSize = ft_strlen( buffer );

	if ( !rs.remainingBody.empty() )
	{
		rs.bodyStream->write( rs.remainingBody.c_str(),  rs.remainingBody.length() );
		rs.bodyStream->flush();
		rs.content_length += rs.remainingBody.length();
		rs.remainingBody.clear();
	}
	else if ( bufferSize ){
		rs.bodyStream->write( buffer,  bufferSize );
		rs.bodyStream->flush();
		rs.content_length += bufferSize;
	}
	if ( !rs.bodyStream->good() )
	{
		rs.stat = 400; // not 400 for sure;
		throw std::runtime_error( "Error: incomplete data trensfer" );
	}
	std::cout << "rs.content_length: " << rs.content_length << " | " << "rs.requestBodyLength: " << rs.requestBodyLength << std::endl;
	if ( rs.content_length == rs.requestBodyLength )
	{
		rs.stat = 200;
		throw std::runtime_error( "SARF LI KATSAL HANTA KHDITIH" );
	}
}