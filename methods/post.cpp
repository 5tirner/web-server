#include "../include/mainHeader.hpp"
#include <exception>
#include <stdexcept>

void    connection::processingBody( Request& rs, char* buffer, int rc, const informations& infoStruct )
{
	if ( rs.headers.at( "method" ) == "get" || rs.headers.at( "method" ) == "delete" )
	{
		rs.readyToSendRes = true;
		// startClient = true;
	}
	else if ( rs.headers["method"] == "post" )
    {
		if ( location_support_upload( rs, infoStruct ) == -1 )
			throw std::invalid_argument("location_not_support_upload");
		if ( !rs.bodyStream->is_open() )
			generateRandomFileName( rs );
		if ( rs.transferEncoding == true )
			processChunkedRequestBody( rs, buffer, rc, rs.readyToSendRes );
		if ( rs.contentLength == true )
		{
			if ( rs.contentLength <= rs.limitClientBodySize )
				processRegularRequestBody( rs, buffer , rc, rs.readyToSendRes );
			else
			{
				rs.stat = 413;
				throw std::exception();
			}
		}
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
				rs.limitClientBodySize = std::atol( infoStruct.limitClientBody.at("limit_client_body").c_str() ) * 100000000;
				if ( rs.limitClientBodySize == 0 )
					return ( rs.stat = 400, -1 );
				return ( rs.stat = 201, 0 );
			}
		}
	}
	return ( rs.stat = 403, -1 );
}

void generateRandomFileName( Request& rs )
{
	const std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::srand( std::time( NULL ) );
	std::string	filename( "./upload/" );

	try {
		for ( int i = 0; i < 25; i++ )
		filename.push_back( CHARACTERS[ rand() % CHARACTERS.length() ] );
    	rs.bodyStream->open( filename.c_str(), std::ios::out | std::ios::trunc );
	} catch (...) {
		rs.stat = 500;
		throw std::invalid_argument("generateRandomFileName fail");
	}
}

long    parseChunkHeader( Request& rs, std::string& buffer )
{
	std::string	chunkHead;

	chunkHead = buffer.substr( 0, buffer.find( "\r\n" ) );
	if ( !chunkHead.empty() )
		rs.currentChunkSize = std::strtol( chunkHead.c_str(), NULL, 16 );
	else
	{
		rs.stat = 400;
		throw std::invalid_argument( "Bad request: invalid chunk size header" );
	}
	if ( rs.currentChunkSize == 9223372036854775807 )
	{
		rs.stat = 400;
		throw std::invalid_argument( "Bad request: invalid chunk size header" );
	}
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
			rs.chunkSizeSum += rs.currentChunkSize;
			if ( rs.chunkSizeSum > rs.limitClientBodySize )
			{
				rs.stat = 413;
				throw std::invalid_argument( "client body size > allowed body size" );
			}
			if ( rs.currentChunkSize == 0 )
				return true;
			bufflen = buffer.length();
		}
		if ( rs.currentChunkSize > buffer.length() )
		{
			rs.bodyStream->write( buffer.c_str(),  buffer.length() );
			// if ( !rs.bodyStream->good() )
			// {
			// 	rs.stat = 400; // not 400 for sure;
			// 	throw std::exception();
			// }
			rs.bodyStream->flush();
			rs.currentChunkSize -= buffer.length();
			rs.isChunkHeader = false;
			return ( false );
		}
		else if ( rs.currentChunkSize < buffer.length() ) // add equal check it again
		{
			rs.bodyStream->write( buffer.c_str(), rs.currentChunkSize );
			// if ( !rs.bodyStream->good() )
			// {
			// 	rs.stat = 400; // not 400 for sure;
			// 	throw std::exception();
			// }
			rs.bodyStream->flush();
			bufflen -= rs.currentChunkSize + 2;
			buffer = buffer.substr( rs.currentChunkSize + 2 );
			rs.isChunkHeader = true;
		}
		else if ( rs.currentChunkSize == buffer.length() )
		{
			std::cout << "EQUAL" << std::endl;
			rs.bodyStream->write( buffer.c_str(), rs.currentChunkSize );
			rs.bodyStream->flush();
			bufflen -= rs.currentChunkSize;
			return false;
		}
	}
	return false;
}

void    processChunkedRequestBody( Request& rs, char* buffer, int& rc, bool& sendRes)
{
    if ( !rs.remainingBody.empty() )
    {
		// OUT( "POSTING THE FIST BODY" );
        if ( chunkedComplete( rs, rs.remainingBody ) )
        {
			rs.stat = 201;
			sendRes = true;
			throw std::exception();
		}
        rs.remainingBody.clear();
    }
    else
    {
        std::string receivedData( buffer, rc );
        if ( chunkedComplete( rs, receivedData ) )
        {
			rs.stat = 201;
			sendRes = true;
			throw std::exception();
		}
    }
}

void	processRegularRequestBody( Request& rs, char* buffer, int& rc, bool& sendRes )
{
	if ( !rs.remainingBody.empty() )
	{
		rs.bodyStream->write( rs.remainingBody.c_str(),  rs.remainingBody.length() );
		rs.bodyStream->flush();
		rs.content_length += rs.remainingBody.length();
		rs.remainingBody.clear();
	}
	else if ( rc ){
		rs.bodyStream->write( buffer,  rc );
		rs.bodyStream->flush();
		rs.content_length += rc;
	}
	if ( !rs.bodyStream->good() )
	{
		rs.stat = 400; // not 400 for sure;
		throw std::exception();
	}
	if ( rs.content_length == rs.requestBodyLength )
	{
		rs.stat = 201;
		sendRes = true;
		return;
	}
	else if ( rs.content_length > rs.requestBodyLength )
	{
		rs.stat = 413;
		throw std::exception();
	}
}
