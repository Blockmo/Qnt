#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>  // Include the JSON library header
#include <sstream>      // Include the stringstream header

// Callback function to handle the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to fetch K-Line data from Binance
std::vector<Json::Value> getKLineData(const std::string& symbol, const std::string& interval, int limit) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    std::vector<Json::Value> klines;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        std::string url = "https://api.binance.com/api/v3/klines?symbol=" + symbol + "&interval=" + interval + "&limit=" + std::to_string(limit);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse JSON response
            Json::CharReaderBuilder rbuilder;
            std::string errs;
            Json::Value jsonObj;
            std::istringstream s(readBuffer); // Create a string stream from the buffer
            bool parsingSuccessful = Json::parseFromStream(rbuilder, s, &jsonObj, &errs);
            if (parsingSuccessful) {
                for (const auto& kline : jsonObj) {
                    klines.push_back(kline);
                }
            } else {
                std::cerr << "Failed to parse JSON: " << errs << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return klines;
}

int main() {
    std::string symbol = "BTCUSDT";
    std::string interval = "1h"; // Example interval: 1 minute, 1 hour, 1 day, etc.
    int limit = 5; // Number of candlesticks to retrieve

    std::vector<Json::Value> klines = getKLineData(symbol, interval, limit);

    // Print K-Line data
    for (const auto& kline : klines) {
        std::cout << "Open time: " << kline[0].asUInt64() << ", "
                  << "Open: " << kline[1].asString() << ", "
                  << "High: " << kline[2].asString() << ", "
                  << "Low: " << kline[3].asString() << ", "
                  << "Close: " << kline[4].asString() << ", "
                  << "Volume: " << kline[5].asString() << std::endl;
    }

    return 0;
}
