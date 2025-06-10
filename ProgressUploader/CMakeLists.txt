add_executable(ProgressUploader)
target_sources(ProgressUploader PRIVATE src/main.cpp)
target_link_libraries(ProgressUploader PRIVATE CURL::libcurl)
target_link_libraries(ProgressUploader PRIVATE nlohmann_json::nlohmann_json)