#pragma once
#include <drogon/HttpAppFramework.h>
#include <json/json.h>

using namespace drogon;


class ApiResponse {
public:
    static HttpResponsePtr ok(const std::string &path, const Json::Value &data);
    static HttpResponsePtr success(HttpStatusCode code, const std::string &path, const Json::Value &data);
    static HttpResponsePtr error(HttpStatusCode code, const std::string &message, const std::string &path);

private:
    static std::string getTimestamp();
    static HttpResponsePtr createResponse(HttpStatusCode code, const std::string &msg, const std::string &path, const Json::Value &data);
};