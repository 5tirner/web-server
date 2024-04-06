#include "../include/mainHeader.hpp"

static void generateRandomFileName( Request& request, const std::string& uploadDirectory )
{
    try
    {
        const std::string validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::string randomFileName;

        std::srand(std::time(NULL));
		for ( int i = 0; i < 25; i++ )
			randomFileName.push_back( validChars[ rand() % validChars.length() ] );

        std::string filePath = uploadDirectory;
        if ( filePath.at( filePath.size() - 1 ) != '/' )
            filePath.push_back( '/' );
        filePath += randomFileName + request.extension;

        request.filename = filePath;
        request.bodyStream->open(filePath.c_str(), std::ios::binary | std::ios::trunc);

        if ( !request.bodyStream->is_open() )
        {
            Logger::log() << "[ Error ]: Could not open file to store request body" << std::endl;
            throw std::runtime_error("Failed to open file");
        }
    }
    catch (const std::exception& e)
    {
        request.stat = 500;
        throw std::exception();
    }
}

int connection::location_support_upload(Request& request, int serverId)
{
    try {
        std::string location;
        informations serverInfo;

        std::map<std::string, informations>::iterator it = notBindingServers.find(request.headers.at("host"));
        if ( it != notBindingServers.end() )
            serverInfo = it->second;
		else
            serverInfo = OverLoad.at(serverId);

        std::string uri(request.headers.at("uri"));
        std::string longestMatchingLocation;
        size_t longestMatchingIndex = std::string::npos;

        for ( size_t i = 0; i < serverInfo.locationsInfo.size(); ++i )
		{
            location = serverInfo.locationsInfo.at(i).directory.at("location" );
            if ( uri.compare(0, location.size(), location) == 0 && location.length() > longestMatchingLocation.length() )
			{
                longestMatchingIndex = i;
                longestMatchingLocation = location;
            }
        }

        if ( longestMatchingIndex != std::string::npos )
		{
            bool isDir = false;
            size_t i = longestMatchingIndex;
            if ( serverInfo.locationsInfo.at(i).cgi.at("cgi") == "on" )
			{
                request.scriptName = serverInfo.locationsInfo.at(i).root.at("root") + uri.substr( longestMatchingLocation.length() );
                if ( GetExtension(request.scriptName ) != "NormalFile") request.cgi = true;
				else if (isDirectory(request.scriptName))
				{
                    request.scriptName += "/";
                    std::string indexPath;
                    if (request.scriptName.empty() || request.scriptName.at(request.scriptName.size() - 1) == '/')
					{
                        std::istringstream iss(serverInfo.locationsInfo.at(i).index.at("index"));
                        std::string indexFile;
                        while (std::getline(iss, indexFile, ' '))
						{
                            indexPath = request.scriptName + indexFile;
                            if (!indexPath.empty() && fileExists(indexPath))
							{
                                request.cgi = true;
                                isDir = true;
                                break;
                            }
                        }
                    }
                }
            }
			if ( !request.cgi )
			{
				std::string second = request.headers.at("content-type");
				if ( second.empty() )
				{
					Logger::log() << "[ Error ] Content type is required" << std::endl;
					return ( request.stat = 400, -1 );
				}
				std::string s1, s2;
				size_t		slash( second.find_first_of( '/' ) );
				if ( slash == std::string::npos )
				{
					Logger::log() << "[ Error ] Content type mal formed" << std::endl;
					return ( request.stat = 400, -1 );
				}
				
				s1 = second.substr( 0 , slash );
				s2 = second.substr( slash + 1 );

				if ( s1.empty() || s2.empty() )
				{
					Logger::log() << "[ Error ] Content type mal formed" << std::endl;
					return ( request.stat = 400, -1 );
				}
				if ( s1 == "multipart" )
				{
					Logger::log() << "[ Error ] Content type multipart should be processed by cgi" << std::endl;
					return ( request.stat = 501, -1 );
				}
				else
					request.extension = getMimeTypeForPost(second);
			}

			std::string allowedMethods = serverInfo.locationsInfo.at(i).allowed_methodes.at("allowed_methodes");
			std::string uploadDirectory = serverInfo.locationsInfo.at(i).upload.at("upload");

			if (!uploadDirectory.empty() || request.cgi)
			{ 
				if (request.cgi && !isDir)
					uploadDirectory = "/tmp";
			}
			
			if ( allowedMethods.find("POST") != std::string::npos )
			{
				if ( access(uploadDirectory.c_str(), F_OK) == 0 ) {
					if ( access(uploadDirectory.c_str(), W_OK) != 0 ) {
						Logger::log() << "[ Error ]: Client can't upload in this location due to write permission" << std::endl;
						return request.stat = 403, -1;
					}
				}
				else
				{
					Logger::log() << "[ Error ]: Client can't upload in this location as directory not found" << std::endl;
					return request.stat = 404, -1;
				}

				request.limitClientBodySize = std::atol(serverInfo.limitClientBody.at("limit_client_body").c_str()) * 1000000; //! what???
				if (request.limitClientBodySize == 0)
				{
					Logger::log() << "[ Error ]: Client body size limit is 0" << std::endl;
					return request.stat = 400, -1;
				}

				if ( !request.bodyStream->is_open() ) generateRandomFileName( request, uploadDirectory );

				request.locationGotChecked = true;
				return 0;
			}
			else
				return request.stat = 405, -1;
        }
    } catch ( const std::exception& e )
	{
        Logger::log() << "[ Error ]: Client can't upload in this location" << std::endl;
        return request.stat = 500, -1;
    }

    Logger::log() << "[ Error ]: Client can't upload in this location as method not allowed" << std::endl;
    return request.stat = 404, -1;
}

static size_t parseChunkHeader( Request& request, std::string& buffer )
{
    if ( buffer.empty() )
        return std::string::npos;

    if ( request.expectCRLF )
	{
        if ( buffer.size() >= 2 && buffer.substr(0, 2) == "\r\n" )
            buffer.erase( 0, 2 );
		else
		{
			if ( buffer[ 0 ] == '\r' )
			{
	
				buffer.erase( 0, 1 );
            	return std::string::npos;
			}
			else if ( buffer[ 0 ] == '\n' )
				buffer.erase( 0, 1 );
		}
        request.expectCRLF = false;
    }

    buffer = request.partialChunkHeader + buffer;
    request.partialChunkHeader.clear();

    size_t crlfPos = buffer.find("\r\n");
    if ( crlfPos == std::string::npos )
	{
        request.partialChunkHeader = buffer;
        return std::string::npos;
    }

    std::string chunkHeader = buffer.substr(0, crlfPos);
    char* endPtr = NULL;
    long chunkSize = std::strtol( chunkHeader.c_str(), &endPtr, 16 );
    if ( endPtr == chunkHeader.data() || ( *endPtr != '\0' && !std::isspace( *endPtr ) ) )
	{
		request.stat = 500;
        throw std::invalid_argument("Invalid chunk size header");
    }
    if ( chunkSize == LONG_MAX || chunkSize == LONG_MIN )
	{
		request.stat = 500;
        throw std::overflow_error("Chunk size overflow/underflow");
    }
    request.currentChunkSize = static_cast<size_t>( chunkSize );
    buffer.erase( 0, crlfPos + 2 );
	request.isChunkHeader = false;
    return request.currentChunkSize;
}

static bool chunkedComplete(Request& request, std::string& buffer)
{
    size_t bufferLength = buffer.size();
	
    while ( bufferLength != 0 )
	{
        if ( request.isChunkHeader )
		{
			size_t chunkSize = parseChunkHeader( request, buffer );
			if ( chunkSize == std::string::npos )
				return false;

			request.chunkSizeSum += request.currentChunkSize;
			if (request.chunkSizeSum > request.limitClientBodySize)
			{
				request.stat = 413;
				request.bodyStream->close();
				std::remove( request.filename.data() );
				throw std::runtime_error("Request body size limit exceeded");
			}

			bufferLength = buffer.size();

			if ( request.currentChunkSize == 0 )
				return true;
        }
		if( request.currentChunkSize <= bufferLength )
		{

			request.bodyStream->write(buffer.data(), request.currentChunkSize);
			if (!request.bodyStream->good())
			{
				request.stat = 500;
				throw std::runtime_error("Error writing to body stream");
			}
			request.bodyStream->flush();

			buffer.erase(0, request.currentChunkSize);

			bufferLength = buffer.size();
			if ( buffer.substr( 0, 2 ) == "\r\n" || bufferLength == 0 )
				request.expectCRLF = true;

			if ( bufferLength == 1 )
				request.expectCRLF = true;

			request.isChunkHeader = true;
		}
		else
		{
			request.bodyStream->write(buffer.data(), bufferLength);
			if (!request.bodyStream->good())
			{
				request.stat = 500;
				throw std::runtime_error("Error writing to body stream");
			}
			request.bodyStream->flush();
			request.currentChunkSize -= bufferLength;
			request.isChunkHeader = false;
			return false;
		}
    }
    return false;
}

static void	processChunkedRequestBody( Request& request, char* buffer, int& rc )
{
    if ( !request.remainingBody.empty() )
    {
        if ( chunkedComplete( request, request.remainingBody ) )
        {
			if (!request.cgi)
				request.stat = 201;
			else
			{
				request.cgiInfo.contentLength = request.headers["content-length"];
				request.cgiInfo.contentType = request.headers["content-type"];
				request.cgiInfo.method = "POST";
				request.cgiInfo.input = request.filename;
				request.headers.at("method") = "get";
			}
			request.readyToSendRes = true;
			request.bodyStream->close();
			throw std::exception();
		}
        request.remainingBody.clear();
    }
    else
    {
        std::string receivedData( buffer, rc );
        if ( chunkedComplete( request, receivedData ) )
        {
			if (!request.cgi)
				request.stat = 201;
			else
			{
				request.cgiInfo.contentLength = request.headers["content-length"];
				request.cgiInfo.contentType = request.headers["content-type"];
				request.cgiInfo.method = "POST";
				request.cgiInfo.input = request.filename;
				request.headers.at("method") = "get";
			}
			request.readyToSendRes = true;
			request.bodyStream->close();
			throw std::exception();
		}
    }
}

static void processRegularRequestBody( Request& request, char* buffer, int& bytesRead )
{
	if ( !request.remainingBody.empty() )
	{
		size_t size( request.remainingBody.size() );
		request.bodyStream->write(request.remainingBody.data(), size);
		request.bodyStream->flush();
		request.bytesWrite += size;
		request.remainingBody.clear();
	}
	else if ( bytesRead > 0 )
	{
		request.bodyStream->write(buffer, bytesRead);
		request.bodyStream->flush();
		request.bytesWrite += bytesRead;
	}

	if ( !request.bodyStream->good() )
	{
		request.stat = 500;
		request.bodyStream->close();
		throw std::runtime_error("Failed to write to body stream");
	}

	if ( request.bytesWrite == request.contentlength )
	{
		if ( !request.cgi )
			request.stat = 201; // Created
		else
		{
			request.cgiInfo.contentLength = request.headers["content-length"];
			request.cgiInfo.contentType = request.headers["content-type"];
			request.cgiInfo.method = "POST";
			request.cgiInfo.input = request.filename;
			request.headers.at("method") = "get"; // Change the method to GET for CGI
		}
		request.readyToSendRes = true;
		request.bodyStream->close();
		throw std::exception();
	}
	else if (request.bytesWrite > request.contentlength)
	{
		request.stat = 413;
		request.bodyStream->close();
		std::remove( request.filename.data() );
		throw std::length_error("Request body exceeds declared length");
	}
}

void	connection::processingBody( Request& request, char* buffer, int rc, int serverID )
{
	if ( request.headers.at( "method" ) == "get" || request.headers.at( "method" ) == "delete" )
		request.readyToSendRes = true;
	else if ( request.headers.at( "method" ) == "post" )
    {
		if ( request.locationGotChecked == false && location_support_upload( request, serverID ) == -1 )
			throw std::exception();
		else if ( request.transferEncoding == true )
			processChunkedRequestBody( request, buffer, rc );
		else if ( request.isContentLength == true )
		{
			if ( request.contentlength <= request.limitClientBodySize )
				processRegularRequestBody( request, buffer , rc );
			else
			{
				request.stat = 413;
				request.bodyStream->close();
				std::remove( request.filename.data() );
				throw std::length_error("Request body exceeds declared limit client length");
			}
		}
    }
}
