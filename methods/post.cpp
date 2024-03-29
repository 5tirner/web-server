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

	try {
		rq.filename = path;
		const std::string CHARACTErq = "ABCDEFGHIJKLMNOPQrqTUVWXYZabcdefghijklmnopqrqtuvwxyz";
		std::srand( std::time( NULL ) );

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


int connection::location_support_upload( Request& rq, int serverID )
{	
	try
	{
		size_t i = 0;
		std::string	location;
		informations serverInfo;
		std::map<std::string, informations>::iterator it = notBindingServers.find( rq.headers.at("host") );
		if ( it != notBindingServers.end() )
			serverInfo = it->second;
		else
			serverInfo = OverLoad.at( serverID );

		std::string newUri( getUrl( rq.headers.at("uri") ) );

		for (; i < serverInfo.locationsInfo.size(); i++ )
		{
			location = serverInfo.locationsInfo.at(i).directory.at( "location" );
			std::cout << "location: " << location << std::endl;
			if ( location.length() > 1 && location.at( location.length() - 1 ) == '/' )
				location.resize( location.length() - 1 );
			if ( newUri == location )
				break ;
		}
		std::cout << "NEW URI: " << newUri << " location: " << location << std::endl;
		if ( newUri == location )
		{
			if ( serverInfo.locationsInfo.at( i ).cgi.at("cgi") != "" )
				rq.cgi = true;
			std::string upload   = serverInfo.locationsInfo.at( i ).upload.at( "upload" );
			std::string method	 = serverInfo.locationsInfo.at( i ).allowed_methodes.at( "allowed_methodes" );
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
						Logger::log() << "[ Error ] : Client can't upload in this location perimssion"<< std::endl;
						return ( rq.stat = 403, -1 );
					}
					rq.limitClientBodySize = std::atol( serverInfo.limitClientBody.at("limit_client_body").c_str() ) * 100000000; //! need the exact amount
					if ( rq.limitClientBodySize == 0 )
					{
						Logger::log() << "[ Error ] : Client body size limit is 0" << std::endl;
						return ( rq.stat = 400, -1 );
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
		Logger::log() << "[ Error ] : Client can't upload in this location " << std::endl;
		return ( rq.stat = 403, -1 );
	}
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
				Logger::log() << "[ Error ] Client bosy size greater than body size limit" << std::endl;
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

static void	processChunkedRequestBody( Request& rq, char* buffer, int& rc, bool& sendRes)
{
    if ( !rq.remainingBody.empty() )
    {
        if ( chunkedComplete( rq, rq.remainingBody ) )
        {
			rq.stat = 201;
			sendRes = true;
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
			sendRes = true;
			Logger::log() << "[ sucess ] body file created" << std::endl;
			throw std::exception();
		}
    }
}

static void	processRegularRequestBody( Request& rq, char* buffer, int& rc, bool& sendRes )
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
	if ( rq.content_length == rq.requestBodyLength )
	{
		rq.stat = 201;
		sendRes = true;
		Logger::log() << "[ sucess ] body file created" << std::endl;
		return;
	}
	else if ( rq.content_length > rq.requestBodyLength )
	{
		rq.stat = 413;
		Logger::log() << "[ Error ] Request Entity Too Large" << std::endl;
		throw std::exception();
	}
}

void	connection::processingBody( Request& rq, char* buffer, int rc, int serverID )
{
	if ( rq.headers.at( "method" ) == "get" || rq.headers.at( "method" ) == "delete" )
		rq.readyToSendRes = true;
	else if ( rq.headers.at( "method" ) == "post" )
    {
		if ( rq.locationGotChecked == false && location_support_upload( rq, serverID ) == -1 )
			throw std::exception();
		if ( rq.transferEncoding == true )
			processChunkedRequestBody( rq, buffer, rc, rq.readyToSendRes );
		if ( rq.contentLength == true )
		{
			if ( rq.contentLength <= rq.limitClientBodySize )		
				processRegularRequestBody( rq, buffer , rc, rq.readyToSendRes );
			else
			{
				rq.stat = 413;
				Logger::log() << "[ Error ] Request Entity Too Large" << std::endl;
				throw std::exception();
			}
		}
    }
}
