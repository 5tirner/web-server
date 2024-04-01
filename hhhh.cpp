// bool isPathWithinRoot(std::string& resolvedPath, std::string& rootPath)
// {
//     std::cout << "res: --->: " << resolvedPath << std::endl;
//     std::cout << "rooot: --->: " << rootPath << std::endl;
//     if (resolvedPath[resolvedPath.length() - 1] != '/')
//         resolvedPath += "/";
//     if (resolvedPath.substr(0, rootPath.size()) == rootPath)
//         return true;
//     return false;
// }

// std::string resolveFilePath(std::string& path)
// {
//     char *realPath = NULL;
//     realPath = realpath(path.c_str(), realPath);
    
//     if (realPath == NULL)
//         return "";
//     return std::string(realPath);
// }

// std::string mapUriToFilePath( std::string& uri,  location locConfig)
// {
//     try
//     {
//         std::string rootPath = locConfig.root.at("root"); // Use .at() for const map

//         std::string filePath = rootPath; // Start constructing the file path from the root
//         std::string locPath = locConfig.directory.at("location");
//         std::string pathSuffix;
//         if (uri.find(locPath) == 0)
//             pathSuffix = uri.substr(locPath.length());
//         else
//             pathSuffix = uri;
//         if ((pathSuffix.empty() || pathSuffix[pathSuffix.size() - 1] == '/') && locConfig.autoindex.at("autoindex") != "on")
//         {
//             std::istringstream iss(locConfig.index.at("index")); // Use .at() here as well
//             std::string indexFile;
//             std::cout << "index: " << locConfig.index.at("index") << std::endl;
//             std::string fullPath;
//             while (std::getline(iss, indexFile, ' '))
//             {
//                 fullPath = filePath + (pathSuffix[pathSuffix.length() - 1] == '/' ? pathSuffix : pathSuffix + "/") + indexFile;
//                 std::string tmp = fullPath;
//                 tmp = resolveFilePath(rootPath);
//                 std::cout << "tmp--->: " << tmp.length() << std::endl;
//                 if (tmp.empty())
//                     return "dkhal";
//                 if (!isPathWithinRoot(fullPath, rootPath))
//                 {
//                     std::cout << "\n\n---->\n\n";
//                     return "dkhal";
//                 }
//                 if (fileExists(fullPath) && isPathWithinRoot(fullPath, rootPath))
//                 {
//                     return fullPath; // Found an index file that exists and is within root
//                 }
//             }
//             return fullPath;
//             // Optional: Handle case when no index file is found...
//         }
//         else
//         {
//             // If the pathSuffix is not empty and does not end with '/', directly append it to filePath.
//             if (filePath[filePath.length() -1 ] != '/')
//                 filePath += "/";
//             filePath += pathSuffix;
//             std::string tmp = filePath;
//             tmp = resolveFilePath(rootPath);
//             std::cout << "tmp: " << tmp.length() << std::endl;
//             if (tmp.empty())
//                 return "dkhal";
//             if (!isPathWithinRoot(filePath, rootPath))
//             {
//                 std::cout << "\n\n---->\n\n";
//                 return "dkhal";
//             }
//             // if (fileExists(filePath))
//             return filePath;
//             // Handle file not found if necessary.
//         }
//     }
//     catch (const std::out_of_range& e)
//     {
//         // Handle the case where a key does not exist in the map
//         std::cerr << "Key not found in configuration: " << e.what() << '\n';
//         // Handle error, possibly return a default value or error indicator
//     }
//     return ""; // Placeholder return to satisfy all control paths
// }
