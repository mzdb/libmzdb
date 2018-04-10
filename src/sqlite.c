#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include "lib/sqlite/sqlite3.h"

/*************** PREPARED STATEMENTS EXECUTION ******************/

// The return value should SQLITE_DONE if no error occured
int executePreparedStmt(
  sqlite3 *db,
  sqlite3_stmt *stmt,
  int index,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  long sizeInBytes,
  void *cbRes,
  char **errMsg
  )
{
  int stmtStepRc = 0;

  // While we obtain rows
  while (1)
    {
      // Execute SQL statement
      stmtStepRc = sqlite3_step(stmt);

      // Check we obtain a row
      if (stmtStepRc == SQLITE_ROW)
        {
          resultCallback(stmt, index, sizeInBytes, cbRes);
          // Break if no more row and last call to step has completed normally
        }
      else if (stmtStepRc == SQLITE_DONE)
        {
          break;
          // Else get last error message from DB
        }
      else
        {
          *errMsg = (char*)sqlite3_errmsg(db);
          return stmtStepRc;
        }
    }

  return stmtStepRc;
}

//Prepare and check is the required statement is OK
int executeSelectNStmt(
  sqlite3 *db,
  const char *sql,
  int index,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  long sizeInBytes,
  void *cbRes,
  char **errMsg
  )
{
  sqlite3_stmt *stmt;

  // Prepare SQL query
  int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  int stmtStepRc = 0;

  // Check prepare is OK
  if (rc == SQLITE_OK)
    {
      // Bind stmt here if required
      //Example: rc = sqlite3_bind_int(stmt, 1, id);

      // Execute prepared statement
      stmtStepRc = executePreparedStmt(
        db,
        stmt,
        index,
        resultCallback,
        sizeInBytes,
        cbRes,
        errMsg
        );

      if (stmtStepRc != SQLITE_DONE)
        {
          sqlite3_finalize(stmt);
          return stmtStepRc;
        }
    }

  sqlite3_finalize(stmt);

  return rc;
}

//Statement Test On Bind That We May Need To Use Later
/*sqlite3_stmt *stmt;

   // Prepare SQL query
   int rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);

   int stmtStepRc = 0;

   // Check prepare is OK
   if (rc == SQLITE_OK) {

    // Bind msLevel here
    rc = sqlite3_bind_int(stmt, 1, msLevel);

    if (rc == SQLITE_OK) {

        // Execute prepared statement
        stmtStepRc = executePreparedStmt(
            db,
            stmt,
            resultCallback,
            cbRes,
            errMsg
        );

        if (stmtStepRc != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return stmtStepRc;
        }
    }
   }

   sqlite3_finalize(stmt);*/

//Execute the statement previously prepared
int executeSelectStmt(
  sqlite3 *db,
  const char *sql,
  int index,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  void *cbRes,
  char **errMsg
  )
{
  return executeSelectNStmt(
    db,
    sql,
    index,
    resultCallback,
    1,
    cbRes,
    errMsg
    );
}

int executeSelectFirstFieldStmt(
  sqlite3 *db,
  const char *sql,
  void (*resultCallback)(sqlite3_stmt*, int, long, void*),
  void *cbRes,
  char **errMsg
  )
{
  return executeSelectNStmt(
    db,
    sql,
    0,
    resultCallback,
    1,
    cbRes,
    errMsg
    );
}

/*************** CALLBACKS ******************/

// Source: http://stackoverflow.com/questions/12984248/sqlite-in-c-c-sqlite3-exec-parameter-set-in-callback-function-is-pointing-to
// Note: free must be called on result string by the caller
void getStringCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  *((char**)cbRes) = strdup(sqlite3_column_text(stmt, index));
  //strmemdup((void **)cbRes, sqlite3_column_text(stmt, index));
}

//return the first callback from the statement (case : we're working with double type)
void getDoubleCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  *((double*)cbRes) = sqlite3_column_double(stmt, index);
}

//return the first callback from the statement (case : we're working with Float type)
void getFloatCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  double d = sqlite3_column_double(stmt, index);

  if (d > FLT_MAX)
    {
      printf("libmzdb error: floating overflow !");
      exit(EXIT_FAILURE);
    }
  *((float*)cbRes) = (float)d;
}

//Get the first callback from the statement (case : we're working with Int type)
void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  int test = sqlite3_column_int(stmt, index);

  *((int*)cbRes) = test;
}
/*static void getIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  int test = sqlite3_column_int(stmt, index);

  *((int*)cbRes) = test;
}*/

void getNIntCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  int i;
  int *test;

  test = malloc(index * sizeof(int));
  for (i = 0; i < index; i++)
    {
      test[i] = sqlite3_column_int(stmt, i);
      *((int*)cbRes) = test[i];
    }
}

//return the first callback from the statement (case : we're working with blob type)
void getBlobCb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  memcpy(*((void**)cbRes), sqlite3_column_blob(stmt, index), sizeInBytes);
}

//return the first callback from the statement (case : we're working with int64 type)
static void getInt64Cb(sqlite3_stmt* stmt, int index, long sizeInBytes, void* cbRes)
{
  *((sqlite3_int64*)cbRes) = sqlite3_column_int64(stmt, index);
}

/*static int sqliteQueryPrintfCallback(void *data, int argc, char **argv, char **azColName) {
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
   }*/


/*************** UTILS ******************/

void dieOnSqliteError(int rc, char *errMsg)
{
  if (rc != SQLITE_OK)
    {
      fprintf(stderr, "Exiting program because of SQLite error: %s\n", errMsg);
      sqlite3_free(errMsg);
      exit(EXIT_FAILURE);
    }
}
