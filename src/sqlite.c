#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include "lib/sqlite/sqlite3.h"
#include "util.h"

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
    if (d > FLT_MAX) exit(EXIT_FAILURE);
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
void libmzdb_get_blob_cb(sqlite3_stmt* stmt, int index, long size_in_bytes, void* result)
{
    memcpy( *(void**)result, sqlite3_column_blob(stmt, index), size_in_bytes);
}

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

void libmzdb_dump_blob_content(unsigned char* blob, int blob_size)
{
    printf("blob content: ");
    for (int i=0; i<blob_size; ++i)
      printf("%d ",blob[i]);

    printf("\n");
}
