#include "models/response/ApiResponse.h"
#include "ApiException.h"

void ApiException::apiException() {
  drogon::app().setExceptionHandler(
      [](const std::exception &e, const HttpRequestPtr &req,
         std::function<void(const HttpResponsePtr &)> &&callback) {
        auto resp = ApiResponse::error(drogon::k500InternalServerError,
                                       e.what(), req->getPath());
        callback(resp);
      });
}
