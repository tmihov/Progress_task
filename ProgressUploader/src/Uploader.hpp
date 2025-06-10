#include <curl/curl.h>
#include <curl/easy.h>
#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

class Uploader {
public:
  Uploader(const std::string username, const std::string password) {
    curl = curl_easy_init();
    if (!curl) {
      throw std::runtime_error("Failed to init curl.");
    }

    token = getToken(username, password);

    if (token.empty()) {
      throw std::runtime_error("Failed to get bearer token.");
    }

    homeID = getHomeFolderID(token);

    if (homeID.empty()) {
      throw std::runtime_error("Failed to get Home folder ID token.");
    }
  };
  ~Uploader() { curl_easy_cleanup(curl); };

  void uploadToHome(const std::string filePath) {
    uploadFile(token, homeID, filePath);
  };

private:
  std::string token;
  std::string homeID;

  CURL *curl = nullptr;
  CURLcode res;
  const std::string URL = {"https://testserver.moveitcloud.com/"};

  const std::string getToken(const std::string username,
                             const std::string password) {
    curl_easy_reset(curl);
    std::string response;

    const std::string postfields = {"grant_type=password&username=" + username +
                                    "&password=" + password};
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, (URL + "api/v1/token").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "HTTP code: " << response_code << std::endl
                << response << std::endl;
      throw std::runtime_error("curl_easy_perform() failed: " +
                               std::string(curl_easy_strerror(res)));
    }

    nlohmann::json j;
    try {
      j = nlohmann::json::parse(response);

    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
    }

    return j.value("access_token", "");
  }

  const std::string getHomeFolderID(const std::string token) {
    curl_easy_reset(curl);
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (URL + "/api/v1/users/self").c_str());
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
    curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, token.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "HTTP code: " << response_code << std::endl
                << response << std::endl;
      throw std::runtime_error("curl_easy_perform() failed: " +
                               std::string(curl_easy_strerror(res)));
    }

    nlohmann::json j;
    try {
      j = nlohmann::json::parse(response);

    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
    }

    return std::to_string(j.value("homeFolderID", -1));
  }

  void uploadFile(const std::string token, const std::string folderId,
                  const std::string file_path) {
    curl_easy_reset(curl);
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL,
                     (URL + "/api/v1/folders/" + folderId + "/files").c_str());
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
    curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, token.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_mime *form = nullptr;
    curl_mimepart *field = nullptr;

    form = curl_mime_init(curl);

    field = curl_mime_addpart(form);
    curl_mime_name(field, "sendfile");
    curl_mime_filedata(field, file_path.c_str());

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    std::cout << "Uploading File..." << std::endl;

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      long response_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
      std::cout << "HTTP code: " << response_code << std::endl;
      throw std::runtime_error("curl_easy_perform() failed: " +
                               std::string(curl_easy_strerror(res)));
    }

    std::cout << "Finished uploading." << std::endl;

    curl_mime_free(form);
  }
};