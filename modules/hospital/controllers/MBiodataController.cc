#include "MBiodataController.h"
#include "models/response/ApiResponse.h"
#include "modules/hospital/models/MBiodata.h"
#include <cstdint>
#include <drogon/HttpTypes.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>
#include <trantor/utils/Date.h>
#include <utility>

void MBiodataController::getPagination(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  int page = req->getParameter("page").empty()
                 ? 0
                 : std::stoi(req->getParameter("page"));
  int size = req->getParameter("size").empty()
                 ? 10
                 : std::stoi(req->getParameter("size"));
  if (page <= 0)
    page = 0;
  if (size <= 0)
    size = 10;
  int offset = page * size;

  auto sort = req->getParameter("sort");
  auto filter = req->getParameter("filter");
  auto search = req->getParameter("search");

  auto dbClient = app().getDbClient("postgresql");

  std::string count_sql = "SELECT COUNT(*) FROM public.m_biodata";
  std::string main_sql = "SELECT * FROM public.m_biodata";

  dbClient->execSqlAsync(
      count_sql,
      [callback, req, page, size, offset, dbClient, main_sql](const drogon::orm::Result &countRes) {
        
        long long totalElements = countRes[0][0].as<long long>();
        int totalPages = (totalElements + size - 1) / size;

        dbClient->execSqlAsync(
            main_sql + " LIMIT $1 OFFSET $2",
            [callback, req, totalElements, totalPages, size](const drogon::orm::Result &r) {
              
              Json::Value contents(Json::arrayValue);
              for (const auto &row : r) {
                drogon_model::db_hospital::MBiodata b(row);
                contents.append(b.toJson());
              }

              Json::Value response;
              response["totalElements"] = (Json::Int64)totalElements;
              response["totalPages"] = totalPages;
              response["numberOfElements"] = (int)r.size();
              response["contents"] = std::move(contents);

              callback(ApiResponse::success(drogon::k200OK, req->getPath(), response));
            },
            [callback, req](const DrogonDbException &e) {
              LOG_ERROR << e.base().what();
              callback(ApiResponse::error(drogon::k500InternalServerError, e.base().what(), req->getPath()));
            },
            (long long)size, (long long)offset);
      },
      [callback, req](const DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        callback(ApiResponse::error(drogon::k500InternalServerError, e.base().what(), req->getPath()));
      });
}

void MBiodataController::getById(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, int id) {

  auto dbClient = app().getDbClient("postgresql");

  dbClient->execSqlAsync(
      "SELECT * FROM public.m_biodata WHERE id = $1 LIMIT 1",
      [callback, req](const drogon::orm::Result &r) {
        if (r.size() > 0) {
          drogon_model::db_hospital::MBiodata b(r[0]);
          callback(
              ApiResponse::success(drogon::k200OK, req->getPath(), b.toJson()));
        } else {
          callback(ApiResponse::error(drogon::k404NotFound, "Record not found",
                                      req->getPath()));
        }
      },
      [callback, req](const DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        callback(ApiResponse::error(drogon::k500InternalServerError,
                                    e.base().what(), req->getPath()));
      },
      (long long)id);
}

void MBiodataController::create(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json = req->getJsonObject();
  if (!json) {
    callback(ApiResponse::error(drogon::k400BadRequest, "Invalid payload",
                                req->getPath()));
    return;
  }

  auto dbClient = app().getDbClient("postgresql");
  auto id = (*json)["id"].asInt64();

  dbClient->execSqlAsync(
      "SELECT COUNT(*) FROM public.m_biodata WHERE id = $1",
      [callback, req, json, dbClient](const drogon::orm::Result &r) {
        if (r[0][0].as<int64_t>() > 0) {
          callback(ApiResponse::error(drogon::k400BadRequest,
                                      "Data already exists", req->getPath()));
          return;
        }

        Mapper<drogon_model::db_hospital::MBiodata> mapper(dbClient);
        drogon_model::db_hospital::MBiodata data(*json);
        data.setCreatedOn(trantor::Date::now());
        data.setCreatedBy(0);
        data.setIsDelete(false);

        mapper.insert(
            data,
            [callback, req](const drogon_model::db_hospital::MBiodata &b) {
              callback(ApiResponse::success(drogon::k201Created, req->getPath(),
                                            b.toJson()));
            },
            [callback, req](const DrogonDbException &e) {
              LOG_ERROR << e.base().what();
              callback(ApiResponse::error(drogon::k500InternalServerError,
                                          e.base().what(), req->getPath()));
            });
      },
      [callback, req](const DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        callback(ApiResponse::error(drogon::k500InternalServerError,
                                    e.base().what(), req->getPath()));
      },
      (long long)id);
}

void MBiodataController::update(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto json = req->getJsonObject();
  if (!json) {
    callback(ApiResponse::error(drogon::k400BadRequest, "invalid payload",
                                req->getPath()));
    return;
  }

  Mapper<drogon_model::db_hospital::MBiodata> mapper(
      app().getDbClient("postgresql"));

  mapper.findByPrimaryKey(
      (*json)["id"].asUInt64(),
      [callback, json, mapper,
       req](drogon_model::db_hospital::MBiodata b) mutable {
        if ((*json).isMember("fullname"))
          b.setFullname((*json)["fullname"].asString());
        if ((*json).isMember("mobilePhone"))
          b.setMobilePhone((*json)["mobilePhone"].asString());
        if ((*json).isMember("isDelete")) {
          if ((*json)["isDelete"].asBool()) {
            b.setDeletedOn(trantor::Date::now());
          } else {
            b.setDeletedByToNull();
          }
          b.setIsDelete((*json)["isDelete"].asBool());
        }

        b.setModifiedOn(trantor::Date::now());
        b.setModifiedBy(0);

        mapper.update(
            b,
            [callback, req](const size_t count) {
              callback(ApiResponse::success(drogon::k200OK, req->getPath(),
                                            Json::nullValue));
            },
            [callback, req](const DrogonDbException &e) {
              LOG_ERROR << e.base().what();
              callback(ApiResponse::error(drogon::k500InternalServerError,
                                          e.base().what(), req->getPath()));
            });
      },
      [callback, req](const DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        callback(ApiResponse::error(drogon::k500InternalServerError,
                                    e.base().what(), req->getPath()));
      });
}

void MBiodataController::remove(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback, int id) {
  Mapper<drogon_model::db_hospital::MBiodata> mapper(
      app().getDbClient("postgresql"));
  mapper.deleteByPrimaryKey(
      id,
      [callback, req](const size_t count) {
        callback(ApiResponse::success(drogon::k200OK, req->getPath(),
                                      Json::nullValue));
      },
      [callback, req](const DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        callback(ApiResponse::error(drogon::k500InternalServerError,
                                    e.base().what(), req->getPath()));
      });
}