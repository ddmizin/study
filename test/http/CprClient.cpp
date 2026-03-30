#include "http/CprClient.h"
#include <cpr/cpr.h>

std::expected<std::string, std::string> CprClient::Get(const std::string& url, const StringMap& params) {
    cpr::Parameters cpr_params;
    for (const auto& pair : params) {
        cpr_params.Add({pair.first, pair.second});
    }

    cpr::Response response = cpr::Get(cpr::Url{url}, cpr_params);

    if (response.status_code == 200) {
        return response.text;
    } else {
        return std::unexpected("API Request failed: status code " + std::to_string(response.status_code) + "\n" + response.text);
    }
}