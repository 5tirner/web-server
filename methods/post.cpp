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

// static size_t parseChunkHeader(Request& request, std::string& buffer)
// {
//     // Remove leading CRLF if present
// 	std::string chunkHeader;
//     if (request.iscr)
// 	{
//         buffer.erase(0, 2);
//         request.iscr = false;
//     }
// 	else if (request.islf)
// 	{
//         buffer.erase(0, 1);
//         request.islf = false;
//     }

//     size_t crlfPos = buffer.find("\r\n");
//     if (crlfPos == std::string::npos)
// 	{
// 		chunkHeader = buffer;
// 		// std::cerr << "chunkHeader1: " << chunkHeader << std::endl;
// 	}
// 	else
// 	{
// 		if ( crlfPos == 0 )
// 		{
// 			buffer.erase( 0, 2 );
// 			request.isChunkHeader = false;
//         	return std::string::npos;
// 		}
// 		else
//     		chunkHeader = buffer.substr(0, crlfPos);
// 		// std::cerr << "chunkHeader2: " << chunkHeader << " | crlfPos: " << crlfPos << std::endl;
// 	}

//     char* endPtr;
//     long chunkSize = std::strtol(chunkHeader.data(), &endPtr, 16);

//     if (endPtr == chunkHeader.data() || (*endPtr != '\0' && !std::isspace(*endPtr)))
// 	{
// 		std::cerr << "Chunk Hexa Header" << chunkHeader << std::endl;
//         request.stat = 400;
//         Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
//         throw std::invalid_argument("Invalid chunk size header");
//     }

//     if (chunkSize == LONG_MAX || chunkSize == LONG_MIN)
// 	{
//         request.stat = 400;
//         Logger::log() << "[ Error ] Invalid chunk size header" << std::endl;
//         throw std::overflow_error("Chunk size overflow/underflow");
//     }

//     request.currentChunkSize = static_cast<size_t>(chunkSize);
// 	if ( crlfPos != std::string::npos )
//     	buffer.erase(0, crlfPos + 2);
// 	else
// 		buffer.erase(0, chunkHeader.size());

//     return request.currentChunkSize;
// }

// static bool processingChunkHeader( Request& request, std::string& buffer, size_t& bufferlength )
// {
	

// 	return false;
// }

// static bool chunkSizeSmallerThanBufferLength( Request& request, std::string&buffer, size_t& bufferLength )
// {
// 	request.bodyStream->write( buffer.data(), request.currentChunkSize );
// 	if ( !request.bodyStream->good() )
// 		return request.stat = 500, false;

// 	request.bodyStream->flush();

// 	bufferLength -= request.currentChunkSize;
// 	if ( bufferLength > 1 )
// 	{
// 		buffer = buffer.substr( request.currentChunkSize + 2 );
// 		bufferLength -= 2;
// 	}
// 	else if ( bufferLength == 1 )
// 	{
// 		buffer = buffer.substr( request.currentChunkSize + 1 );
// 		request.islf = true;
// 		bufferLength -= 1;
// 	}
// 	request.isChunkHeader = true;
// 	return true;
// }

// static bool    chunkedComplete( Request& request,  std::string& buffer )
// {
// 	size_t	bufferLength ( buffer.length() );

// 	while ( bufferLength != 0 )
// 	{
// 		if ( request.isChunkHeader == true )
// 		{
// 			size_t tmp = parseChunkHeader( request, buffer );

// 			if ( request.currentChunkSize == std::string::npos )
// 			{
// 				bufferLength = buffer.size();
// 				return false;
// 			}

// 			request.chunkSizeSum += request.currentChunkSize;
// 			if ( request.chunkSizeSum > request.limitClientBodySize )
// 			{
// 				request.stat = 413;
// 				throw std::exception();
// 			}

// 			bufferLength = buffer.size();

// 			if ( request.currentChunkSize == 0 )
// 				return true;
// 			// if ( processingChunkHeader( request, buffer, bufferLength ) )
// 			// 	return true;
// 			// else
// 			// {
// 			// 	if ( bufferLength == 0 )
// 			// 		break ;
// 			// }
// 		}
// 		if ( request.currentChunkSize > buffer.length() )
// 		{
// 			request.bodyStream->write( buffer.data(),  buffer.length() );
// 			if ( !request.bodyStream->good() )
// 			{
// 				request.stat = 500;
// 				throw std::exception();
// 			}
// 			request.bodyStream->flush();
// 			request.currentChunkSize -= buffer.length();
// 			request.isChunkHeader = false;
// 			return ( false );
// 		}
// 		else if ( request.currentChunkSize < buffer.length() )
// 		{
// 			if ( !chunkSizeSmallerThanBufferLength( request, buffer, bufferLength ) )
// 				throw std::exception();
// 		}
// 		else if ( request.currentChunkSize == buffer.length() )
// 		{
// 			std::cerr << "BUFFER LENGTH == CHUNK LENGTH" << std::endl;
// 			request.bodyStream->write( buffer.data(), request.currentChunkSize );
// 			request.bodyStream->flush();
// 			bufferLength -= request.currentChunkSize;
// 			request.iscr = true;
// 			request.isChunkHeader = true;
// 			return false;
// 		}
// 	}
// 	return false;
// }

// Parses the chunk header and updates the request with the chunk size
static size_t parseChunkHeader(Request& request, std::string& buffer)
{
    if (buffer.empty())
	{
		// std::cerr << "1" << std::endl;
        return std::string::npos;
	}

    // Handle leading CRLF from previous chunk
    if ( request.expectCRLF )
	{
        if (buffer.size() >= 2 && buffer.substr(0, 2) == "\r\n")
		{
			// std::cerr << "CRLF FOUND IN PARSE: " << std::endl;
            buffer.erase(0, 2);
		}
		else
		{
			if ( buffer[0] == '\r' )
			{
				buffer.erase( 0, 1 );
            	return std::string::npos;
			}
			else if ( buffer[0] == '\n' )
				buffer.erase( 0, 1 );
		}

        request.expectCRLF = false;
    }

    // Append incomplete header from previous call
    buffer = request.partialChunkHeader + buffer;
    request.partialChunkHeader.clear();

    size_t crlfPos = buffer.find("\r\n");
    if (crlfPos == std::string::npos)
	{
        request.partialChunkHeader = buffer;
		// std::cerr << "3" << std::endl;
        return std::string::npos;
    }

    std::string chunkHeader = buffer.substr(0, crlfPos);
	std::cerr << "CHUNK HEAD STRING: " << chunkHeader << std::endl;
    char* endPtr = NULL;
    long chunkSize = std::strtol(chunkHeader.c_str(), &endPtr, 16);
    if (endPtr == chunkHeader.data() || (*endPtr != '\0' && !std::isspace(*endPtr)))
	{
		request.stat = 500;
		// std::cerr << "chunkHeader: " << chunkHeader << std::endl;
        throw std::invalid_argument("Invalid chunk size header");
    }
    if (chunkSize == LONG_MAX || chunkSize == LONG_MIN)
	{
		request.stat = 500;
		// std::cerr << "chunkHeader2: " << chunkHeader << std::endl;
        throw std::overflow_error("Chunk size overflow/underflow");
    }

    request.currentChunkSize = static_cast<size_t>(chunkSize);
    buffer.erase(0, crlfPos + 2);
	request.isChunkHeader = false;
    return 0;
}

// Manages chunk header processing and transitions to chunk body processing
// static bool processingChunkHeader(Request& request, std::string& buffer, size_t& bufferLength)
// {
//     size_t chunkSize = parseChunkHeader(request, buffer);

//     if (chunkSize == std::string::npos)
// 	{
//         bufferLength = buffer.size();
//         return false; // Header incomplete, wait for more data
//     }

//     request.chunkSizeSum += request.currentChunkSize;
//     if (request.chunkSizeSum > request.limitClientBodySize) {
//         throw std::runtime_error("Request body size limit exceeded");
//     }

//     bufferLength = buffer.size();
//     return request.currentChunkSize == 0; // True if this is the last chunk (0-size)
// }

// Processes the chunk body when its size is within the buffer length
// static bool chunkSizeSmallerThanBufferLength(Request& request, std::string& buffer, size_t& bufferLength)
// {
//     request.bodyStream->write(buffer.data(), request.currentChunkSize);

//     if (!request.bodyStream->good())
//         throw std::runtime_error("Error writing to body stream");

//     request.bodyStream->flush();

//     buffer.erase(0, request.currentChunkSize);
//     bufferLength = buffer.size();
// 	if ( bufferLength > 1 )
// 		request.expectCRLF = true;
//     return true;
// }

// Orchestrates chunk processing, handling various cases based on chunk and buffer sizes
static bool chunkedComplete(Request& request, std::string& buffer)
{
    size_t bufferLength = buffer.size();

    while ( bufferLength != 0 )
	{
        if ( request.isChunkHeader )
		{
			// std::cerr << "buffer Length 0: " << bufferLength << std::endl;

			size_t chunkSize = parseChunkHeader(request, buffer);
			if ( chunkSize == std::string::npos )
			{
				// std::cerr << "HETE" << std::endl;
				return false; // Header incomplete, wait for more data
			}

			request.chunkSizeSum += request.currentChunkSize;
			if (request.chunkSizeSum > request.limitClientBodySize) {
				throw std::runtime_error("Request body size limit exceeded");
			}

			bufferLength = buffer.size();
			// std::cerr << "buffer Length 1: " << bufferLength << std::endl;
			return request.currentChunkSize == 0;
        }
		if( request.currentChunkSize <= bufferLength )
		{
			// std::cerr << "<" << std::endl;
			request.bodyStream->write(buffer.data(), request.currentChunkSize);

			if (!request.bodyStream->good())
				throw std::runtime_error("Error writing to body stream");

			request.bodyStream->flush();
			buffer.erase(0, request.currentChunkSize);
			bufferLength = buffer.size();
			if ( buffer.substr( 0, 2 ) == "\r\n" || bufferLength == 0 )
			{
				// std::cerr << "CRLF FOUND" << std::endl;
				request.expectCRLF = true;
			}
			if ( bufferLength == 1 )
			{
				// std::cerr << "CR ONLY FOUND" << std::endl;
				request.expectCRLF = true;
			}
			request.isChunkHeader = true; // Ready for next chunk header
		}
		else
		{
			// std::cerr << ">" << std::endl;
			// std::cerr << "buffer Length 2: " << bufferLength << std::endl;
			// Chunk size larger than buffer, process partial chunk
			
			request.bodyStream->write(buffer.data(), bufferLength);
			// if ( buffer.find( "\r\n" ) != std::string::npos )
			// {
			// 	std::cerr << "chunk size " << request.currentChunkSize << std::endl;
			// 	std::cerr << "INDEX: " << buffer.find( "\r\n" ) << std::endl;
			// 	request.expectCRLF = true;
			// }
			request.currentChunkSize -= bufferLength;
			buffer.clear();
			request.isChunkHeader = false;
			return false; // Need more data to complete chunk
		}
    }
    return false; // More data needed
}

static void	processChunkedRequestBody( Request& rq, char* buffer, int& rc )
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
			rq.readyToSendRes = true;
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
			rq.readyToSendRes = true;
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

	if ( !request.bodyStream->good() ) {
		request.stat = 500;
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
		throw std::exception();
	}
	else if (request.bytesWrite > request.contentlength)
	{
		request.stat = 413;
		throw std::length_error("Request body exceeds declared length");
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
			processChunkedRequestBody( rq, buffer, rc );
		else if ( rq.isContentLength == true )
		{
			if ( rq.contentlength <= rq.limitClientBodySize )
				processRegularRequestBody( rq, buffer , rc );
			else
			{
				rq.stat = 413;
				throw std::length_error("Request body exceeds declared limit client length");
			}
		}
    }
}
