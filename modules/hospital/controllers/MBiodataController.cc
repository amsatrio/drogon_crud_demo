#include "MBiodataController.h"
#include "models/response/ApiResponse.h"
#include "modules/hospital/models/MBiodata.h"
#include <drogon/HttpTypes.h>
#include <drogon/orm/Exception.h>
#include <json/value.h>
#include <trantor/utils/Date.h>
#include <utility>

void MBiodataController::getAll(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) {

  auto dbClient = app().getDbClient("postgresql");

  dbClient->execSqlAsync(
      "SELECT * FROM public.m_biodata",
      [callback, req](const drogon::orm::Result &r) {
        Json::Value data(Json::arrayValue);
        for (int i = 0; i < r.size(); i++) {
          drogon_model::db_hospital::MBiodata b(r[i]);
          data.append(std::move(b.toJson()));
        }
        callback(ApiResponse::success(drogon::k200OK, req->getPath(), data));
      },
      [callback, req](const DrogonDbException &e) {
        LOG_ERROR << e.base().what();
        callback(ApiResponse::error(drogon::k500InternalServerError,
                                    e.base().what(), req->getPath()));
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
      id);
}

void MBiodataController::create(
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