#pragma once
#include <drogon/HttpController.h>
#include <drogon/HttpTypes.h>

using namespace drogon;
using namespace drogon::orm;

class TaskController : public HttpController<TaskController>
{
  public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(TaskController::getAll, "/tasks", Get);
        ADD_METHOD_TO(TaskController::create, "/tasks", Post);
        ADD_METHOD_TO(TaskController::getById, "/tasks/{1}", Get);  
        ADD_METHOD_TO(TaskController::update, "/tasks/{1}", Put);  
        ADD_METHOD_TO(TaskController::remove, "/tasks/{1}", Delete);
    METHOD_LIST_END

    void getAll(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void getById(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int taskId);
    void update(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int taskId);
    void remove(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, int taskId);
};