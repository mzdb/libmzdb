#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include "lib/sqlite/sqlite3.h"
#include "util.h"

/*TODO*/

/*************** PREPARED STATEMENTS EXECUTION ******************/

int libmzdb_execute_prepared_stmt(sqlite3 *db, sqlite3_stmt *stmt, int index, void(*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void *result, char **err_msg)
{
    int stmt_step_res; //step result state
    while ( (stmt_step_res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        callback(stmt, index, size_in_bytes, result);
    }

    if (stmt_step_res != SQLITE_DONE) *err_msg = (char*)sqlite3_errmsg(db);

    return stmt_step_res;
}

int libmzdb_execute_select_stmt_for_all_types(sqlite3 *db, const char *sql, int index, void(*callback)(sqlite3_stmt*, int, long, void*), long size_in_bytes, void **result, char **err_msg)
{
    sqlite3_stmt *stmt;
    int rc /*sql result state*/, stmt_step_res /*step result state*/;

    //*err_msg = "Error (libmzdb_execute_select_stmt_for_all_types): sql prepare failed";

    if ((rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)) != SQLITE_OK)
    {
        //        printf("%d, %d", rc, SQLITE_OK);
        //        printf("%s", sqlite3_errmsg(db));
        *err_msg = (char*)sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        return rc;
    }

    stmt_step_res = libmzdb_execute_prepared_stmt(db, stmt, index, callback, size_in_bytes, result, err_msg);

    sqlite3_finalize(stmt);

    return (stmt_step_res != SQLITE_DONE) ? stmt_step_res : rc;
}

int libmzdb_execute_select_stmt(sqlite3 *db, const char *sql, int index, void (*callback)(sqlite3_stmt*, int, long, void*), void**result, char**err_msg)
{
    return libmzdb_execute_select_stmt_for_all_types(db, sql, index, callback, 1, result, err_msg);
}

int libmzdb_execute_select_first_field_stmt(sqlite3* db, const char* sql, void (*callback)(sqlite3_stmt*, int, long, void*), void*result, char** err_msg)
{
    return libmzdb_execute_select_stmt_for_all_types(db, sql, 0, callback, 1, result, err_msg);
}

/*************** CALLBACKS ******************/

void libmzdb_get_string_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result) //size in bytes unused
{
    const char * str = libmzdb_strdup((const char *) sqlite3_column_text(stmt, index));
    *((void**)result) = (void*) str;
}

void libmzdb_get_double_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    *(double*)result = sqlite3_column_double(stmt, index);
}

void libmzdb_get_float_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    double d = sqlite3_column_double(stmt, index);
    if (d > FLT_MAX)
    {
        printf("libmzdb error: floating overflow !");
        exit(EXIT_FAILURE);
    }
    *(float*) result = (float)d;
}

void libmzdb_get_int_cb(sqlite3_stmt *stmt, int index, long size_in_bytes, void *result)
{
    *(int*)result = sqlite3_column_int(stmt, index);
}


void libmzdb_get_n_int_cb(sqlite3_stmt *stmt, int index, long size_in_bytes, void *result)
{
    int* arr= (int*) malloc(sizeof(int)*index);
    for (int i= 0; i<index; ++i) arr[i]= sqlite3_column_int(stmt, i);
    result= (void *) arr;
}


/* The pointer returned by sqlite3_column_blob() points to memory
** that is owned by the statement handle (pStmt). It is only good
** until the next call to an sqlite3_XXX() function (e.g. the
** sqlite3_finalize() below) that involves the statement handle.
** So we need to make a copy of the blob into memory obtained from
** malloc() to return to the caller.
*/
// FIXME: WTF!
void libmzdb_get_blob_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    memcpy( *(void**)result, sqlite3_column_blob(stmt, index), size_in_bytes);
}

/*void get_blob_cb(sqlite3_stmt* stmt, int index, void* result)
{
    TODO: malloc a blob_result_t
    memcpy( *(void**)result, sqlite3_column_blob(stmt, index), sqlite3_column_bytes(stmt, index));
}*/

void libmzdb_get_int_64_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    *(sqlite3_int64*)result = sqlite3_column_int64(stmt, index);
}

/*************** UTILS ******************/

void libmzdb_die_on_sqlite_error(int rc, char** err_msg)
{
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Exiting program because of SQLite error: %s\n", *err_msg);
        sqlite3_free(err_msg);
        exit(EXIT_FAILURE);
    }
}

/*end TODO*/

void libmzdb_dump_blob_content(unsigned char* blob, int blob_size)
{
    printf("blob content: ");
    for (int i=0; i<blob_size; ++i)
      printf("%d ",blob[i]);

    printf("\n");
}
















///*************** PREPARED STATEMENTS EXECUTION ******************/

///*

//int executeSelectFirstFieldStmt(
//    sqlite3 *db,
//    const char *sql,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    void *cbRes,
//    char **errMsg
//    )
//{
//  return executeSelectNStmt(
//        db,
//        sql,
//        0,
//        resultCallback,
//        1,
//        cbRes,
//        errMsg
//        );
//}

////Execute the statement previously prepared
//int executeSelectStmt(
//    sqlite3 *db,
//    const char *sql,
//    int index,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    void *cbRes,
//    char **errMsg
//    )
//{
//  return executeSelectNStmt(
//        db,
//        sql,
//        index,
//        resultCallback,
//        1,
//        cbRes,
//        errMsg
//        );
//}



////Prepare and check is the required statement is OK
//int executeSelectNStmt(
//    sqlite3 *db,
//    const char *sql,
//    int index,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    long sizeInBytes,
//    void *cbRes,
//    char **errMsg
//    )
//{
//  sqlite3_stmt *stmt;

//  // Prepare SQL query
//  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

//  int stmtStepRc = 0;

//  // Check prepare is OK
//  if (rc == SQLITE_OK)
//  {
//    // Bind stmt here if required
//    //Example: rc = sqlite3_bind_int(stmt, 1, id);

//    // Execute prepared statement
//    stmtStepRc = executePreparedStmt(
//          db,
//          stmt,
//          index,
//          resultCallback,
//          sizeInBytes,
//          cbRes,
//          errMsg
//          );

//    if (stmtStepRc != SQLITE_DONE)
//    {
//      sqlite3_finalize(stmt);
//      return stmtStepRc;
//    }
//  }

//  sqlite3_finalize(stmt);

//  return rc;
//}





//// The return value should SQLITE_DONE if no error occured
//int executePreparedStmt(
//    sqlite3 *db,
//    sqlite3_stmt *stmt,
//    int index,
//    void (*resultCallback)(sqlite3_stmt*, int, long, void*),
//    long sizeInBytes,
//    void *cbRes,
//    char **errMsg
//    )
//{
//  int stmtStepRc = 0;

//  // While we obtain rows
//  while (1)
//  {
//    // Execute SQL statement
//    stmtStepRc = sqlite3_step(stmt);

//    // Check we obtain a row
//    if (stmtStepRc == SQLITE_ROW)
//    {
//      resultCallback(stmt, index, sizeInBytes, cbRes);
//      // Break if no more row and last call to step has completed normally
//    }
//    else if (stmtStepRc == SQLITE_DONE)
//    {
//      break;
//      // Else get last error message from DB
//    }
//    else
//    {
//      *errMsg = (char*)sqlite3_errmsg(db);
//      return stmtStepRc;
//    }
//  }

//  return stmtStepRc;
//}




////Statement Test On Bind That We May Need To Use Later
///*sqlite3_stmt *stmt;

//   // Prepare SQL query
//   int rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);

//   int stmtStepRc = 0;

//   // Check prepare is OK
//   if (rc == SQLITE_OK) {

//    // Bind msLevel here
//    rc = sqlite3_bind_int(stmt, 1, msLevel);

//    if (rc == SQLITE_OK) {

//        // Execute prepared statement
//        stmtStepRc = executePreparedStmt(
//            db,
//            stmt,
//            resultCallback,
//            cbRes,
//            errMsg
//        );

//        if (stmtStepRc != SQLITE_DONE) {
//            sqlite3_finalize(stmt);
//            return stmtStepRc;
//        }
//    }
//   }

//   sqlite3_finalize(stmt);

//*/

///*************** CALLBACKS ******************/

///*
//// Source: http://stackoverflow.com/questions/12984248/sqlite-in-c-c-sqlite3-exec-parameter-set-in-callback-function-is-pointing-to
//// Note: free must be called on result string by the caller
//void getStringCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  *((char**)cbRes) = strdup(sqlite3_column_text(stmt, index));
//  //strmemdup((void **)cbRes, sqlite3_column_text(stmt, index));
//}

////return the first callback from the statement (case : we're working with double type)
//void getDoubleCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  *((double*)cbRes) = sqlite3_column_double(stmt, index);
//}

////return the first callback from the statement (case : we're working with Float type)
//void getFloatCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  double d = sqlite3_column_double(stmt, index);

//  if (d > FLT_MAX)
//  {
//    printf("libmzdb error: floating overflow !");
//    exit(EXIT_FAILURE);
//  }
//  *((float*)cbRes) = (float)d;
//}

////Get the first callback from the statement (case : we're working with Int type)
//void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  int test = sqlite3_column_int(stmt, index);

//  *((int*)cbRes) = test;
//}
///*static void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  int test = sqlite3_column_int(stmt, index);

//  *((int*)cbRes) = test;
//}

//*//*

//void getNIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  int i;
//  int *test;

//  test = malloc(index * sizeof(int));
//  for (i = 0; i < index; i++)
//  {
//    test[i] = sqlite3_column_int(stmt, i);
//    *((int*)cbRes) = test[i];
//  }
//}

////return the first callback from the statement (case : we're working with blob type)
//void getBlobCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  memcpy(*((void**)cbRes), sqlite3_column_blob(stmt, index), sizeInBytes);
//}

////return the first callback from the statement (case : we're working with int64 type)
//static void getInt64Cb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
//{
//  *((sqlite3_int64*)cbRes) = sqlite3_column_int64(stmt, index);
//}

///*static int sqliteQueryPrintfCallback(void *data, int argc, char **argv, char **azColName) {
//    int i;
//    fprintf(stderr, "%s: ", (const char*)data);
//    for(i=0; i<argc; i++){
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }
//    printf("\n");
//    return 0;
//   }
//*/

///*************** UTILS ******************/

///*
//void dieOnSqliteError(int rc, char *errMsg)
//{
//  if (rc != SQLITE_OK)
//  {
//    fprintf(stderr, "Exiting program because of SQLite error: %s\n", errMsg);
//    sqlite3_free(errMsg);
//    exit(EXIT_FAILURE);
//  }
//}
//*/
