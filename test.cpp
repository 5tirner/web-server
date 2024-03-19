std::string mapUriToFilePath(std::string& uri, location& locConfig) {
    try {
        std::string rootPath = locConfig.root.at("root");
        std::string filePath = rootPath;

        // Determine pathSuffix more flexibly
        std::string locPath = locConfig.directory.at("location");
        std::string pathSuffix;
        if (uri.find(locPath) == 0) {
            // URI starts with the location path
            pathSuffix = uri.substr(locPath.length());
        } else {
            // No matching location path; URI might be requesting root or doesn't match any location
            pathSuffix = uri;
        }

        // Append slash if missing to construct the full path correctly
        if (!pathSuffix.empty() && pathSuffix.front() != '/') {
            pathSuffix = "/" + pathSuffix;
        }

        // Continue with constructing fullPath using pathSuffix as before
        // ...
    } catch (const std::out_of_range& e) {
        std::cerr << "Configuration error: " << e.what() << '\n';
    }
    return "";
}
