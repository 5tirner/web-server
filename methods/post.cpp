#include "../include/mainHeader.hpp"

static std::string& getUrl( std::string& uri )
{
	size_t slash = 0;

	for ( size_t i =  1; i < uri.length(); i++ )
	{
		if ( uri.at( i ) == '/' )
			slash = i;
	}
	if ( slash > 0 )
		uri.resize( slash );
	return uri;
}

static void generateRandomFileName( Request& rq, std::string& path )
{
	rq.filename = path;
	const std::string CHARACTErq = "ABCDEFGHIJKLMNOPQrqTUVWXYZabcdefghijklmnopqrqtuvwxyz";
	std::srand( std::time( NULL ) );

	try {
		if ( rq.filename.length() > 1 && rq.filename.at( rq.filename.length() - 1 ) != '/' )
			rq.filename += "/";
		for ( int i = 0; i < 25; i++ )
			rq.filename.push_back( CHARACTErq[ rand() % CHARACTErq.length() ] );
		rq.filename += rq.extension;
    	rq.bodyStream->open( rq.filename.c_str(), std::ios::binary | std::ios::trunc );
		if ( !rq.bodyStream->is_open() )
		{
			Logger::log() << "[ Error ] : couldn't open file to store request body" << std::endl;
			throw std::exception();
		}
	} catch (...) {
		rq.stat = 500;
		throw std::exception();
	}
}


static int location_support_upload( Request& rq, const informations& infoStruct )
{	
	std::string upload;
	std::string method;
	std::string	location;
	std::string newUri;
	try
	{
		size_t i	= 0;
		newUri		= getUrl( rq.headers.at("uri") );
		for (; i < infoStruct.locationsInfo.size(); i++ )
		{
			location = infoStruct.locationsInfo.at(i).directory.at( "location" );
			if ( location.length() > 1 && location.at( location.length() - 1 ) == '/' )
				location.resize( location.length() - 1 );
			if ( newUri == location )
				break ;
		}
		if ( newUri == location )
		{
			if ( infoStruct.locationsInfo.at( i ).cgi.at("cgi") != "" )
			{
				rq.cgi = true;
			}
			upload   = infoStruct.locationsInfo.at( i ).upload.at( "upload" );
			method	 = infoStruct.locationsInfo.at( i ).allowed_methodes.at( "allowed_methodes" );
			if ( upload[0] )
			{
				if ( method.find( "POST" ) != std::string::npos )
				{
					if ( access( upload.c_str(), F_OK | W_OK | X_OK ) == 0 )
					{
						DIR* directory = opendir( upload.c_str() );
						if ( directory != NULL )
							closedir( directory );
						else
						{
							Logger::log() << "[ Error ] : Client can't upload in this location directory not found" << "\'" << upload << "\'" << std::endl;
							return ( rq.stat = 403, -1 );
						}
					}
					else
					{
						Logger::log() << "[ Error ] : Client can't upload in this location " << "\'" << upload << "\'" << std::endl;
						return ( rq.stat = 403, -1 );
					}
					long holder = std::strtol( infoStruct.limitClientBody.at("limit_client_body").c_str(), NULL, 10 ) * 1000000;
					if ( holder == 0 || holder == LONG_MAX || holder == LONG_MIN )
					{
						Logger::log() << "[ Error ] : Client limit body size incorrect" << std::endl;
						return ( rq.stat = 400, -1 );
					}
					rq.limitClientBodySize = holder;
					if ( rq.content_length > rq.limitClientBodySize )
					{
						Logger::log() << "[ Error ] : Client limit body size smaller that body content length" << std::endl;
						return ( rq.stat = 413, -1 );
					}
					if ( !rq.bodyStream->is_open() )
						generateRandomFileName( rq, upload );
					rq.locationGotChecked = true;
					return ( rq.stat = 201, 0 );
				}
			}
		}	
	}
	catch(const std::exception& e)
	{
		Logger::log() << "[ Error ] : Client can't upload in this location " << "\'" << upload << "\'" << std::endl;
		return ( rq.stat = 403, -1 );
	}
	Logger::log() << "[ Error ] : upload location was not provided at config file "<< std::endl;
	return ( rq.stat = 403, -1 );
}

static size_t	parseChunkHeader( Request& rq, std::string& buffer )
{
	std::string	chunkHead;
	long		size;
	size_t 		crlfPos = 0;

	if ( rq.iscr == true )
	{
		buffer = buffer.substr( 2 );
		rq.iscr = false;
	}
	else if ( rq.islf == true )
	{
		buffer = buffer.substr( 1 );
		rq.islf = false;
	}

	crlfPos = buffer.find( "\r\n" );
	
	if ( crlfPos == std::string::npos )
		return std::string::npos;
	chunkHead = buffer.substr( 0, crlfPos );
	if ( !chunkHead.empty() )
		size = std::strtol( chunkHead.c_str(), NULL, 16 );
	else
	{
		rq.stat = 400;
		Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
		throw std::exception();
	}
	if ( size == LONG_MAX || size == LONG_MIN )
	{
		rq.stat = 400;
		Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
		throw std::exception();
	}
	else
		rq.currentChunkSize = size;
	buffer = buffer.substr( chunkHead.length() + 2 );
	return ( rq.currentChunkSize );
}

static bool    chunkedComplete( Request& rq,  std::string& buffer )
{
	size_t	bufflen ( buffer.length() );
	while ( bufflen != 0 )
	{
		if ( rq.isChunkHeader == true )
		{
			rq.currentChunkSize = parseChunkHeader( rq, buffer );
			if ( rq.currentChunkSize == std::string::npos )
				return false;
			rq.chunkSizeSum += rq.currentChunkSize;
			if ( rq.chunkSizeSum > rq.limitClientBodySize )
			{
				rq.stat = 413;
				Logger::log() << "[ Error ] Client body size greater than body size limit" << std::endl;
				throw std::exception();
			}
			if ( rq.currentChunkSize == 0 )
				return true;
			bufflen = buffer.length();
		}
		if ( rq.currentChunkSize > buffer.length() )
		{
			rq.bodyStream->write( buffer.c_str(),  buffer.length() );
			if ( !rq.bodyStream->good() )
			{
				rq.stat = 500;
				Logger::log() << "[ Error ] write body stream failed" << std::endl;
				throw std::exception();
			}
			rq.bodyStream->flush();
			rq.currentChunkSize -= buffer.length();
			rq.isChunkHeader = false;
			return ( false );
		}
		else if ( rq.currentChunkSize < buffer.length() )
		{
			rq.bodyStream->write( buffer.c_str(), rq.currentChunkSize );
			if ( !rq.bodyStream->good() )
			{
				rq.stat = 500;
				Logger::log() << "[ Error ] write body stream failed" << std::endl;
				throw std::exception();
			}
			rq.bodyStream->flush();
			bufflen -= rq.currentChunkSize;
			if ( bufflen > 1 )
			{
				buffer = buffer.substr( rq.currentChunkSize + 2 );
				bufflen -= 2;
			}
			else
			{
				buffer = buffer.substr( rq.currentChunkSize + 1 );
				rq.islf = true;
				bufflen -= 1;
			}
			rq.isChunkHeader = true;
		}
		else if ( rq.currentChunkSize == buffer.length() )
		{
			rq.bodyStream->write( buffer.c_str(), rq.currentChunkSize );
			rq.bodyStream->flush();
			bufflen -= rq.currentChunkSize;
			rq.iscr = true;
			rq.isChunkHeader = true;
			return false;
		}
	}
	return false;
}

static void	processChunkedRequestBody( Request& rq, char* buffer, int& rc )
{
    if ( !rq.remainingBody.empty() )
    {
        if ( chunkedComplete( rq, rq.remainingBody ) )
        {
			rq.stat = 201;
			rq.readyToSendRes = true;
			Logger::log() << "[ sucess ] body file created" << std::endl;
			throw std::exception();
		}
        rq.remainingBody.clear();
    }
    else
    {
        std::string receivedData( buffer, rc );
        if ( chunkedComplete( rq, receivedData ) )
        {
			rq.stat = 201;
			rq.readyToSendRes = true;
			Logger::log() << "[ sucess ] body file created" << std::endl;
			throw std::exception();
		}
    }
}

static void	processRegularRequestBody( Request& rq, char* buffer, int& rc )
{
	if ( !rq.remainingBody.empty() )
	{
		rq.bodyStream->write( rq.remainingBody.c_str(),  rq.remainingBody.length() );
		rq.bodyStream->flush();
		rq.content_length += rq.remainingBody.length();
		rq.remainingBody.clear();
	}
	else if ( rc ){
		rq.bodyStream->write( buffer,  rc );
		rq.bodyStream->flush();
		rq.content_length += rc;
	}
	if ( !rq.bodyStream->good() )
	{
		rq.stat = 500;
		Logger::log() << "[ Error ] write body stream failed" << std::endl;
		throw std::exception();
	}
	if ( rq.content_length == rq.limitClientBodySize )
	{
		rq.stat = 201;
		rq.readyToSendRes = true;
		rq.bodyStored = true;
		Logger::log() << "[ sucess ] body file created" << std::endl;
		return;
	}
}

static void storeBodyForCgi( Request& rq, int rc, char *buffer )
{
	if ( !rq.remainingBody.empty() )
	{
		rq.bodyStream->write( rq.remainingBody.c_str(),  rq.remainingBody.length() );
		rq.bodyStream->flush();
		rq.content_length += rq.remainingBody.length();
		rq.remainingBody.clear();
	}
	else if ( rc )
	{
		rq.bodyStream->write( buffer,  rc );
		rq.bodyStream->flush();
		rq.content_length += rc;
	}
	if ( !rq.bodyStream->good() )
	{
		rq.stat = 500;
		Logger::log() << "[ Error ] write body stream failed" << std::endl;
		throw std::exception();
	}
	if ( rq.transferEncoding == true )
		std::cout << "DO IT" << std::endl;
	else if ( rq.contentLength == true )
		processRegularRequestBody( rq, buffer , rc );
}


void	processingBody( Request& rq, char* buffer, int rc, const informations& infoStruct )
{
	if ( rq.headers.at( "method" ) == "get" || rq.headers.at( "method" ) == "delete" )
		rq.readyToSendRes = true;
	else if ( rq.headers.at( "method" ) == "post" )
    {
		if ( rq.locationGotChecked == false && location_support_upload( rq, infoStruct ) == -1 )
			throw std::exception();
		if ( rq.cgi == true )
		{
			std::cout << "CGI IS ON" << std::endl;
			storeBodyForCgi( rq, rc, buffer );
			if ( rq.bodyStored )
			{
				std::cout << "cgi ready to be executed" << std::endl;
				throw std::exception();
			}
			throw std::exception();
		}
		else if ( rq.transferEncoding == true )
			processChunkedRequestBody( rq, buffer, rc );
		else if ( rq.contentLength == true ) // ! make a change here !
			processRegularRequestBody( rq, buffer , rc );
    }
}
