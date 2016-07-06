#ifndef QUERIES_H
#define QUERIES_H
#include "lib/sqlite/sqlite3.h"
#include "models.h"

// this pragma is needed to pack all data structure without 8bits alignment
#pragma pack(push)
#pragma pack(1)



int getModelVersion(
  sqlite3 *db,
  char **modelVersion,
  char **errMsg
  );

char *getModelVersionOrDie(
  sqlite3 *db                /* The database on which the SQL executes */
  );

int getParamTreeChromatogram(
  sqlite3 *db,
  char **paramTreeChro,
  char **errMsg
  );

char *getParamTreeChromatogramOrDie(
  sqlite3 *db
  );

int getParamTreeSpectrum(
  sqlite3 *db,
  int paramTreeId,
  char **paramTreeSpectrum,
  char **errMsg
  );

char *getParamTreeSpectrumOrDie(
  sqlite3 *db,
  int paramTreeId
  );

int getParamTreeMzdb(
  sqlite3 *db,
  MzdbParamTree *paramTreeMzdb,
  char **errMsg
  );

char *getParamTreeMzdbOrDie(
  sqlite3 *db
  );

int getPwizMzDbVersion(
  sqlite3 *db,
  char **mzDbVersion,
  char **errMsg
  );

char *getPwizMzDbVersionOrDie(
  sqlite3 *db
  );

int getLastTime(
  sqlite3 *db,   /* The database on which the SQL executes */
  double *lastTime,
  char **errMsg  /* Write error messages here */
  );

double getLastTimeOrDie(
  sqlite3 *db
  );

int getMaxMsLevel(
  sqlite3 *db,
  int *maxMsLevel,
  char **errMsg
  );

int getMaxMsLevelOrDie(
  sqlite3 *db
  );

int getBoundingBoxesCountFromSequence(sqlite3 *db,
                                      int *bbCount
                                      , char *errMsg);

int getBoundingBoxesCountFromSequenceOrDie(
  sqlite3 *db
  );

int getMzRange(
  sqlite3 *db,
  int msLevel,
  int *minMaxMz,
  char **errMsg
  );

int getBoundingBoxesCount(
  sqlite3 *db,
  int bbRunSliceId,
  int *bbCount,
  char **errMsg
  );

int getBoundingBoxesCountOrDie(
  sqlite3 *db,
  int bbRunSliceId
  );

int getCyclesCount
(
  sqlite3 *db,
  int *cyclesCount,
  char **errMsg

);

int getCyclesCountOrDie(
  sqlite3 *db
  );

int getDataEncodingsCountFromSequence(
  sqlite3 *db,
  int *count,
  char *errMsg
  );

int getDataEncodingsCountFromSequenceOrDie(
  sqlite3 *db
  );

int getSpectraCountFromSequence(
  sqlite3 *db,
  int *spectraCount,
  char *errMsg
  );

int getSpectraCountFromSequenceOrDie(
  sqlite3 *db
  );

int getSpectraCount
(
  sqlite3 *db,
  int msLevel,
  int *spectraCount,
  char **errMsg
);

int getSpectraCountOrDie(
  sqlite3 *db,
  int msLevel
  );

int getRunSlicesCountFromSequence(
  sqlite3 *db,
  int *runSlicesCount,
  char *errMsg
  );

int getRunSlicesCountFromSequenceOrDie(
  sqlite3 *db
  );

int getTableRecordsCount(
  sqlite3 *db,
  char *tableName,
  int *tableRecordsCount,
  char **errMsg
  );

int getBoundingBoxData
(
  sqlite3 *db,
  int bbId,
  byte **blob,
  int *blobLength,
  char **errMsg
);

void displaySpectraContent64_64(void *blob, int blobLength);

void displaySpectraContent64_32(void *blob, int blobLength);

void displaySpectraContent32_32(void *blob, int blobLength);

int getChromatogramDataPoints
(
  sqlite3 *db,
  int cId,
  byte **data,
  char **errMsg
);

byte *getChromatogramDataPointsOrDie
(
  sqlite3 *db,
  int cId
);


typedef void (*DisplaySpectraContentPtr)(void *blob, int blobLength);

DisplaySpectraContentPtr getDisplaySpectraContent(sqlite3 *db, int dataEncodingId);

long getBoundingBoxFirstSpectrumId
(
  sqlite3 *db,
  int firstId,
  long *bbFirstSpectrumId,
  char **errMsg
);

long getBoundingBoxFirstSpectrumIdOrDie(
  sqlite3 *db,
  int firstId
  );

int getBoundingBoxMinMz
(
  sqlite3 *db,
  int bbRtreeId,
  float *bbMinMz,
  char **errMsg
);

float getBoundingBoxMinMzOrDie(
  sqlite3 *db,
  int bbRtreeId
  );

int getBoundingBoxMinTime
(
  sqlite3 *db,
  int bbRtreeId,
  float *bbMinTime,
  char **errMsg
);

float getBoundingBoxMinTimeOrDie(
  sqlite3 *db,
  int bbRtreeId
  );

int getRunSliceId
(
  sqlite3 *db,
  int bbId,
  int *runSliceId,
  char **errMsg
);

int getMsLevelFromRunSliceIdManually
(
  sqlite3 *db,
  int runSliceId,
  int *msLevel,
  char **errMsg
);

int getBoundingBoxMsLevel(
  sqlite3 *db,
  int bbId,
  int *resultMsLevel,
  char **errMsg
  );

int getRunSliceIdOrDie(
  sqlite3 *db,
  int bbId
  );

int getMsLevelFromRunSliceIdManuallyOrDie(
  sqlite3 *db,
  int runSliceId
  );


int getBoundingBoxMsLevelOrDie(
  sqlite3 *db,
  int bbId
  );

int getTableRecordsCountOrDie(
  sqlite3 *db,
  char *tableName
  );

void getMzRangeOrDie(
  sqlite3 *db,
  int msLevel
  );

typedef void (*FillPeaksCallbackPtr)(byte *blob, SpectrumData *spectrumData);

/* Callback for getAllFromBoundingBox function. Specific to 64_32 Data Precision */
void fillPeaks64_32Callback(byte *blob, SpectrumData *spectrumData);

/* Callback for getAllFromBoundingBox function. Specific to 64_64 Data Precision */
void fillPeaks64_64Callback(byte *blob, SpectrumData *spectrumData);

/* Callback for getAllFromBoundingBox function. Specific to 32_32 Data Precision */
void fillPeaks32_32Callback(byte *blob, SpectrumData *spectrumData);

/* Callback for getAllFromBoundingBox function. Specific to Fitted 64_32 Data Precision */
void fillFittedPeaks64_32Callback(byte *blob, SpectrumData *spectrumData);

// returning data precision depending on Data encoding stored into SQL Database
DataPrecision getDataPrecision(sqlite3 *db, int dataEncodingId);

//Function returning a callback pointer (FillPeaksCallbackPtr) depending on the Data Encoding Id
FillPeaksCallbackPtr getFillPeaksCallback(DataPrecision dataPrecision);

int getDataEncodingId
(
  sqlite3 *db,
  int boundingBoxId,
  int *dataEncodingId,
  char **errMsg
);

double getXValue(SpectrumData spectrumData, int index);

double getYValue(SpectrumData spectrumData, int index);


int getAllFromBoundingBox(
  sqlite3 *db,
  int boundingBoxId,
  BoundingBox *boundingBox,
  char **errMsg
  );

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
  );

SpectrumSlice *getSpectrumSlicesInRangeOrDie
(
  sqlite3 *db,
  float minMz,
  float maxMz,
  float minTime,
  float maxTime,
  int msLevel
);
#pragma pack(pop)

#endif /* QUERIES_H */
