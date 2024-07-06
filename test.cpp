#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <sstream>

using namespace std;

// Callback function to handle the response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void getContractPrice(const string& symbol) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        string url = "https://api.binance.com/api/v3/ticker/price?symbol=" + symbol;
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            cout << "Response data: " << readBuffer << endl;
        }
        
        curl_easy_cleanup(curl);
    }
}

// Function to fetch all symbols from Binance
vector<string> getAllSymbols() {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    vector<string> symbols;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        string url = "https://api.binance.com/api/v3/exchangeInfo";

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Parse JSON response
            Json::CharReaderBuilder rbuilder;
            string errs;
            Json::Value jsonObj;
            istringstream s(readBuffer);
            bool parsingSuccessful = Json::parseFromStream(rbuilder, s, &jsonObj, &errs);
            if (parsingSuccessful) {
                const Json::Value& symbolsArray = jsonObj["symbols"];
                for (const auto& symbolObj : symbolsArray) {
                    symbols.push_back(symbolObj["symbol"].asString());
                }
            } else {
                cerr << "Failed to parse JSON: " << errs << endl;
            }
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    return symbols;
}

int main() {
    // string symbol = "BTCUSDT";
    // getContractPrice(symbol);
    
    vector<string> symbols = getAllSymbols();
    for(string s:symbols)
    {
        if(s.find("USDT") != std::string::npos)
        getContractPrice(s);
    }

    return 0;
}