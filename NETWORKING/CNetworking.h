#pragma once
#include <curl/curl.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

class Curl {
    public:
        static std::string Get(const std::string &url) {
        CURL *curl = curl_easy_init();
        if (!curl)
            throw std::runtime_error("curl_easy_init failed");

        std::string buffer;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // handle redirects

        CURLcode rc = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (rc != CURLE_OK)
            throw std::runtime_error(curl_easy_strerror(rc));

        return buffer;
        }

    private:
        static size_t WriteCallback(void *ptr, size_t size, size_t nmemb,
                                  void *userdata) {
        auto *out = static_cast<std::string *>(userdata);
        out->append(static_cast<char *>(ptr), size * nmemb);
        return size * nmemb;
        }

    };

    namespace NETWORKING {
    inline static std::string url_encode(const std::string &s) {
        std::ostringstream o;
        o.fill('0');
        o << std::hex << std::uppercase;
        for (unsigned char c : s) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
                o << c;
        else
            o << '%' << std::setw(2) << int(c);
        }
        return o.str();
    }

    inline static void open_in_browser(const std::string &url) {
        #if defined(_WIN32)
            ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        #elif defined(__APPLE__)
          std::string cmd = "open \"" + url + "\" &";
          std::system(cmd.c_str());
        #else
          std::string cmd = "xdg-open \"" + url + "\" &";
          std::system(cmd.c_str());
        #endif
    }
}
