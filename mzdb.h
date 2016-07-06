#ifndef MZDB_H
#define MZDB_H
#include "models.h"


void freeSpectrumHeader(SpectrumHeader *spectrumHeaderPtr);

extern int getSpectrumHeaders(
        sqlite3 *db,
        SpectrumHeader **spectrumHeaders,
        char **errMsg
);

int getSpectrumHeader(sqlite3_stmt *stmt,
                      SpectrumHeader *spectrumHeaderPtr,
                      char **errMsg
);

#endif
