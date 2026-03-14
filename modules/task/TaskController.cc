#include "TaskController.h"
#include "models/response/ApiResponse.h"
#include <drogon/HttpTypes.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>

// 1. GET ALL
void TaskController::getAll(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto db = app().getDbClient("sqlite");
  db->execSqlAsync(
      "SELECT * FROM tasks",
      [callback, req](const Result &result) {
        Json::Value arr(Json::arrayValue);
        for (auto row : result) {
          Json::Value item;
          item["id"] = row["id"].as<int>();
          item["title"] = row["title"].as<std::string>();
          arr.append(item);
        }
        callback(ApiResponse::ok(req->getPath(), arr));
      },
      [callback, req](const DrogonDbException &e) {
        callback(ApiResponse::error(k500InternalServerError, e.base().what(),
                                    req->getPath()));
      });
}

void TaskController::getById(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, int taskId) {
  auto db = app().getDbClient("sqlite");
  db->execSqlAsync(
      "SELECT * FROM tasks WHERE id = ?",
      [callback, req](const Result &result) {
        Json::Value item;
        for (auto row : result) {
          item["id"] = row["id"].as<int>();
          item["title"] = row["title"].as<std::string>();
          break;
        }
        if(item.isNull()){
          callback(ApiResponse::error(drogon::k404NotFound, "not found", req->getPath()));
          return;
        }
        callback(ApiResponse::ok(req->getPath(), item));
      },
      [callback, req](const DrogonDbException &e) {
        callback(ApiResponse::error(k500InternalServerError, e.base().what(),
                                    req->getPath()));
      },
      taskId
    );
}

// 2. CREATE
void TaskController::create(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {
  auto json = req->getJsonObject();
  if (!json) {
    return callback(
        ApiResponse::error(k400BadRequest, "invalid payload", req->getPath()));
  }

  app().getDbClient("sqlite")->execSqlAsync(
      "INSERT INTO tasks (title) VALUES (?)",
      [callback, req](const Result &r) {
        callback(ApiResponse::success(drogon::k201Created, req->getPath(), Json::nullValue));
      },
      [callback, req](const DrogonDbException &e) {
        callback(ApiResponse::error(k500InternalServerError, e.base().what(),
                                    req->getPath()));
      },
      (*json)["title"].asString());
}

// 3. UPDATE
void TaskController::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, int taskId) {
  auto json = req->getJsonObject();
  if (!json) {
    return callback(
        ApiResponse::error(k400BadRequest, "invalid payload", req->getPath()));
  }

  app().getDbClient("sqlite")->execSqlAsync(
      "UPDATE tasks SET title = ? WHERE id = ?",
      [callback, req](const Result &r) {
        if (!r.affectedRows()) {
          return callback(ApiResponse::error(k404NotFound, "data not found",
                                             req->getPath()));
        }
        callback(ApiResponse::success(drogon::k200OK, req->getPath(), Json::nullValue));
      },
      [callback, req](const DrogonDbException &e) {
        callback(ApiResponse::error(k500InternalServerError, e.base().what(),
                                    req->getPath()));
      },
      (*json)["title"].asString(), taskId);
}

// 4. DELETE
void TaskController::remove(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, int taskId) {
  app().getDbClient("sqlite")->execSqlAsync(
      "DELETE FROM tasks WHERE id = ?",
      [callback, req](const Result &r) {
        callback(ApiResponse::success(drogon::k200OK, req->getPath(), Json::nullValue));
      },
      [callback, req](const DrogonDbException &e) {
        callback(ApiResponse::error(k500InternalServerError, e.base().what(),
                                    req->getPath()));
      },
      taskId);
}