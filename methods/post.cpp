#include "../include/mainHeader.hpp"
#include <dirent.h>

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
        if (filePath.at( filePath.size() - 1 ) != '/')
            filePath.push_back('/');
        filePath += randomFileName + request.extension;

        request.filename = filePath;
        request.bodyStream->open(filePath.c_str(), std::ios::binary | std::ios::trunc);

        if (!request.bodyStream->is_open())
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

// int connection::location_support_upload( Request& rq, int serverID )
// {	
// 	try
// 	{
// 		std::string	location;
// 		informations serverInfo;
// 		std::map<std::string, informations>::iterator it = notBindingServers.find( rq.headers.at("host") );
// 		if ( it != notBindingServers.end() )
// 			serverInfo = it->second;
// 		else
// 			serverInfo = OverLoad.at( serverID );

// 		size_t i = 0;
// 		size_t longestMatchingIndex = std::string::npos;
// 		std::string longestMatchingLocation;
// 		std::string uri( rq.headers.at("uri") );
// 		for (; i < serverInfo.locationsInfo.size(); i++ )
// 		{
// 			location = serverInfo.locationsInfo.at(i).directory.at( "location" );
// 			if ( uri.compare(0, location.size(), location.c_str()) == 0 && location.length() > longestMatchingLocation.length())
// 			{
// 				longestMatchingIndex = i;
// 				longestMatchingLocation = location;
// 			}
// 		}
// 		i = longestMatchingIndex;
// 		if ( longestMatchingIndex != std::string::npos )
// 		{
// 			bool isdir( false );
// 			if ( serverInfo.locationsInfo.at( i ).cgi.at("cgi") == "on" )
// 			{
// 				rq.scriptName = serverInfo.locationsInfo.at( i ).root.at("root") + uri.substr(longestMatchingLocation.length());
// 				if (GetExtension(rq.scriptName) != "NormalFile")
// 					rq.cgi = true;
// 				else if (isDirectory(rq.scriptName))
// 				{
// 					rq.scriptName += "/";
// 					std::string indexPath;
// 					if ((rq.scriptName.empty() || rq.scriptName[rq.scriptName.length() - 1] == '/'))
// 					{
// 						std::istringstream iss(serverInfo.locationsInfo.at( i ).index.at("index"));
// 						std::string indexFile;
// 						while (std::getline(iss, indexFile, ' '))
// 						{
// 							indexPath = rq.scriptName + indexFile;
// 							if (!indexPath.empty() && fileExists(indexPath))
// 							{
// 								rq.cgi = true;
// 								isdir = true;
// 								break;
// 							}
// 						}
// 					}
// 				}
// 			}	
// 			std::string uploadDirectory = serverInfo.locationsInfo.at(i).upload.at("upload");
//             std::string allowedMethods = serverInfo.locationsInfo.at(i).allowed_methodes.at("allowed_methodes");
// 			if ( upload[0] || rq.cgi )
// 			{
// 				if ( rq.cgi && !isdir )
// 					upload = "/tmp";
// 				if ( method.find( "POST" ) != std::string::npos )
// 				{
// 					if ( access( upload.c_str(), F_OK ) == 0 )
// 					{
// 						if ( access( upload.c_str(), W_OK ) != 0 )
// 						{
// 							Logger::log() << "[ Error ] : Client can't upload in this location write permission"<< std::endl;
// 							return ( rq.stat = 403, -1 );
// 						}
// 					}
// 					else
// 					{
// 						Logger::log() << "[ Error ] : Client can't upload in this location dir not found"<< std::endl;
// 						return ( rq.stat = 404, -1 );
// 					}
// 					rq.limitClientBodySize = std::atol( serverInfo.limitClientBody.at("limit_client_body").c_str() ) * 100000000; //! need the exact amount
// 					if ( rq.limitClientBodySize == 0 )
// 					{
// 						Logger::log() << "[ Error ] : Client body size limit is 0" << std::endl;
// 						return ( rq.stat = 400, -1 );
// 					}
// 					if ( !rq.bodyStream->is_open() )
// 						generateRandomFileName( rq, upload );
// 					rq.locationGotChecked = true;
// 					return ( 0 );
// 				}
// 				else
// 					return ( rq.stat = 405, -1 );
// 			}
// 		}	
// 	}
// 	catch(const std::exception& e)
// 	{
// 		Logger::log() << "[ Error ] : Client can't upload in this location " << std::endl;
// 		return ( rq.stat = 500, -1 );
// 	}
// 	Logger::log() << "[ Error ] : Client can't upload in this location method not allowed" << std::endl;
// 	return ( rq.stat = 404, -1 );
// }

int connection::location_support_upload(Request& request, int serverId) {
    try {
        std::string location;
        informations serverInfo;
        std::map<std::string, informations>::iterator it = notBindingServers.find(request.headers.at("host"));
        if (it != notBindingServers.end()) {
            serverInfo = it->second;
        } else {
            serverInfo = OverLoad.at(serverId);
        }

        std::string uri(request.headers.at("uri"));
        std::string longestMatchingLocation;
        size_t longestMatchingIndex = std::string::npos;

        for (size_t i = 0; i < serverInfo.locationsInfo.size(); ++i) {
            location = serverInfo.locationsInfo.at(i).directory.at("location");
            if (uri.compare(0, location.size(), location) == 0 && location.length() > longestMatchingLocation.length()) {
                longestMatchingIndex = i;
                longestMatchingLocation = location;
            }
        }

        if (longestMatchingIndex != std::string::npos)
		{
            bool isDir = false;
            size_t i = longestMatchingIndex;
            if (serverInfo.locationsInfo.at(i).cgi.at("cgi") == "on")
			{
                request.scriptName = serverInfo.locationsInfo.at(i).root.at("root") + uri.substr(longestMatchingLocation.length());
                if (GetExtension(request.scriptName) != "NormalFile") request.cgi = true;
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

			std::string allowedMethods = serverInfo.locationsInfo.at(i).allowed_methodes.at("allowed_methodes");
			std::string uploadDirectory = serverInfo.locationsInfo.at(i).upload.at("upload");

			if (!uploadDirectory.empty() || request.cgi)
			{ 
				if (request.cgi && !isDir)
					uploadDirectory = "/tmp";
			}
			
			if (allowedMethods.find("POST") != std::string::npos)
			{
				if (access(uploadDirectory.c_str(), F_OK) == 0) {
					if (access(uploadDirectory.c_str(), W_OK) != 0) {
						Logger::log() << "[ Error ]: Client can't upload in this location due to write permission" << std::endl;
						return request.stat = 403, -1;
					}
				}
				else
				{
					Logger::log() << "[ Error ]: Client can't upload in this location as directory not found" << std::endl;
					return request.stat = 404, -1;
				}

				request.limitClientBodySize = std::atol(serverInfo.limitClientBody.at("limit_client_body").c_str()) * 100000000; //! what???
				if (request.limitClientBodySize == 0)
				{
					Logger::log() << "[ Error ]: Client body size limit is 0" << std::endl;
					return request.stat = 400, -1;
				}

				if (!request.bodyStream->is_open()) generateRandomFileName(request, uploadDirectory);

				request.locationGotChecked = true;
				return 0;
			}
			else
				return request.stat = 405, -1;
        }
    } catch (const std::exception& e) {
        Logger::log() << "[ Error ]: Client can't upload in this location" << std::endl;
        return request.stat = 500, -1;
    }

    Logger::log() << "[ Error ]: Client can't upload in this location as method not allowed" << std::endl;
    return request.stat = 404, -1;
}

// static size_t	parseChunkHeader( Request& rq, std::string& buffer )
// {
// 	std::string	chunkHead;
// 	long		size;
// 	size_t 		crlfPos = 0;

// 	if ( rq.iscr == true )
// 	{
// 		buffer = buffer.substr( 2 );
// 		rq.iscr = false;
// 	}
// 	else if ( rq.islf == true )
// 	{
// 		buffer = buffer.substr( 1 );
// 		rq.islf = false;
// 	}

// 	crlfPos = buffer.find( "\r\n" );
	
// 	if ( crlfPos == std::string::npos )
// 		return std::string::npos;
// 	chunkHead = buffer.substr( 0, crlfPos );
// 	if ( !chunkHead.empty() )
// 		size = std::strtol( chunkHead.c_str(), NULL, 16 );
// 	else
// 	{
// 		rq.stat = 400;
// 		Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
// 		throw std::exception();
// 	}
// 	if ( size == LONG_MAX || size == LONG_MIN )
// 	{
// 		rq.stat = 400;
// 		Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
// 		throw std::exception();
// 	}
// 	else
// 		rq.currentChunkSize = size;
// 	buffer = buffer.substr( chunkHead.length() + 2 );
// 	return ( rq.currentChunkSize );
// }

static size_t parseChunkHeader(Request& request, std::string& buffer)
{
    // Remove leading CRLF if present
    if (request.iscr)
	{
        buffer.erase(0, 2);
        request.iscr = false;
    }
	else if (request.islf)
	{
        buffer.erase(0, 1);
        request.islf = false;
    }

    size_t crlfPos = buffer.find("\r\n");
    if (crlfPos == std::string::npos)
        return std::string::npos;

    std::string chunkHeader = buffer.substr(0, crlfPos);

    char* endPtr;
    long chunkSize = std::strtol(chunkHeader.c_str(), &endPtr, 16);

    if (endPtr == chunkHeader.c_str() || (*endPtr != '\0' && !std::isspace(*endPtr)))
	{
        request.stat = 400;
        Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
        throw std::invalid_argument("Invalid chunk size header");
    }

    if (chunkSize == LONG_MAX || chunkSize == LONG_MIN)
	{
        request.stat = 400;
        Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
        throw std::overflow_error("Chunk size overflow/underflow");
    }

    request.currentChunkSize = static_cast<size_t>(chunkSize);
    buffer.erase(0, crlfPos + 2);

    return request.currentChunkSize;
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
			rq.bodyStream->write( buffer.data(),  buffer.length() );
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
			rq.bodyStream->write( buffer.data(), rq.currentChunkSize );
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
			rq.bodyStream->write( buffer.data(), rq.currentChunkSize );
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
			if (!rq.cgi)
				rq.stat = 201;
			else
			{
				rq.cgiInfo.contentLength = rq.headers["content-length"];
				rq.cgiInfo.contentType = rq.headers["content-type"];
				rq.cgiInfo.method = "POST";
				rq.cgiInfo.input = rq.filename;
				rq.headers.at("method") = "get";
			}
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
			if (!rq.cgi)
				rq.stat = 201;
			else
			{
				rq.cgiInfo.contentLength = rq.headers["content-length"];
				rq.cgiInfo.contentType = rq.headers["content-type"];
				rq.cgiInfo.method = "POST";
				rq.cgiInfo.input = rq.filename;
				rq.headers.at("method") = "get";
			}
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
		rq.bodyStream->write( rq.remainingBody.data(),  rq.remainingBody.length() );
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
		if (!rq.cgi)
			rq.stat = 201;
		else
		{
			rq.cgiInfo.contentLength = rq.headers["content-length"];
			rq.cgiInfo.contentType = rq.headers["content-type"];
			rq.cgiInfo.method = "POST";
			rq.cgiInfo.input = rq.filename;
			rq.headers.at("method") = "get";
		}
		sendRes = true;
		Logger::log() << "[ sucess ] body file created" << std::endl;
		throw std::exception();
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
		else if ( rq.transferEncoding == true )
			processChunkedRequestBody( rq, buffer, rc, rq.readyToSendRes );
		else if ( rq.contentLength == true )
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
