#include "HelloController.h"
#include "models/response/ApiResponse.h"
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>

void HelloController::helloWorld(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
  callback(ApiResponse::ok(req->getPath(), "Hello, World!"));
}

void HelloController::helloWorldPath(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, std::string &&name) {
  callback(ApiResponse::ok(req->getPath(), "Hello, World! path: " + name));
}

void HelloController::helloWorldQuery(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback) {
  auto message = req->getParameter("message");
  callback(ApiResponse::ok(req->getPath(), "Hello World! query: " + message));
}

void HelloController::helloWorldPayload(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) {
  auto jsonPtr = req->getJsonObject();
  if(!jsonPtr) {
    callback(ApiResponse::error(drogon::k400BadRequest, "invalid payload", req->getPath()));
    return;
  }

  std::string message = (*jsonPtr)["message"].asString();

  callback(ApiResponse::ok(req->getPath(), "Hello World! payload: " + message));
}

void HelloController::helloWorldError(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) {
  throw std::runtime_error("throw error");
}