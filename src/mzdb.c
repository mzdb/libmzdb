#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include "lib/sqlite/sqlite3.h"
#include "queries.h"
#include "mzdb.h"

int openMzDbFile(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
) {
  return sqlite3_open(filename, ppDb);
}

int closeMzDbFile(sqlite3* ppDb) {
  return sqlite3_close(ppDb);
}

void freeSpectrumHeader(SpectrumHeader *spectrumHeaderPtr)
{
  if (spectrumHeaderPtr == NULL)
    return;

  if (spectrumHeaderPtr->title != NULL)
    free(spectrumHeaderPtr->title);

  if (spectrumHeaderPtr->activationType != NULL)
    free(spectrumHeaderPtr->activationType);

  if (spectrumHeaderPtr->paramTree != NULL)
    free(spectrumHeaderPtr->paramTree);

  if (spectrumHeaderPtr->scanList != NULL)
    free(spectrumHeaderPtr->scanList);

  if (spectrumHeaderPtr->precursorList != NULL)
    free(spectrumHeaderPtr->precursorList);

  if (spectrumHeaderPtr->productList != NULL)
    free(spectrumHeaderPtr->productList);
}

/* get one spectrum header from a n SQL statement */
int getSpectrumHeader(sqlite3_stmt *stmt,
                      SpectrumHeader *spectrumHeaderPtr,
                      char **errMsg
                      )
{
  if (spectrumHeaderPtr == NULL)
    {
      return SQLITE_ERROR;
    }
  spectrumHeaderPtr->id = sqlite3_column_int(stmt, 0);
  spectrumHeaderPtr->initialId = sqlite3_column_int(stmt, 1);
  spectrumHeaderPtr->title = _strdup(sqlite3_column_text(stmt, 2));
  spectrumHeaderPtr->cycle = sqlite3_column_int(stmt, 3);
  spectrumHeaderPtr->time = (float)sqlite3_column_double(stmt, 4);
  spectrumHeaderPtr->msLevel = sqlite3_column_int(stmt, 5);
  spectrumHeaderPtr->activationType = _strdup(sqlite3_column_text(stmt, 6));
  spectrumHeaderPtr->tic = (float)sqlite3_column_double(stmt, 7);
  spectrumHeaderPtr->basePeakMz = sqlite3_column_double(stmt, 8);
  spectrumHeaderPtr->basePeakIntensity = (float)sqlite3_column_double(stmt, 9);
  spectrumHeaderPtr->precursorMz = sqlite3_column_double(stmt, 10);
  spectrumHeaderPtr->precursorCharge = sqlite3_column_int(stmt, 11);
  spectrumHeaderPtr->peaksCount = sqlite3_column_int(stmt, 12);
  spectrumHeaderPtr->paramTree = _strdup(sqlite3_column_text(stmt, 13));
  spectrumHeaderPtr->scanList = _strdup(sqlite3_column_text(stmt, 14));
  spectrumHeaderPtr->precursorList = _strdup(sqlite3_column_text(stmt, 15));
  spectrumHeaderPtr->productList = _strdup(sqlite3_column_text(stmt, 16));
  spectrumHeaderPtr->sharedParamTreeID = sqlite3_column_int(stmt, 17);
  spectrumHeaderPtr->instrumentConfigurationID = sqlite3_column_int(stmt, 18);
  spectrumHeaderPtr->sourceFileID = sqlite3_column_int(stmt, 19);
  spectrumHeaderPtr->runID = sqlite3_column_int(stmt, 20);
  spectrumHeaderPtr->dataProcessingID = sqlite3_column_int(stmt, 21);
  spectrumHeaderPtr->dataEncodingID = sqlite3_column_int(stmt, 22);
  spectrumHeaderPtr->bbFirstSpectrumId = sqlite3_column_int(stmt, 23);

  printf("Spectrum ID : %d\n", spectrumHeaderPtr->id);
  printf("Spectrum initial ID : %d\n", spectrumHeaderPtr->initialId);
  printf("Spectrum Title : %s\n", spectrumHeaderPtr->title);
  printf("Spectrum cycle : %d\n", spectrumHeaderPtr->cycle);
  printf("Spectrum time : %f\n", spectrumHeaderPtr->time);
  printf("Spectrum Ms Level : %d\n", spectrumHeaderPtr->msLevel);
  printf("Spectrum Activation Type : %s\n", spectrumHeaderPtr->activationType);
  printf("Spectrum TIC : %f\n", spectrumHeaderPtr->tic);
  printf("Spectrum Base Peak Mz : %f\n", spectrumHeaderPtr->basePeakMz);
  printf("Spectrum Base Peak Intensity : %f\n", spectrumHeaderPtr->basePeakIntensity);
  printf("Spectrum Main Precursor Mz : %f\n", spectrumHeaderPtr->precursorMz);
  printf("Spectrum Main Precursor Charge : %d\n", spectrumHeaderPtr->precursorCharge);
  printf("Spectrum Data Points Count : %d\n", spectrumHeaderPtr->peaksCount);
  printf("Spectrum Param Tree : %s\n", spectrumHeaderPtr->paramTree);
  printf("Spectrum Scan List : %s\n", spectrumHeaderPtr->scanList);
  printf("Spectrum Precursor List : %s\n", spectrumHeaderPtr->precursorList);
  printf("Spectrum Product List : %s\n", spectrumHeaderPtr->productList);
  printf("Spectrum Shared Param Tree ID : %d\n", spectrumHeaderPtr->sharedParamTreeID);
  printf("Spectrum Instrument Configuration ID : %d\n", spectrumHeaderPtr->instrumentConfigurationID);
  printf("Spectrum Source File ID : %d\n", spectrumHeaderPtr->sourceFileID);
  printf("Spectrum Run ID : %d\n", spectrumHeaderPtr->runID);
  printf("Spectrum Data Processing ID : %d\n", spectrumHeaderPtr->dataProcessingID);
  printf("Spectrum Data Encoding ID : %d\n", spectrumHeaderPtr->dataEncodingID);
  printf("Spectrum Bounding Box First Spectrum ID : %d\n\n", spectrumHeaderPtr->bbFirstSpectrumId);

  return SQLITE_OK;
}

/* get all spectrum headers from a database */
int getSpectrumHeaders(
  sqlite3 *db,
  SpectrumHeader **spectrumHeaders,
  char **errMsg

  )
{
  sqlite3_stmt *stmt;   // sql statement
  SpectrumHeader *spectrumHeader = NULL;   // current spectrum header to treat
  int rc;   // sql result state
  int stmtStepRc;   // step result state
  int spectrumIndex;
  int spectrumCount;

  const char *sqlString = "SELECT * FROM spectrum";

  //printf("SQL: %s \n\n",sqlString);

  sqlite3_prepare_v2(db, "SELECT count(id) FROM spectrum", -1, &stmt, NULL);
  stmtStepRc = sqlite3_step(stmt);
  spectrumCount = sqlite3_column_int(stmt, 0);
  sqlite3_finalize(stmt);

  // Prepare SQL query
  rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);

  // Check prepare is OK
  if (rc != SQLITE_OK)
    return rc;
/*
    //Count rows -> faster with prepare/step/finalize
    spectrumCount = 0;
    stmtStepRc = sqlite3_step(stmt);
    while ( stmtStepRc == SQLITE_ROW ) {

        stmtStepRc = sqlite3_step(stmt);
        spectrumCount++;

    }

    sqlite3_reset(stmt);*/




  //printf("Number of counts : %d\n", spectrumCount);
  *spectrumHeaders = (SpectrumHeader*)malloc(spectrumCount * sizeof(SpectrumHeader));

  // loop over rows
  spectrumIndex = 0;
  stmtStepRc = sqlite3_step(stmt);
  printf("number of values : %d\n", sqlite3_data_count(stmt));
  while (stmtStepRc == SQLITE_ROW)
    {
      spectrumHeader = (*spectrumHeaders) + spectrumIndex;
      // treat one row (one spectrum)
      getSpectrumHeader(stmt, spectrumHeader, errMsg);

      stmtStepRc = sqlite3_step(stmt);
      spectrumIndex++;
    }

  if (stmtStepRc != SQLITE_DONE)
    {
      return rc;
    }




  /* int rc = sqlite3_blob_open(db, "main", "spectrum", "id", index, 0, &blobHandle);
     if(rc != SQLITE_OK)
     {
   * errMsg = "Error occured : Opening Blob 'data' in table 'bounding_box'";
       return rc;
     }
     blobLength = sqlite3_blob_bytes(blobHandle);
     blob = (byte*)calloc(blobLength,sizeof(byte));

     rc = sqlite3_blob_read(blobHandle, blob,blobLength, 0);
     intArray = (int*)blob;
     printf("ID : %d\n", intArray[0]);
     printf("Initial ID : %d\n", intArray[1]);
     sqlite3_blob_close(blobHandle);*/
  return rc;
}
/*public SpectrumHeader extract(SQLiteRecord record) throws SQLiteException {

                SQLiteStatement stmt = record.getStatement();

                // long nano = System.nanoTime();
                int msLevel = stmt.columnInt(SpectrumHeaderColIdx.msLevel);

                double precursorMz = 0.0;
                int precursorCharge = 0;
                if (msLevel == 2) {
                    precursorMz = stmt.columnDouble(SpectrumHeaderColIdx.mainPrecursorMz);
                    precursorCharge = stmt.columnInt(SpectrumHeaderColIdx.mainPrecursorCharge);
                }

                int bbFirstSpectrumId = stmt.columnInt(SpectrumHeaderColIdx.bbFirstSpectrumId);

                DataEncoding dataEnc = _dataEncodingReader.getDataEncoding(stmt.columnInt(SpectrumHeaderColIdx.dataEncodingId), connection);

                boolean isHighRes = dataEnc.getPeakEncoding() == PeakEncoding.LOW_RES_PEAK ? false : true;

                SpectrumHeader sh = new SpectrumHeader(
                    stmt.columnLong(SpectrumHeaderColIdx.id),
                    stmt.columnInt(SpectrumHeaderColIdx.initialId),
                    stmt.columnInt(SpectrumHeaderColIdx.cycleCol),
                    (float) stmt.columnDouble(SpectrumHeaderColIdx.time),
                    msLevel,
                    stmt.columnInt(SpectrumHeaderColIdx.dataPointsCount),
                    isHighRes,
                    (float) stmt.columnDouble(SpectrumHeaderColIdx.tic),
                    stmt.columnDouble(SpectrumHeaderColIdx.basePeakMz),
                    (float) stmt.columnDouble(SpectrumHeaderColIdx.basePeakIntensity),
                    precursorMz,
                    precursorCharge,
                    bbFirstSpectrumId
                );

                if (SpectrumHeaderReader.loadParamTree) {
                    sh.setParamTree( ParamTreeParser.parseParamTree(stmt.columnString(SpectrumHeaderColIdx.paramTree)) );
                }
                if (SpectrumHeaderReader.loadScanList) {
                    sh.setScanList(ParamTreeParser.parseScanList(stmt.columnString(SpectrumHeaderColIdx.scanList)));
                }
                if (SpectrumHeaderReader.loadPrecursorList && msLevel > 1) {
                    sh.setPrecursor(ParamTreeParser.parsePrecursor(stmt.columnString(SpectrumHeaderColIdx.precursorList)));
                }

                // System.out.println( (double) (System.nanoTime() - nano) / 1e3 );

                // sh.setParamTree(paramTree);

                return sh;
            }

        };

        return _spectrumHeaderExtractor;
    }*/
