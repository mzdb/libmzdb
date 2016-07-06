#define _CRT_SECURE_NO_WARNINGS // TODO: comment the define

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include "lib/sqlite/sqlite3.h"
#include "util.h"
#include "xml.h"
#include "queries.h"
#include "models.h"
#include "sqlite.h"

//Return the whole statement to find the model Version
int getModelVersion(
  sqlite3 *db,               /* The database on which the SQL executes */
  char **modelVersion,       /* The model version retrieved from the mzDB */
  char **errMsg        /* Write error messages here */
  )
{
  const char *sqlString = "SELECT version FROM mzdb LIMIT 1";

  //sqlite3_exec(db, sqlString, callback, pArg, pzErrMsg);

  printf("SQL: %s \n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getStringCb, modelVersion, errMsg);
}

//Only return the modelVersion from the previous statement
char *getModelVersionOrDie(
  sqlite3 *db                      /* The database on which the SQL executes */
  )
{
  char *modelVersion;
  char *errMsg = NULL;

  int rc = getModelVersion(db, &modelVersion, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return modelVersion;
}

//Stock the Param Tree values from Chromatogram into paramTreeChro
int getParamTreeChromatogram(
  sqlite3 *db,
  char **paramTreeChro,
  char **errMsg
  )
{
  const char *sqlString = "SELECT param_tree FROM chromatogram";

  printf("SQL: %s \n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getStringCb, paramTreeChro, errMsg);
}

//Only return the paramTreeChro from the previous statement
char *getParamTreeChromatogramOrDie(
  sqlite3 *db
  )
{
  char *paramTreeChro;
  char *errMsg = NULL;

  int rc = getParamTreeChromatogram(db, &paramTreeChro, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return paramTreeChro;
}

//Stock the Param Tree values from Spectrum into paramTreeSpectrum
int getParamTreeSpectrum(
  sqlite3 *db,
  int paramTreeId,
  char **paramTreeSpectrum,
  char **errMsg
  )
{
  const char *sqlString = sqlite3_mprintf("SELECT param_tree FROM spectrum WHERE id = %d", paramTreeId);

  printf("SQL: %s \n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getStringCb, paramTreeSpectrum, errMsg);
}

//Only return the paramTreeSpectrum from the previous statement
char *getParamTreeSpectrumOrDie(
  sqlite3 *db,
  int paramTreeId
  )
{
  char *paramTreeSpectrum;
  char *errMsg = NULL;

  int rc = getParamTreeSpectrum(db, paramTreeId, &paramTreeSpectrum, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return paramTreeSpectrum;
}

//stock the Param Tree values from Mzdb into paramTreeMzdb
int getParamTreeMzdb(
  sqlite3 *db,
  MzdbParamTree *paramTreeMzdb,
  char **errMsg
  )
{
  char *paramTree = NULL;
  int rc;
  nodecPtr xml = NULL;
  const char *sqlString = "SELECT param_tree FROM mzdb";

  //    printf("SQL: %s \n", sqlString);

  rc = executeSelectStmt(db, sqlString, 0, &getStringCb, &paramTree, errMsg);
  if (rc != SQLITE_OK)
    {
      *errMsg = _strdup("An error occured trying to return param tree\n");
      return rc;
    }

  //testXmlBare(paramTree);
  xml = xmlParse(paramTree);
  memset(paramTreeMzdb, 0, sizeof(MzdbParamTree));
  xmlBrowse(xml, xmlBrowseCallbackMzdbParamTree, paramTreeMzdb);
  printf("Test A Param \n");
  fflush(stdout);
  return rc;
}

//Only return the paramTreeMzdb values from the previous statement
char *getParamTreeMzdbOrDie(
  sqlite3 *db
  )
{
  char *paramTreeMzdb;
  char *errMsg = NULL;

  int rc = getParamTreeMzdb(db, &paramTreeMzdb, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return paramTreeMzdb;
}

//Return the whole statement to find the PwizMzDbVersion
int getPwizMzDbVersion(
  sqlite3 *db,
  char **mzDbVersion,
  char **errMsg
  )
{
  const char *sqlString = "SELECT version FROM software WHERE name LIKE '%mzDB'";

  printf("SQL: %s \n\n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getStringCb, mzDbVersion, errMsg);
}

//Only return the PwizMzDbVersion from the previous statement
char *getPwizMzDbVersionOrDie(
  sqlite3 *db
  )
{
  char *pwizMzDbVersion;
  char *errMsg = NULL;

  int rc = getPwizMzDbVersion(db, &pwizMzDbVersion, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return pwizMzDbVersion;
}

//Return the whole statement to find the last Time from spectrum table
int getLastTime(
  sqlite3 *db,         /* The database on which the SQL executes */
  double *lastTime,
  char **errMsg        /* Write error messages here */
  )
{
  // Retrieve the number of spectra
  const char *sqlString = "SELECT time FROM spectrum ORDER BY id DESC LIMIT 1";

  printf("SQL: %s \n\n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getDoubleCb, lastTime, errMsg);
}

//Only return the lastTime from the previous statement
double getLastTimeOrDie(
  sqlite3 *db
  )
{
  double lastTime;
  char *errMsg = NULL;

  int rc = getLastTime(db, &lastTime, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return lastTime;
}

//Return the whole statement to find the MaxMsLevel from run_slice table
int getMaxMsLevel(
  sqlite3 *db,
  int *maxMsLevel,
  char **errMsg
  )
{
  const char *sqlString = "SELECT max(ms_level) FROM run_slice";

  printf("SQL: %s \n\n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getIntCb, maxMsLevel, errMsg);
}

//Only return the MaxMsLevel from the previous statement
int getMaxMsLevelOrDie(
  sqlite3 *db
  )
{
  int maxMsLevel;
  char *errMsg = NULL;

  int rc = getMaxMsLevel(db, &maxMsLevel, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return maxMsLevel;
}

//Uses the method getTableRecordsCount to specifically return the Table Records Count from the bounding_box table
int getBoundingBoxesCountFromSequence(
  sqlite3 *db,
  int *bbCount,
  char *errMsg
  )
{
  int rc = 0;

  rc = getTableRecordsCount(db, "bounding_box", bbCount, &errMsg);
  //printf("Bounding Boxes Table Records Count: %d\n\n",i);

  return rc;
}


//Only return the Bounding Boxes count from the previous statement
int getBoundingBoxesCountFromSequenceOrDie(
  sqlite3 *db
  )
{
  int bbcount;
  char *errMsg = NULL;

  int rc = getBoundingBoxesCountFromSequence(db, &bbcount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return bbcount;
}

//Return the Mz Range depending on the ms_level
int getMzRange(
  sqlite3 *db,
  int msLevel,
  int *minMaxMz,
  char **errMsg
  )
{
  int maxMz = 0;
  char *zSQL = sqlite3_mprintf("SELECT min(begin_mz), max(end_mz) FROM run_slice WHERE ms_level=%d", msLevel);
  int minMz = executeSelectStmt(db, zSQL, 0, &getIntCb, minMaxMz, errMsg);

  printf("Valeur De Mz Minimale : %d\n", *minMaxMz);
  maxMz = executeSelectStmt(db, zSQL, 1, &getIntCb, minMaxMz, errMsg);
  printf("Valeur De Mz Maximale : %d\n\n", *minMaxMz);
  sqlite3_free(zSQL);
  return 0;
}

//Only return the mz Range from the previous statement
void getMzRangeOrDie(
  sqlite3 *db,
  int msLevel
  )
{
  int minMaxMz;
  char *errMsg = NULL;

  int rc = getMzRange(db, msLevel, &minMaxMz, &errMsg);

  dieOnSqliteError(rc, errMsg);
}

//Return the whole statement to find the BoundingBoxesCount from bounding_box table where the id is given by the user
int getBoundingBoxesCount(
  sqlite3 *db,
  int bbRunSliceId,
  int *bbCount,
  char **errMsg
  )
{
  char *zSQL = sqlite3_mprintf("SELECT count(*) FROM bounding_box WHERE bounding_box.run_slice_id = %d", bbRunSliceId);

  int rc = executeSelectStmt(db, zSQL, 0, &getIntCb, bbCount, errMsg);

  printf("SQL: %s \n\n", zSQL);

  sqlite3_free(zSQL);

  return rc;
}


//Only return the bbCount from the previous statement
int getBoundingBoxesCountOrDie(
  sqlite3 *db,
  int bbRunSliceId
  )
{
  int bbCount;
  char *errMsg = NULL;

  int rc = getBoundingBoxesCount(db, bbRunSliceId, &bbCount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return bbCount;
}

//Return the whole statement to find the Cycles Count from spectrum table
int getCyclesCount
(
  sqlite3 *db,
  int *cyclesCount,
  char **errMsg

)
{
  const char *sqlString = "SELECT cycle FROM spectrum ORDER BY id DESC LIMIT 1";

  printf("SQL: %s \n\n", sqlString);

  return executeSelectStmt(db, sqlString, 0, &getIntCb, cyclesCount, errMsg);
}

//Only return the cyclesCount from the previous statement
int getCyclesCountOrDie(
  sqlite3 *db
  )
{
  int cyclesCount;
  char *errMsg = NULL;

  int rc = getCyclesCount(db, &cyclesCount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return cyclesCount;
}

//Uses the method getTableRecordsCount to specifically return the Data Encodings Count from the data_encoding table
int getDataEncodingsCountFromSequence(
  sqlite3 *db,
  int *count,
  char **errMsg
  )
{
  int rc = 0;

  rc = getTableRecordsCount(db, "data_encoding", count, errMsg);
  //printf("Data Encoding Table Records Count: %d\n\n",i);

  return rc;
}

//Only return the Data Encodings count from the previous statement
int getDataEncodingsCountFromSequenceOrDie(
  sqlite3 *db
  )
{
  int count;
  char *errMsg = NULL;

  int rc = getDataEncodingsCountFromSequence(db, &count, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return count;
}

//Uses the method getTableRecordsCount to specifically return the Spectra Count from the spectrum table
int getSpectraCountFromSequence(
  sqlite3 *db,
  int *spectraCount,
  char *errMsg
  )
{
  int rc = 0;

  rc = getTableRecordsCount(db, "spectrum", spectraCount, &errMsg);

  return rc;
}

//Only return the Spectra count from the previous statement
int getSpectraCountFromSequenceOrDie(
  sqlite3 *db
  )
{
  int spectraCount;
  char *errMsg = NULL;

  int rc = getSpectraCountFromSequence(db, &spectraCount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return spectraCount;
}

//Return the whole statement to find the Spectra Count from spectrum table where the ms_level is given by the user
int getSpectraCount
(
  sqlite3 *db,
  int msLevel,
  int *spectraCount,
  char **errMsg
)
{
  //printf("SQL: %s \n\n",sqlString);

  char *zSQL = sqlite3_mprintf("SELECT count(*) FROM spectrum WHERE ms_level = %d", msLevel);

  int rc = executeSelectStmt(db, zSQL, 0, &getIntCb, spectraCount, errMsg);

  sqlite3_free(zSQL);

  return rc;

  //return bindAndExecuteSelectStmt(db, sqlString, &bindSpectraCountQuery, &getFirstIntCb, spectraCount, errMsg);
  //return 0;
}

//Only return the spectraCount from the previous statement
int getSpectraCountOrDie(
  sqlite3 *db,
  int msLevel
  )
{
  int spectraCount;
  char *errMsg = NULL;

  int rc = getSpectraCount(db, msLevel, &spectraCount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return spectraCount;
}

//Uses the method getTableRecordsCount to specifically return the Run Slices Count from the run_slice table
int getRunSlicesCountFromSequence(
  sqlite3 *db,
  int *runSlicesCount,
  char **errMsg
  )
{
  int rc = 0;

  rc = getTableRecordsCount(db, "run_slice", runSlicesCount, errMsg);
  //printf("Run Slices Records Count: %d\n\n",count);

  return rc;
}

//Only return the Run Slices count from the previous statement
int getRunSlicesCountFromSequenceOrDie(
  sqlite3 *db
  )
{
  int runSlicesCount;
  char *errMsg = NULL;

  int rc = getRunSlicesCountFromSequence(db, &runSlicesCount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return runSlicesCount;
}

//Return the sequence from a specific table where the user gives the table name
int getTableRecordsCount(
  sqlite3 *db,
  char *tableName,
  int *tableRecordsCount,
  char **errMsg
  )
{
  int rc = 0;
  char *zSQL = sqlite3_mprintf("SELECT seq FROM sqlite_sequence WHERE name = '%s'", tableName);

  printf("Requete egale : %s\n\n", zSQL);
  rc = executeSelectStmt(db, zSQL, 0, &getIntCb, tableRecordsCount, errMsg);
  sqlite3_free(zSQL);
  return rc;
}

//Only return the table record count from the previous statement
int getTableRecordsCountOrDie(
  sqlite3 *db,
  char *tableName
  )
{
  int tableRecordsCount;
  char *errMsg = NULL;

  int rc = getTableRecordsCount(db, tableName, &tableRecordsCount, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return tableRecordsCount;
}


//Return blobs from the bounding box table where the bounding box id is given by the user.
//First we allocate memory to a variable depending on the blob length, then we stock the blob into this variable (our variable here is data)
int getBoundingBoxData
(
  sqlite3 *db,
  int bbId,
  byte **blob,
  int *blobLength,
  char **errMsg
)
{
  sqlite3_blob *blobHandle;
  int rc = sqlite3_blob_open(db, "main", "bounding_box", "data", bbId, 0, &blobHandle);

  if (rc != SQLITE_OK)
    {
      *errMsg = "Error occured : Opening Blob 'data' in table 'bounding_box'";
      return rc;
    }
  *blobLength = sqlite3_blob_bytes(blobHandle);
  *blob = (byte*)calloc(*blobLength, sizeof(byte));

  rc = sqlite3_blob_read(blobHandle, *blob, *blobLength, 0);
  sqlite3_blob_close(blobHandle);
  return rc;
}

//Only return the data contained in a bounding box from the previous statement
byte *getBoundingBoxDataOrDie(
  sqlite3 *db,
  int bbId,
  int *blobLength
  )
{
  byte *blob = NULL;
  char *errMsg = NULL;

  int rc = getBoundingBoxData(db, bbId, &blob, blobLength, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return blob;
}

typedef void (*DisplaySpectraContentPtr)(void *blob, int blobLength);

//Function returning a callback pointer depending on the Data Encoding Id
//return NULL if DataEncoding is not managed
//TO DO : Create cache to avoid sqlQuerie at each call
DisplaySpectraContentPtr getDisplaySpectraContent(sqlite3 *db, int dataEncodingId)
{
  char *mode, *compression, *byteOrder;
  int mzPrecision, intensityPrecision;
  //TO DO: Manage error
  char *errMsg = NULL;
  char *sqlStringEncoding = sqlite3_mprintf("SELECT * FROM data_encoding d WHERE d.id = %d", dataEncodingId);
  int rc = executeSelectStmt(db, sqlStringEncoding, 1, &getStringCb, &mode, &errMsg);

  rc = executeSelectStmt(db, sqlStringEncoding, 2, &getStringCb, &compression, &errMsg);
  rc = executeSelectStmt(db, sqlStringEncoding, 3, &getStringCb, &byteOrder, &errMsg);
  rc = executeSelectStmt(db, sqlStringEncoding, 4, &getIntCb, &mzPrecision, &errMsg);
  rc = executeSelectStmt(db, sqlStringEncoding, 5, &getIntCb, &intensityPrecision, &errMsg);
  printf("mode : %s\n", mode);
  printf("compression : %s\n", compression);
  printf("byteOrder : %s\n", byteOrder);
  printf("mzPrecision : %d\n", mzPrecision);
  printf("intensityPrecision : %d\n", intensityPrecision);

  sqlite3_free(sqlStringEncoding);
  if (!strcmp(mode, "centroid") && !strcmp(compression, "none") && !strcmp(byteOrder, "little_endian") && mzPrecision == 32 && intensityPrecision == 32)
    return displaySpectraContent32_32;
  if (!strcmp(mode, "centroid") && !strcmp(compression, "none") && !strcmp(byteOrder, "little_endian") && mzPrecision == 64 && intensityPrecision == 32)
    return displaySpectraContent64_32;
  if (!strcmp(mode, "centroid") && !strcmp(compression, "none") && !strcmp(byteOrder, "little_endian") && mzPrecision == 64 && intensityPrecision == 64)
    return displaySpectraContent64_64;
  return NULL;
}


//Return blobs from the Chromatogram table where the chromatogram id is given by the user.
//First we allocate memory to a variable depending on the blob length, then we stock the blob into this variable (our variable here is data)
int getChromatogramDataPoints
(
  sqlite3 *db,
  int cId,
  byte **data,
  char **errMsg
)
{
  int blobSize = 0;
  char *zSQL = sqlite3_mprintf("SELECT length(data_points) FROM chromatogram WHERE chromatogram.id = %d", cId);
  int rc = executeSelectStmt(db, zSQL, 0, &getIntCb, &blobSize, errMsg);

  printf("Taille du Blob : %d\n\n", blobSize);
  *data = (byte*)calloc(blobSize, sizeof(byte));

  zSQL = sqlite3_mprintf("SELECT data_points FROM chromatogram WHERE chromatogram.id = %d\n\n", cId);
  rc = executeSelectNStmt(db, zSQL, 0, &getBlobCb, blobSize, data, errMsg);
  sqlite3_free(zSQL);
  return rc;
}

byte *getChromatogramDataPointsOrDie
(
  sqlite3 *db,
  int cId
)
{
  byte *blob = NULL;
  char *errMsg = NULL;
  int rc = getChromatogramDataPoints(db, cId, &blob, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return blob;
}

//Return the whole statement to find the bounding box first spectrum id from spectrum table where the id is given by the user
long getBoundingBoxFirstSpectrumId
(
  sqlite3 *db,
  int firstId,
  long *bbFirstSpectrumId,
  char **errMsg
)
{
  char *zSQL = sqlite3_mprintf("SELECT bb_first_spectrum_id FROM spectrum WHERE id = %d", firstId);
  int rc = executeSelectStmt(db, zSQL, 0, &getIntCb, bbFirstSpectrumId, errMsg);

  sqlite3_free(zSQL);

  return rc;
}

//Only return the bbFirstSpectrumId from the previous statement
long getBoundingBoxFirstSpectrumIdOrDie(
  sqlite3 *db,
  int firstId
  )
{
  long bbFirstSpectrumId;
  char *errMsg = NULL;

  int rc = getBoundingBoxFirstSpectrumId(db, firstId, &bbFirstSpectrumId, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return bbFirstSpectrumId;
}

//Return the whole statement to find the bounding box minimum Mz from bounding_box_rtree table where the bounding box rtree id is given by the user
int getBoundingBoxMinMz
(
  sqlite3 *db,
  int bbRtreeId,
  float *bbMinMz,
  char **errMsg
)
{
  char *zSQL;
  int rc;

  printf("Test \n");

  zSQL = sqlite3_mprintf("SELECT min_mz FROM bounding_box_rtree WHERE bounding_box_rtree.id = %d", bbRtreeId);
  printf("Test SQL : %s\n", zSQL);
  rc = executeSelectStmt(db, zSQL, 0, &getFloatCb, bbMinMz, errMsg);
  printf("Test rc : %d\n", rc);

  sqlite3_free(zSQL);

  return rc;
}

//Only return the bbMinMz from the previous statement
float getBoundingBoxMinMzOrDie(
  sqlite3 *db,
  int bbRtreeId
  )
{
  float bbMinMz;
  char *errMsg = NULL;

  int rc = getBoundingBoxMinMz(db, bbRtreeId, &bbMinMz, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return bbMinMz;
}

//Return the whole statement to find the bounding box minimum time from bounding_box_rtree table where the bounding box rtree id is given by the user
int getBoundingBoxMinTime
(
  sqlite3 *db,
  int bbRtreeId,
  float *bbMinTime,
  char **errMsg
)
{
  char *zSQL = sqlite3_mprintf("SELECT min_time FROM bounding_box_rtree WHERE bounding_box_rtree.id = %d", bbRtreeId);
  int rc = executeSelectStmt(db, zSQL, 0, &getFloatCb, bbMinTime, errMsg);

  sqlite3_free(zSQL);
  return rc;
}

//Only return the bbMinTime from the previous statement
float getBoundingBoxMinTimeOrDie(
  sqlite3 *db,
  int bbRtreeId
  )
{
  float bbMinTime;
  char *errMsg = NULL;

  int rc = getBoundingBoxMinTime(db, bbRtreeId, &bbMinTime, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return bbMinTime;
}

//Return the whole statement to find the run Slice Id from bounding_box table where the id is given by the user
int getRunSliceId
(
  sqlite3 *db,
  int bbId,
  int *runSliceId,
  char **errMsg
)
{
  char *zSQL = sqlite3_mprintf("SELECT run_slice_id FROM bounding_box WHERE id = %d", bbId);
  int rc = executeSelectStmt(db, zSQL, 0, &getIntCb, runSliceId, errMsg);

  sqlite3_free(zSQL);
  return rc;
}

//Only return the Run Slice ID from the previous statement
int getRunSliceIdOrDie(
  sqlite3 *db,
  int bbId
  )
{
  int runSliceId;
  char *errMsg = NULL;

  int rc = getRunSliceId(db, bbId, &runSliceId, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return runSliceId;
}


//Return the whole statement to find the Ms Level from run_slice table where the run_slice_id is given by the user
//The user must type the result from "getRunSliceID" himself to obtain the related Ms Level
int getMsLevelFromRunSliceIdManually
(
  sqlite3 *db,
  int runSliceId,
  int *msLevel,
  char **errMsg
)
{
  char *zSQL = sqlite3_mprintf("SELECT ms_level FROM run_slice WHERE run_slice.id = %d", runSliceId);
  int rc = executeSelectStmt(db, zSQL, 0, &getIntCb, msLevel, errMsg);

  sqlite3_free(zSQL);
  return rc;
}

int getMsLevelFromRunSliceIdManuallyOrDie(
  sqlite3 *db,
  int runSliceId
  )
{
  int msLevel;
  char *errMsg = NULL;

  int rc = getMsLevelFromRunSliceIdManually(db, runSliceId, &msLevel, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return msLevel;
}

//Combine last two functions in one
//Get the run slice Id from the bounding_box_id and its associated MsLevel
int getBoundingBoxMsLevel(
  sqlite3 *db,
  int bbId,
  int *resultMsLevel,
  char **errMsg
  )
{
  int *firstResult = 0;
  int rc = 0;
  char *zSQLBis;
  char *zSQL = sqlite3_mprintf("SELECT run_slice_id FROM bounding_box WHERE id = %d", bbId);

  rc = executeSelectStmt(db, zSQL, 0, &getIntCb, &firstResult, errMsg);
  printf("L'id de run_slice recherche est : %d\n\n", firstResult);
  zSQLBis = sqlite3_mprintf("SELECT ms_level FROM run_slice WHERE run_slice.id = %d", firstResult);
  rc = executeSelectStmt(db, zSQLBis, 0, &getIntCb, resultMsLevel, errMsg);
  printf("Et voila son indice ms associe : %d\n\n", *resultMsLevel);

  return 0;
}


//Only return the bounding box ms level from the previous statement
int getBoundingBoxMsLevelOrDie(
  sqlite3 *db,
  int bbId
  )
{
  int resultMsLevel;
  char *errMsg = NULL;

  int rc = getBoundingBoxMsLevel(db, bbId, &resultMsLevel, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return resultMsLevel;
}

/* Callback for getAllFromBoundingBox. Specific to 64_32 peaks */
void fillPeaks64_32Callback(byte *blob, SpectrumData *spectrumData)
{
  int i;
  int peaksCount;
  DataPoint64_32 *peaks64_32 = NULL;

  spectrumData->dataPrecision = DATA_PRECISION_64_32;
  peaksCount = ((int*)blob)[1];
  // convert blob to 64_32 peaks
  // id = ((int*)blob)[0]); // [unused]
  spectrumData->nbPeaks = peaksCount;
  spectrumData->peaks64_32 = (DataPoint64_32*)malloc(sizeof(DataPoint64_32) * peaksCount);
  peaks64_32 = (DataPoint64_32*)((int*)blob + 2);

  for (i = 0; i < peaksCount; i++)
    {
      spectrumData->peaks64_32[i].x = peaks64_32[i].x;
      spectrumData->peaks64_32[i].y = peaks64_32[i].y;
    }
}

//Not tested!
/* Callback for getAllFromBoundingBox. Specific to 64_64 peaks */
void fillPeaks64_64Callback(byte *blob, SpectrumData *spectrumData)
{
  int i;
  int peaksCount;
  DataPoint64_64 *peaks64_64 = NULL;

  spectrumData->dataPrecision = DATA_PRECISION_64_64;

  peaksCount = ((int*)blob)[1];
  // convert blob to 64_64 peaks
  // id = ((int*)blob)[0]); // [unused]
  spectrumData->nbPeaks = peaksCount;
  spectrumData->peaks64_64 = (DataPoint64_64*)malloc(sizeof(DataPoint64_64) * peaksCount);
  peaks64_64 = (DataPoint64_64*)((int*)blob + 2);

  for (i = 0; i < peaksCount; i++)
    {
      spectrumData->peaks64_64[i].x = peaks64_64[i].x;
      spectrumData->peaks64_64[i].y = peaks64_64[i].y;
    }
}

/* Callback for getAllFromBoundingBox. Specific to 32_32 peaks */
void fillPeaks32_32Callback(byte *blob, SpectrumData *spectrumData)
{
  int i;
  int peaksCount;
  DataPoint32_32 *peaks32_32 = NULL;

  spectrumData->dataPrecision = DATA_PRECISION_32_32;

  peaksCount = ((int*)blob)[1];

  // convert blob to 32_32 peaks
  // id = ((int*)blob)[0]); // [unused]
  spectrumData->nbPeaks = peaksCount;
  spectrumData->peaks32_32 = (DataPoint32_32*)malloc(sizeof(DataPoint32_32) * peaksCount);
  peaks32_32 = (DataPoint32_32*)((int*)blob + 2);

  for (i = 0; i < peaksCount; i++)
    {
      spectrumData->peaks32_32[i].x = peaks32_32[i].x;
      spectrumData->peaks32_32[i].y = peaks32_32[i].y;
    }
}

//Not tested
//TO DO : more callback for other data_encoding types when they are used
/* Callback for getAllFromBoundingBox. Specific to fitted 64_32 peaks */
void fillFittedPeaks64_32Callback(byte *blob, SpectrumData *spectrumData)
{
  int i;
  int peaksCount;
  FittedPeak *fittedPeaks64_32 = NULL;

  spectrumData->dataPrecision = DATA_PRECISION_64_32;

  peaksCount = ((int*)blob)[1];

  // convert blob to 64_32 fitted peaks
  // id = ((int*)blob)[0]); // [unused]
  spectrumData->nbPeaks = peaksCount;
  spectrumData->peaksFitted64_32 = (FittedPeak*)malloc(sizeof(FittedPeak) * peaksCount);
  fittedPeaks64_32 = (FittedPeak*)((int*)blob + 2);

  for (i = 0; i < peaksCount; i++)
    {
      spectrumData->peaksFitted64_32[i].x = fittedPeaks64_32[i].x;
      spectrumData->peaksFitted64_32[i].y = fittedPeaks64_32[i].y;
    }
}

// returning data precision depending on Data encoding stored into SQL Database
DataPrecision getDataPrecision(sqlite3 *db, int dataEncodingId)
{
  char *mode, *compression, *byteOrder;
  int mzPrecision, intensityPrecision;
  //TO DO: Manage error
  char *errMsg = NULL;
  char *sqlStringEncoding = sqlite3_mprintf("SELECT * FROM data_encoding d WHERE d.id = %d", dataEncodingId);
  int rc = executeSelectStmt(db, sqlStringEncoding, 1, &getStringCb, &mode, &errMsg);

  rc = executeSelectStmt(db, sqlStringEncoding, 2, &getStringCb, &compression, &errMsg);
  rc = executeSelectStmt(db, sqlStringEncoding, 3, &getStringCb, &byteOrder, &errMsg);
  rc = executeSelectStmt(db, sqlStringEncoding, 4, &getIntCb, &mzPrecision, &errMsg);
  rc = executeSelectStmt(db, sqlStringEncoding, 5, &getIntCb, &intensityPrecision, &errMsg);
  //    printf("mode : %s\n", mode);
  //    printf("compression : %s\n", compression);
  //    printf("byteOrder : %s\n", byteOrder);
  //    printf("mzPrecision : %d\n", mzPrecision);
  //    printf("intensityPrecision : %d\n", intensityPrecision);

  sqlite3_free(sqlStringEncoding);
  if (!strcmp(mode, "centroid") && !strcmp(compression, "none") && !strcmp(byteOrder, "little_endian") && mzPrecision == 32 && intensityPrecision == 32)
    return DATA_PRECISION_32_32;
  if (!strcmp(mode, "centroid") && !strcmp(compression, "none") && !strcmp(byteOrder, "little_endian") && mzPrecision == 64 && intensityPrecision == 32)
    return DATA_PRECISION_64_32;
  if (!strcmp(mode, "centroid") && !strcmp(compression, "none") && !strcmp(byteOrder, "little_endian") && mzPrecision == 64 && intensityPrecision == 64)
    return DATA_PRECISION_64_64;
  return DATA_PRECISION_UNKNOWN;
}

//Function returning a callback pointer depending on the Data Encoding Id
//return NULL if DataEncoding is not managed
//TO DO : Create cache to avoid sqlQuerie at each call
FillPeaksCallbackPtr getFillPeaksCallback(DataPrecision dataPrecision)
{
  switch (dataPrecision)
    {
    case DATA_PRECISION_32_32:
      return fillPeaks32_32Callback;
      break;

    case DATA_PRECISION_64_32:
      return fillPeaks64_32Callback;
      break;

    case DATA_PRECISION_64_64:
      return fillPeaks64_64Callback;
      break;
    }
  return NULL;
}

//get the data_encoding id from spectrum where the bounding box ID is given by the user
int getDataEncodingId
(
  sqlite3 *db,
  int boundingBoxId,
  int *dataEncodingId,
  char **errMsg
)
{
  char *sqlStringEncodingId = sqlite3_mprintf("SELECT s.data_encoding_id FROM spectrum s, bounding_box b WHERE b.id = %d AND b.first_spectrum_id = s.id", boundingBoxId);
  int rc = executeSelectStmt(db, sqlStringEncodingId, 0, &getIntCb, dataEncodingId, errMsg);

  sqlite3_free(sqlStringEncodingId);
  return rc;
}

//Only return the Data Encoding ID from the previous statement
int getDataEncodingIdOrDie
(
  sqlite3 *db,
  int boundingBoxId
)
{
  int dataEncodingId;
  char *errMsg = NULL;

  int rc = getDataEncodingId(db, boundingBoxId, &dataEncodingId, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return dataEncodingId;
}


//get x value from SpectrumData depending of its DATA_PRECISION
double getXValue(SpectrumData spectrumData, int index)
{
  switch (spectrumData.dataPrecision)
    {
    case DATA_PRECISION_32_32:
      return spectrumData.peaks32_32[index].x;
      break;

    case DATA_PRECISION_64_32:
      return spectrumData.peaks64_32[index].x;
      break;

    case DATA_PRECISION_64_64:
      return spectrumData.peaks64_64[index].x;
      break;
    }
  return 0;
}

//get y value from SpectrumData depending of its DATA_PRECISION
double getYValue(SpectrumData spectrumData, int index)
{
  switch (spectrumData.dataPrecision)
    {
    case DATA_PRECISION_32_32:
      return spectrumData.peaks32_32[index].y;
      break;

    case DATA_PRECISION_64_32:
      return spectrumData.peaks64_32[index].y;
      break;

    case DATA_PRECISION_64_64:
      return spectrumData.peaks64_64[index].y;
      break;
    }
  return 0;
}

//get LeftHwhmValue value from SpectrumData depending of its DATA_PRECISION (only needed in fitted mode)
double getLeftHwhmValue(SpectrumData spectrumData, int index, DataPrecision dataPrecision)
{
  switch (dataPrecision)
    {
    case DATA_PRECISION_32_32:
      return 0;
      break;

    case DATA_PRECISION_64_32:
      return 0;
      break;

    case DATA_PRECISION_64_64:
      return 0;
      break;

    case DATA_PRECISION_FITTED_64_32:
      return spectrumData.peaksFitted64_32[index].y;
      break;
    }
  return 0;
}
// TODO: check x, y <=> left, right
//get RightHwhmValue value from SpectrumData depending of its DATA_PRECISION (only needed in fitted mode)
double getRightHwhmValue(SpectrumData spectrumData, int index, DataPrecision dataPrecision)
{
  switch (dataPrecision)
    {
    case DATA_PRECISION_32_32:
      return 0;
      break;

    case DATA_PRECISION_64_32:
      return 0;
      break;

    case DATA_PRECISION_64_64:
      return 0;
      break;

    case DATA_PRECISION_FITTED_64_32:
      return spectrumData.peaksFitted64_32[index].x;
      break;
    }
  return 0;
}

//get the size of each data Precision (used for storage allocation)
int getDataPrecisionSizeInBytes(DataPrecision dataPrecision)
{
  switch (dataPrecision)
    {
    case DATA_PRECISION_32_32:
      return sizeof(DataPoint32_32);
      break;

    case DATA_PRECISION_64_32:
      return sizeof(DataPoint64_32);
      break;

    case DATA_PRECISION_64_64:
      return sizeof(DataPoint64_64);
      break;

    case DATA_PRECISION_FITTED_64_32:
      return sizeof(FittedPeak);
      break;
    }
  return 0;
}

//test
//return all the values contained in a Bounding Box where the id is given by the user.
/*int getAllFromBoundingBox(
        sqlite3 *db,
        int boundingBoxId,
        BoundingBox *boundingBox,
        char **errMsg
        ) {
    int rc, stmtStepRc;
    FillPeaksCallbackPtr callback;
    DisplayBlobPtr display;
    sqlite3_stmt *stmt;
    byte *blob = NULL;
    int blobSize;
    int dataEncodingId;
    char *zSQL = sqlite3_mprintf("SELECT * FROM bounding_box WHERE id =%d" ,boundingBoxId);


    // Prepare SQL query
    rc = sqlite3_prepare_v2(db, zSQL, -1, &stmt, NULL);
    if ( rc != SQLITE_OK ) {
        printf("erreur rencontrée : %s", errMsg);
        return rc;
    }
    while(1) {

        stmtStepRc = sqlite3_step(stmt);

        while ( stmtStepRc == SQLITE_ROW ) {

            // get first column (id)
            getIntCb(stmt, 0, 1, &(boundingBox->id));

            getIntCb(stmt, 2, 1, &(boundingBox->runSliceId));
            getIntCb(stmt, 3, 1, &(boundingBox->firstSpectrumId));
            getIntCb(stmt, 4, 1, &(boundingBox->lastSpectrumId));

            getDataEncodingId(db, boundingBox->id, &dataEncodingId, errMsg);

            blobSize = sqlite3_column_bytes(stmt, 1);
            blob = (byte *)malloc(blobSize);
            memcpy(blob, sqlite3_column_blob(stmt, 1), blobSize);

            callback = getFillPeaksCallback(db, dataEncodingId);
            if(callback == NULL){
 * errMsg = _strdup("Erreur rencontrée : Traitement de Data Encoding inconnue");
                return SQLITE_ERROR;
            }

            // fill peaks structure with blob content (using callback)
            callback(blob, boundingBox);

            if(boundingBox->nbPeak != 0)
            {
                getBoundingBoxData(db, boundingBox->id, &blob, &blobSize, errMsg);
                display = getDisplaySpectraContent(db, dataEncodingId);
                if(display == NULL){
 * errMsg = _strdup("Erreur rencontrée : Traitement de Data Encoding inconnue");
                    return SQLITE_ERROR;
                }

                display(blob, blobSize);
            }
            free(blob);


            // read next SQL row
            stmtStepRc = sqlite3_step(stmt);
        }

        rcBis = getDataEncodingId(db, &(boundingBox->id), &dataEncodingId, errMsg);
        printf("Data Encoding Id = %d\n", dataEncodingId);


        if (stmtStepRc != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return stmtStepRc;
        }
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    return stmtStepRc;
   }*/

//TO DO : getSpectrumSlicesInRange
int getSpectrumSlicesInRange(
  sqlite3 *db,
  float minMz,
  float maxMz,
  float minTime,
  float maxTime,
  int msLevel,
  SpectrumSlice **spectrumSlicesPtr,
  int *nbSpectrumSlicesPtr,
  char **errMsg
  )
{
  int nPeak;
  int rc, stmtStepRc;
  FillPeaksCallbackPtr fillPeaksCallback;
  BoundingBox boundingBox;
  SpectrumSlice sqlSpectrumSlice;
  SpectrumSlice newSpectrumSlice;
  DataPrecision dataPrecision = DATA_PRECISION_UNKNOWN;
  MzdbParamTree mzdbParamTree;
  sqlite3_stmt *stmt;
  byte *blob = NULL;
  int blobSize;
  int dataPrecisionSize;
  float rtWidth;
  double mzHeight;
  int dataEncodingId;
  const char *sqlString;

  printf("initial ptr = %X\n", nbSpectrumSlicesPtr);

  rc = getParamTreeMzdb(db, &mzdbParamTree, errMsg);
  if (rc != SQLITE_OK)
    {
      printf("An error occured trying to return param tree: '%s'\n", errMsg);
      exit(2);
    }

  if (msLevel = 1)
    {
      rtWidth = mzdbParamTree.ms1BbTimeWidth;
      mzHeight = mzdbParamTree.ms1BbMzWidth;
    }
  else
    {
      rtWidth = mzdbParamTree.msnBbTimeWidth;
      mzHeight = mzdbParamTree.msnBbMzWidth;
    }

  sqlString = sqlite3_mprintf("SELECT * FROM bounding_box WHERE id IN(SELECT id FROM bounding_box_rtree WHERE min_mz >= %f AND max_mz <= %f AND min_time >= %f AND max_time <= %f ) ORDER BY first_spectrum_id", minMz - mzHeight, maxMz + mzHeight, minTime - rtWidth, maxTime + rtWidth);
  printf("SQL: %s \n", sqlString);

  rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);
  if (rc != SQLITE_OK)
    {
      printf("erreur rencontrée : %s", errMsg);
      return rc;
    }
  stmtStepRc = sqlite3_step(stmt);
  *nbSpectrumSlicesPtr = 0;
  *spectrumSlicesPtr = NULL;

  while (stmtStepRc == SQLITE_ROW)
    {
      // get first column (id)
      getIntCb(stmt, 0, 1, &(boundingBox.id));

      getIntCb(stmt, 2, 1, &(boundingBox.runSliceId));
      getIntCb(stmt, 3, 1, &(boundingBox.firstSpectrumId));
      getIntCb(stmt, 4, 1, &(boundingBox.lastSpectrumId));

      getDataEncodingId(db, boundingBox.id, &dataEncodingId, errMsg);

      dataPrecision = getDataPrecision(db, dataEncodingId);
      dataPrecisionSize = getDataPrecisionSizeInBytes(dataPrecision);

      blobSize = sqlite3_column_bytes(stmt, 1);
      blob = (byte*)malloc(blobSize);
      memcpy(blob, sqlite3_column_blob(stmt, 1), blobSize);


      fillPeaksCallback = getFillPeaksCallback(dataPrecision);

      if (fillPeaksCallback == NULL)
        {
          *errMsg = _strdup("Erreur rencontrée : Traitement de Data Encoding inconnue");
          return SQLITE_ERROR;
        }



      // fill peaks structure with blob content (using callback)
      fillPeaksCallback(blob, &(sqlSpectrumSlice.spectrum.data));

      //Initialisation du nouveau SpectrumSlice
      newSpectrumSlice.spectrum.data.nbPeaks = 0;
      newSpectrumSlice.spectrum.data.peaks = NULL;
      newSpectrumSlice.spectrum.data.dataPrecision = dataPrecision;
      newSpectrumSlice.runSliceId = boundingBox.runSliceId;

      // DANGER: on créé un spectrumSlice même si on ne mettra pas de peaks dedans :(

      // loop over all spectra and filter those outside given mz/rt range
      for (nPeak = 0; nPeak < sqlSpectrumSlice.spectrum.data.nbPeaks; nPeak++)
        {
          int peaksShift;
          double mz = getXValue(sqlSpectrumSlice.spectrum.data, nPeak);
          double rt = getYValue(sqlSpectrumSlice.spectrum.data, nPeak);
          if (1 || mz >= minMz && mz <= maxMz && rt >= minTime && rt <= maxTime && sqlSpectrumSlice.spectrum.data.nbPeaks != 0)
            {
              int sqlIndex = 0;
              int newIndex = 0;
              void *sqlCurrentPtr = NULL;
              void *newCurrentPtr = NULL;


              // JET: ajouter(copier au sens memcpy) un peak (realloc) de sqlSpectrumSlice.spectrum.data vers spectrumSlice->spectrum.data
              newSpectrumSlice.spectrum.data.nbPeaks++;

              // JET: reallouer le champs data correspondant à la precision avec le bon nb de pics

              newSpectrumSlice.spectrum.data.peaks = realloc(newSpectrumSlice.spectrum.data.peaks, newSpectrumSlice.spectrum.data.nbPeaks * dataPrecisionSize);

              sqlIndex = nPeak;
              newIndex = newSpectrumSlice.spectrum.data.nbPeaks - 1;
              sqlCurrentPtr = (void*)(((int)sqlSpectrumSlice.spectrum.data.peaks) + sqlIndex * dataPrecisionSize);
              newCurrentPtr = (void*)(((int)newSpectrumSlice.spectrum.data.peaks) + newIndex * dataPrecisionSize);
              peaksShift = nPeak * dataPrecisionSize;
              memcpy(newCurrentPtr, sqlCurrentPtr, dataPrecisionSize);
            }
        }

      if (newSpectrumSlice.spectrum.data.nbPeaks != 0)
        {
          // instantiate a new spectrumSlice
          (*nbSpectrumSlicesPtr)++;

          // JET: réallouer (oc) spectrumSlices avec un SpectrumSlice de plus
          // JET: copier les nouveaux peaks filtrés dans le nouveau spectrumSlice
          *spectrumSlicesPtr = (SpectrumSlice*)realloc(*spectrumSlicesPtr, (*nbSpectrumSlicesPtr) * sizeof(SpectrumSlice));
          memcpy(&(*spectrumSlicesPtr)[(*nbSpectrumSlicesPtr) - 1], &newSpectrumSlice, sizeof(SpectrumSlice));
        }

      // read next SQL row
      stmtStepRc = sqlite3_step(stmt);
    }

  if (stmtStepRc != SQLITE_DONE)
    {
      sqlite3_finalize(stmt);
      return stmtStepRc;
    }
  sqlite3_finalize(stmt);
  return SQLITE_OK;
}

//return the values contained in the SpectrumSlice structure filled thanks to the previous method
SpectrumSlice *getSpectrumSlicesInRangeOrDie
(
  sqlite3 *db,
  float minMz,
  float maxMz,
  float minTime,
  float maxTime,
  int msLevel
)
{
  SpectrumSlice *spectrumSlicesPtr;
  int nbSpectrumSlicesPtr;
  char *errMsg = NULL;
  int rc = getSpectrumSlicesInRange(db, minMz, maxMz, minTime, maxTime, msLevel, &spectrumSlicesPtr, &nbSpectrumSlicesPtr, &errMsg);

  dieOnSqliteError(rc, errMsg);

  return spectrumSlicesPtr;
}

//TO DO : COMPLETE WHEN getNearestPeak are completed
/*int spectrumSlicesToXic(
        SpectrumSlice *spectrumSlices,
        double searchedMz,
        double mzTolPPM,
        XicMethodType xicMethod,
        Peak *xicPeaks,
        char **errMsg
        ) {
    SpectrumSlice spectrumSliceTmp;
    DataPrecision dataPrecision = DATA_PRECISION_UNKNOWN;
    int spectrumSlicesCount;
    int dataPrecisionSize;
    int i;

    spectrumSlicesCount = sizeof(*spectrumSlices);

    if(spectrumSlices = NULL){
 * errMsg = _strdup("null detected");
        return -1;
    }

    if(spectrumSlicesCount == 0){
        memset(xicPeaks, 0, sizeof(xicPeaks));
    }


    xicPeaks->mz = searchedMz;

    switch(xicMethod){
    case MAX:
        for(i = 0; i < spectrumSlicesCount; i++)
        {
            int j;
            int peaksCount;
            spectrumSliceTmp = spectrumSlices[i];
            dataPrecision = spectrumSliceTmp.spectrum.data.dataPrecision;
            dataPrecisionSize = getDataPrecisionSizeInBytes(dataPrecision);
            for(j = 0; j<spectrumSliceTmp.spectrum.data.nbPeaks; j++)
            {
                if(dataPrecision == DATA_PRECISION_64_32)
                {
                    xicPeaks[j].mz = spectrumSliceTmp.spectrum.data.peaks64_32->x;
                    xicPeaks[j].intensity = spectrumSliceTmp.spectrum.data.peaks64_32->y;
                }

                // Do we have to check if left and rightHwhm are != NULL? (spectrumSliceTmp.spectrum.data.peaksFitted64_32->leftHwhm != NULL && spectrumSliceTmp.spectrum.data.peaksFitted64_32->rightHwhm != NULL)
                if(dataPrecision == DATA_PRECISION_FITTED_64_32)
                {
                    xicPeaks[j].mz = spectrumSliceTmp.spectrum.data.peaksFitted64_32->x;
                    xicPeaks[j].intensity = spectrumSliceTmp.spectrum.data.peaksFitted64_32->y;
                    xicPeaks[j].leftHwhm = spectrumSliceTmp.spectrum.data.peaksFitted64_32->leftHwhm;
                    xicPeaks[j].rightHwhm = spectrumSliceTmp.spectrum.data.peaksFitted64_32->rightHwhm;
                }
            }

            peaksCount = sizeof(*xicPeaks);
            if(peaksCount == 0)
            {
                continue;
            }
            //To-Do : check if qSort is the equivalent of Arrays.sort in C language : if it is use it to order the xicPeaks array.
            //To-Do : malloc size for the xicPeaks array and free it.
        }
        break;
    case NEAREST:
        //code
        break;
    case SUM:
        //code
        break;
    default:
        //code
        break;
    }

   }*/
