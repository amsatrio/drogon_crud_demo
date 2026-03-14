#pragma once

#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <functional>

using namespace drogon;

class HelloController : public HttpController<HelloController>
{
  public:
    METHOD_LIST_BEGIN
      ADD_METHOD_TO(HelloController::helloWorld, "/v1/hello-world", Get);
      ADD_METHOD_TO(HelloController::helloWorldPath, "/v1/hello-world/path/{name}", Get);
      ADD_METHOD_TO(HelloController::helloWorldQuery, "/v1/hello-world/query", Get);
      ADD_METHOD_TO(HelloController::helloWorldPayload, "/v1/hello-world/payload", Post);
      ADD_METHOD_TO(HelloController::helloWorldError, "/v1/hello-world/error", Get);
    METHOD_LIST_END

    void helloWorld(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void helloWorldPath(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, std::string &&name);
    void helloWorldQuery(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void helloWorldPayload(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
    void helloWorldError(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);
};
