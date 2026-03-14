#include "ApiResponse.h"
#include <json/value.h>

using namespace drogon;

HttpResponsePtr ApiResponse::ok(const std::string &path, const Json::Value &data) {
	return createResponse(k200OK, "success", path, data);
}

HttpResponsePtr ApiResponse::success(HttpStatusCode code, const std::string &path, const Json::Value &data) {
	return createResponse(code, "success", path, data);
}

HttpResponsePtr ApiResponse::error(HttpStatusCode code, const std::string &message, const std::string &path) {
	return createResponse(code, message, path, Json::nullValue);
}

std::string ApiResponse::getTimestamp() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
	return ss.str();
}

HttpResponsePtr ApiResponse::createResponse(HttpStatusCode code, const std::string &msg, const std::string &path, const Json::Value &data) {
	Json::Value root;
	root["status"] = (int)code;
	root["message"] = msg;
	root["path"] = path;
	root["timestamp"] = getTimestamp();
	if(!data.isNull()) {
		root["data"] = data;
	}

	auto res = HttpResponse::newHttpJsonResponse(root);
	res->setStatusCode(code);
	return res;
}
