#include "pch.h"
#include "Curl.h"

void Nexa::Curl::Delete(string URL, const vector<const char*>& Headers) {
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        
        struct curl_slist* headers = NULL;
        for (const char* header : Headers) {
            if (header && strlen(header) > 0) {
                headers = curl_slist_append(headers, header);
            }
        }
        
        if (headers) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
        
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        curl_easy_perform(curl);
        
        if (headers) {
            curl_slist_free_all(headers);
        }
        curl_easy_cleanup(curl);
    }
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    if (!contents || !s) return 0;
    
    size_t total_size = size * nmemb;
    try {
        s->append((char*)contents, total_size);
        return total_size;
    } catch (...) {
        return 0;
    }
}

string Nexa::Curl::Get(string URL, const vector<const char*>& Headers) {
    string response;

    try {
        CURL* curl = curl_easy_init();
        if (!curl) return "";

        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

        struct curl_slist* headers = NULL;

        // Process all headers in the vector
        for (const char* header : Headers) {
            if (header && strlen(header) > 0) {
                headers = curl_slist_append(headers, header);
            }
        }

        if (headers) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (headers) {
            curl_slist_free_all(headers);
        }

        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            return "";
        }

        return response;
    }
    catch (...) {
        return "";
    }
}

string Nexa::Curl::Post(string URL, string JsonData, const vector<const char*>& Headers) {
    string response;

    try {
        CURL* curl = curl_easy_init();
        if (!curl) return "";

        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

        struct curl_slist* headers = NULL;

        // Process all headers in the vector
        for (const char* header : Headers) {
            if (header && strlen(header) > 0) {
                headers = curl_slist_append(headers, header);
            }
        }

        if (headers) {
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, JsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (headers) {
            curl_slist_free_all(headers);
        }
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            return "";
        }

        return response;
    }
    catch (const std::exception& e) {
        return "";
    }
}