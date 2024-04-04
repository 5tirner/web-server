#include "../include/mainHeader.hpp"

/*-------------- yachaab code start ---------------*/

static void lowcase( std::string& str )
{
	for ( size_t i = 0; i < str.length(); i++  )
	{
		if ( str[ i ] >= 65 && str[ i ] <= 90 )
			str[ i ] += 32;
	}
}

static bool	extractMethodAndUri( Request& rq )
{
	std::string startLine;
	size_t		lfp ( rq.fullRequest.find( "\n" ) ) ;
	try
	{
		if ( lfp != std::string::npos )
			startLine = rq.fullRequest.substr( 0, lfp );
		else
			throw std::exception();

		if ( startLine.at( startLine.length() - 1 ) == '\r' )
        	startLine.resize( startLine.length() - 1 );

		int spNbr = std::count( startLine.begin(), startLine.end(), ' ' );

		if ( spNbr != 2 )
			throw std::exception();

		rq.headers["method"]    =	startLine.substr( 0, startLine.find( ' ' )  );
		rq.headers["uri"]       =	startLine.substr( rq.headers.at("method").length() + 1, startLine.find_last_of( ' ' ) -  rq.headers.at("method").length() - 1 );
		rq.headers["version"]   =	startLine.substr( rq.headers.at("method").length() + rq.headers.at("uri").length() + 2 );
		
		lowcase( rq.headers.at("method") );
		lowcase( rq.headers.at("version") );

		if ( rq.headers.at("version") != "http/1.1" )
			throw std::exception();
		if ( rq.headers.at("method") != "get" && rq.headers.at("method") != "post" && rq.headers.at("method") != "delete" )
			throw std::exception();
	}
	catch( ... )
	{
		Logger::log() << "[ Error ] extract Method And Uri failed" << std::endl;
		return ( rq.stat = 400, false );
	}
	return ( true );
}

static bool	validateUri( const std::string& uri )
{
    if ( uri.empty() )
		return ( false );
    if ( uri.at( 0 ) != '/' )
		return ( false );
    if ( uri.find(' ') != std::string::npos )
		return ( false );
	if ( uri.length() > 2048 )
		return ( false );
	return ( true );
}



std::string decodeURI(const std::string& uri)
{
    std::string result;
    for (std::size_t i = 0; i < uri.size(); ++i)
    {
		// if ( uri[i] == '/' )
		// {
		// 	size_t j = i + 1;
		// 	for ( ; j <= i + 2 && uri[j] == '.'; ++j );
		// 	if ( j > i + 1 && j == uri.size() )
		// 	{
		// 		std::cerr << "????????????????????????" << std::endl;
		// 		return "";
		// 	}
		// 	if ( j != i + 1 && uri[ j ] == '/' )
		// 	{
		// 		std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
		// 		return "";
		// 	}
		// }
        if (uri[i] == '%' && i + 2 < uri.length())
        {
            std::string hex = uri.substr(i + 1, 2);
            std::stringstream ss;
            int ch;
            ss << std::hex << hex;
            ss >> ch;
            result += static_cast<char>(ch);
            i += 2;
        }
        else
            result += uri[i];
    }
    return result;
}

static bool validateUriAndExtractQueries( Request& rq )
{
	if ( validateUri( rq.headers.at("uri") ) == false )
	{
		if ( rq.headers.at("uri").length() > 2048 )
		{
			return ( rq.stat = 414, false );
			Logger::log() << "[ Error ] validate Uri And Extract Queries: Request-URI Too Long" << std::endl;
		}
		Logger::log() << "[ Error ] validate Uri And Extract Queries: validate uri: failed" << std::endl;
		return ( rq.stat = 400, false );
	}

	rq.headers.at("uri") = decodeURI(rq.headers.at("uri"));

	if ( rq.headers.at("uri").empty() )
		return ( rq.stat = 400, false );
		
	size_t queryPos = rq.headers.at("uri").find('?');
    if ( queryPos != std::string::npos )
	{
        rq.cgiInfo.queries = rq.headers.at("uri").substr( queryPos + 1 );
        rq.headers.at("uri") = rq.headers.at("uri").substr( 0, queryPos );
    }
	return ( true );
}

static int whiteSpace( char ch )
{
	if ( ch == '\t' || ch == ' ' || ch == '\r' || ch == '\f' || ch == '\n' )
		return 1;
	return 0;
}

static void strTrim( std::string& str )
{
	size_t i = 0, j = str.length() - 1;

	for ( ; i < str.length() && whiteSpace( str[i] ); i++ );
	for ( ; j > 0 && whiteSpace( str[j] ); j-- );
	str = str.substr( i, (j - i) + 1 );
}

std::string getMimeTypeForPost(std::string& type)
{
    std::map<std::string, std::string> mimeTypes;
	mimeTypes["text/css"] 					= ".css";
	mimeTypes["font/ttf"] 					= ".ttf";
	mimeTypes["font/otf"] 					= ".otf";
	mimeTypes["text/html"]					= ".html";
	mimeTypes["image/png"] 					= ".png";
	mimeTypes["video/mp4"] 					= ".mp4";
	mimeTypes["video/ogg"] 					= ".ogv";
	mimeTypes["audio/ogg"] 					= ".ogg";
	mimeTypes["font/woff"] 					= ".woff";
	mimeTypes["audio/wav"] 					= ".wav";
	mimeTypes["image/gif"] 					= ".gif";
	mimeTypes["text/plain"] 				= ".txt";
	mimeTypes["image/jpeg"] 				= ".jpg";
	mimeTypes["image/jpeg"] 				= ".jpeg";
	mimeTypes["audio/mpeg"] 				= ".mp3";
	mimeTypes["video/webm"] 				= ".webm";
	mimeTypes["audio/webm"] 				= ".webm";
	mimeTypes["font/woff2"] 				= ".woff2";
	mimeTypes["message/http"] 				= ".http";
	mimeTypes["image/svg+xml"] 				= ".svg";
	mimeTypes["text/x-python"] 				= ".py";
	mimeTypes["text/javascript"] 			= ".js";
	mimeTypes["application/xml"] 			= ".xml";
	mimeTypes["application/pdf"] 			= ".pdf";
	mimeTypes["application/zip"] 			= ".zip";
	mimeTypes["application/json"] 			= ".json";
	mimeTypes["application/gzip"] 			= ".gz";
	mimeTypes["multipart/form-data"] 		= ".multipart";
	mimeTypes["application/x-httpd-php"] 	= ".php";
	mimeTypes["application/octet-stream"] 	= ".bin";
	mimeTypes["application/x-python-code"]	= ".py";
	
	try
	{
		return mimeTypes.at( type );
	}
	catch( ... ) {}
    return "";
}

static bool	examinHeaders( Request& rq, std::string& first, std::string& second )
{
	if ( first == "content-length" )
	{
		rq.isContentLength = true;
		rq.contentlength = std::atoi( second.c_str() );
	}
	else if ( first == "transfer-encoding" )
	{
		if ( second == "chunked" )
			rq.transferEncoding = true;
		else
			return ( rq.stat = 501, false ) ;
	}
	if ( rq.transferEncoding == true && rq.isContentLength == true )
		rq.isContentLength = false;
	if ( rq.transferEncoding == false && rq.isContentLength == true )
	{
		if ( rq.contentlength == 0 )
		{
			Logger::log() << "[ Error ] content length is 0" << std::endl;
			return ( rq.stat = 400, false );
		}
	}
	if ( first == "host" )
	{
		if ( second.empty() )
		{
			Logger::log() << "[ Error ] Host is required" << std::endl;
			return ( rq.stat = 400, false );
		}
	}
	if ( first == "content-type" )
	{
		if ( second.empty() )
		{
			Logger::log() << "[ Error ] Content type is required" << std::endl;
			return ( rq.stat = 400, false );
		}
		std::string s1, s2;
		size_t		slash( second.find_first_of( '/' ) );
		if ( slash == std::string::npos )
		{
			Logger::log() << "[ Error ] Content type mal formed" << std::endl;
			return ( rq.stat = 400, false ); // not sure;
		}
		
		s1 = second.substr( 0 , slash );
		s2 = second.substr( slash + 1 );

		if ( s1.empty() || s2.empty() )
		{
			Logger::log() << "[ Error ] Content type mal formed" << std::endl;
			return ( rq.stat = 400, false );
		}
		if ( s1 == "multipart" )
		{
			Logger::log() << "[ Error ] Content type multipart should be processed by cgi" << std::endl;
			if ( rq.cgi == false )
				return ( rq.stat = 501, false );
		}
		else
			rq.extension = getMimeTypeForPost(second);
	}
	return true;
}

static int	extractHttpHeaders( Request& rq )
{
	std::string									line, first, second;
	std::vector<std::string>					header_lines;
	std::vector<std::string>::const_iterator	it;
	std::stringstream							headerStream ( rq.fullRequest );

	try
	{
		std::getline( headerStream, line );
		while ( std::getline( headerStream, line ) && line != "\r" )
		{
			if ( line.length() == 0 )
				break ;
			header_lines.push_back( line );
		}

		for ( it = header_lines.begin(); it != header_lines.end(); ++it )
		{
			first	=	it->substr( 0, it->find_first_of( ':' ) );
			second	=	it->substr( it->find_first_of( ':' ) + 1 );
			
			lowcase( first );
			strTrim( second );

			if ( !examinHeaders( rq, first, second ) )
				throw std::invalid_argument( "bad request: Invalid header" );
		
			rq.headers[ first ] = second;
		}
		rq.processingHeaderDone = true;
	}
	catch( const std::exception& e )
	{
		Logger::log() << "[ Error ] extractHttpHeaders header malformed" << std::endl;
		return ( rq.stat = 400, false );
	}
	return ( true );
}

static int	validateHeadersProcess( Request& rq )
{
	try
	{
		size_t	separator ( rq.headers.at( "host" ).find_first_of(':') );
		if ( separator != std::string::npos )
			rq.headers.at( "host" ).resize( separator );
			
		if ( rq.headers.at("method") == "post" )
		{
			if ( rq.headers.find( "content-type" ) == rq.headers.end() )
			{
				Logger::log() << "[ Error ] Content-Type is required" << std::endl;
				return ( rq.stat = 400, false );
			}
			if ( rq.transferEncoding == false )
			{
				if ( rq.isContentLength == false )
				{
					Logger::log() << "[ Error ] Content-Length is required" << std::endl;
					return ( rq.stat = 411, false );
				}
			}
			else
			{
				if ( rq.isContentLength == true )
					rq.isContentLength = false;
			}
		}
	}
	catch(...)
	{
		Logger::log() << "[ Error ] Host is required" << std::endl;
		return ( rq.stat = 400, false );
	}
	return ( true );
}

void	connection::fetchRequestHeader( Request& rq, char* buffer, int rc )
{
    rq.fullRequest.append( buffer, rc );
	size_t	lfp = rq.fullRequest.find( "\n\n" );
	size_t	crp = rq.fullRequest.find( "\r\n\r\n" );
	if ( lfp != std::string::npos )
	{
		rq.remainingBody = rq.fullRequest.substr( lfp + 2 );
     	rq.fetchHeaderDone = true;
	}
	if ( crp != std::string::npos )
	{
		rq.remainingBody = rq.fullRequest.substr( crp + 4 );
     	rq.fetchHeaderDone = true;
	}
}


void	connection::processingHeader( Request& rq )
{
	//intilize time request here
    if ( !extractMethodAndUri( rq ) )
		throw std::exception();
	if ( !validateUriAndExtractQueries( rq ) )
		throw std::exception();
	if ( !extractHttpHeaders( rq ) )
		throw std::exception();
	if ( !validateHeadersProcess( rq ) )
		throw std::exception();
	
}
/*-------------- yachaab code end ---------------*/