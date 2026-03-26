#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;
using namespace drogon::orm;

class MBiodataController : public HttpController<MBiodataController>
{
  public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(MBiodataController::getPagination, "/v1/m-biodata", Get);
        ADD_METHOD_TO(MBiodataController::create, "/v1/m-biodata", Post);
        ADD_METHOD_TO(MBiodataController::getById, "/v1/m-biodata/{1}", Get);  
        ADD_METHOD_TO(MBiodataController::update, "/v1/m-biodata", Put);  
        ADD_METHOD_TO(MBiodataController::remove, "/v1/m-biodata/{1}", Delete);
    METHOD_LIST_END

    void getPagination(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void getById(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int id);
    void update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void remove(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int id);
};