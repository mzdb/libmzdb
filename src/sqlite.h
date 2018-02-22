#ifndef SQLITE_H
#define SQLITE_H

#include "lib/sqlite/sqlite3.h"

//typedef int (*sqlite3_stmt_cb)(sqlite3_stmt*);
//typedef void (*sqlite3_stmt_res_cb)(sqlite3_stmt*, void*);

int executePreparedStmt(
  sqlite3 *db,
  sqlite3_stmt *stmt,
  int index,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  long sizeInBytes,
  void *cbRes,
  char **errMsg
  );

int executeSelectNStmt(
  sqlite3 *db,
  const char *sql,
  int index,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  long sizeInBytes,
  void *cbRes,
  char **errMsg
  );

int executeSelectStmt(
  sqlite3 *db,
  const char *sql,
  int index,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  void *cbRes,
  char **errMsg
  );

void getStringCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);
void getDoubleCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);
void getFloatCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);
void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);
void getNIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);
void getBlobCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);
void getInt64Cb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes);

void dieOnSqliteError(int rc, char *errMsg);

#endif /* SQLITE_H */
