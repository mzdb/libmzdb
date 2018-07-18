#ifndef SQLITE_H
#define SQLITE_H

#include "lib/sqlite/sqlite3.h"

/**
 * @brief execute_prepared_stmt execute for each column the callback
 * @param db the DB
 * @param stmt the stmt
 * @param index the index of the column
 * @param size_in_bytes the size
 * @param result the result
 * @param err_msg the error message
 * @return SQLITE_OK if there is no error
 */
int libmzdb_execute_prepared_stmt( sqlite3 *db, sqlite3_stmt *stmt, int index, void (*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void *result, char **err_msg);

/**
 * @brief execute_select_n_stmt
 * @param db
 * @param sql
 * @param callback
 * @param index
 * @param size_in_bytes
 * @param result
 * @param err_msg
 * @return
 */
int libmzdb_execute_select_stmt_for_all_types(sqlite3 *db, const char *sql, int index, void (*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void** result, char** err_msg);

/**
 * @brief execute_select_stmt execute the sql query and get the result of the index column
 * @param db the DB
 * @param sql the query
 * @param index the index of the column
 * @param result the result
 * @param err_msg the error message
 * @return SQLITE_OK if there is no error
 */
int libmzdb_execute_select_stmt(sqlite3 *db, const char *sql, int index, void (*callback)(sqlite3_stmt*, int, long, void*), void**result, char**err_msg);

/**
 * @brief execute_select_first_field_stmt execute the sql and get the result of the 1st column
 * @param db the DB
 * @param sql the query
 * @param result the result
 * @param err_msg the error message
 * @return SQLITE_OK if there is no error
*/
int libmzdb_execute_select_first_field_stmt(sqlite3* db, const char* sql, void (*callback)(sqlite3_stmt*, int, long, void*), void*result, char** err_msg);

/*************** CALLBACKS ******************/

/**
 * @brief get_string_cb get the str at the "index" column
 * @param stmt the stmt
 * @param index the index of the column
 * @param result the result
 */
void libmzdb_get_string_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result); //size in bytes unused

/**
 * @brief get_double_cb get the double at the "index" column
 * @param stmt the stmt
 * @param index the index of the column
 * @param result the result
 */
void libmzdb_get_double_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/**
 * @brief get_float_cb get the float at the "index" column
 * @param stmt the stmt
 * @param index the column index
 * @param result the result
 */
void libmzdb_get_float_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/**
 * @brief get_int_cb get the int at the "index" column
 * @param stmt the stmt
 * @param index the index of the column
 * @param result the result
 */
void libmzdb_get_int_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/**
 * @brief get_n_int_cb get an array of int of each column in [0, index]
 * @param stmt the stmt
 * @param index the index of the last column in the range
 * @param result the result
 */
void libmzdb_get_n_int_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

typedef struct libmzdb_blob_result {
    unsigned char *blob;
    long blob_size;
} libmzdb_blob_result_t;

/**
 * @brief get_blob_cb get the blob at the "index" column
 * @param stmt the stmt
 * @param index the column index
 * @param size_in_bytes the blob size
 * @param result the result
 */
void libmzdb_get_blob_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);


/**
 * @brief get_int_64_cb get the int64 at the "index" column
 * @param stmt the stmt
 * @param index the column index
 * @param result the result
 */
void libmzdb_get_int_64_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result);

/*************** UTILS ******************/

/**
 * @brief die_on_sql_error exit the program if rc != SQLITE_OK
 * @param rc the sql result state
 * @param err_msg the error message to print
 */
void libmzdb_die_on_sqlite_error(int rc, char** err_msg);


void libmzdb_dump_blob_content(unsigned char* blob, int blob_size);







//typedef int (*sqlite3_stmt_cb)(sqlite3_stmt*);
//typedef void (*sqlite3_stmt_res_cb)(sqlite3_stmt*, void*);

/*
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
*/

#endif /* SQLITE_H */
