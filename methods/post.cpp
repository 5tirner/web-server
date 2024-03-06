#include "../include/mainHeader.hpp"
#include <iostream>
#include <unistd.h>

void    connection::processingBody( Request& rs, char* buffer, int& rc, const informations& infoStruct )
{
    if ( rs.headers["method"] == "get" )
        throw std::invalid_argument( "OK" );
    if ( rs.headers["method"] == "post" )
    {
		if ( location_support_upload( rs, infoStruct ) == -1 )
			throw std::runtime_error( "You don't have the right to upload at this location" );
        if ( !rs.bodyStream->is_open() )
            generateRandomFileName( rs );
        if ( rs.transferEncoding == "chunked" )
            processChunkedRequestBody( rs, buffer, rc );
        if ( rs.transferEncoding == "content-length" )
			processRegularRequestBody( rs, buffer );
    }
}

int location_support_upload( Request& rs,  const informations& infoStruct )
{	
	std::string upload;
	std::string method;

	for ( size_t i = 0; i < infoStruct.locationsInfo.size(); i++ )
	{
		upload   = infoStruct.locationsInfo.at( i ).upload.at( "upload" );
		method	 = infoStruct.locationsInfo.at( i ).allowed_methodes.at( "allowed_methodes" );

		if ( upload[0] )
		{
			if ( method.find( "POST" ) != std::string::npos )
			{
				if ( access( upload.c_str(), F_OK | W_OK ) != 0 )
					return ( rs.stat = 403, -1 );
				return ( rs.stat = 201, 0 );
			}
		}
	}
	return ( rs.stat = 405, -1 );
}

void generateRandomFileName( Request& rs )
{
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );
	std::string	filename( "./upload/" );

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
	OUT( buffer );
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
			// if ( buffer.length() < 2047 )
			// {

			// }
			// std::cout << "current chunk size: " << rs.currentChunkSize << " bufflen:" << bufflen  << " Buffer length: " << buffer.length() << std::endl;
			rs.bodyStream->write( buffer.c_str(),  buffer.length() );
			// if ( !rs.bodyStream->good() )
			// {
			// 	rs.stat = 400; // not 400 for sure;
			// 	throw std::runtime_error( "Error: incomplete data trensfer" );
			// }
			rs.bodyStream->flush();
			rs.currentChunkSize -= buffer.length();
			rs.isChunkHeader = false;
			// std::cout << "1current chunk size: " << rs.currentChunkSize << " 1bufflen:" << bufflen  << " 1Buffer length: " << buffer.length() << std::endl;
			return ( false );
		}
		if ( rs.currentChunkSize <= static_cast<long>( buffer.length() ) ) // add equal check it again
		{
			rs.bodyStream->write( buffer.c_str(),  rs.currentChunkSize );
			// if ( !rs.bodyStream->good() )
			// {
			// 	rs.stat = 400; // not 400 for sure;
			// 	throw std::runtime_error( "Error: incomplete data trensfer" );
			// }
			// std::cout << "1current chunk size: " << rs.currentChunkSize << " 1bufflen:" << bufflen  << " 1Buffer length: " << buffer.length() << std::endl;
			rs.bodyStream->flush();
			bufflen -= rs.currentChunkSize + 2;
			buffer = buffer.substr( rs.currentChunkSize + 2 );
			rs.isChunkHeader = true;
			// std::cout << "NOK" << std::endl;
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
            throw std::invalid_argument( "CHUNK LI KATSAL HANTA KHDITIH" );
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
	// std::cout << "rs.content_length: " << rs.content_length << " | " << "rs.requestBodyLength: " << rs.requestBodyLength << std::endl;
	if ( rs.content_length == rs.requestBodyLength )
	{
		rs.stat = 201;
		throw std::runtime_error( "SARF LI KATSAL HANTA KHDITIH" );
	}
}