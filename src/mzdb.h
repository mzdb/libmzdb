#ifndef MZDB_H
#define MZDB_H
#include "models.h"

//extern static int MZDB_VERBOSE_MODE;

int openMzDbFile(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);

int closeMzDbFile(sqlite3* ppDb);

void freeSpectrumHeader(SpectrumHeader *spectrumHeaderPtr);

int getSpectrumHeader(sqlite3_stmt *stmt,
                      SpectrumHeader *spectrumHeaderPtr,
                      char **errMsg
);

int getSpectrumHeaders(
        sqlite3 *db,
        SpectrumHeader **spectrumHeaders,
        char **errMsg
);


#endif
