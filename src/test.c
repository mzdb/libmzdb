#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "models.h"
#include "mzdb.h"
#include "tables.h"
#include "queries.h"
#include "util.h"
#include "xml.h"
#include "lib/sqlite/sqlite3.h"

int testSlices(sqlite3 *db, char **errMsg)
{
  MzdbParamTree mzdbParamTree;
  SpectrumSlice *spectrumSlices;
  SpectrumSlice *spectrumSlicePtr;
  int nbSpectrumSlices;
  int nPeak;
  int rc;
  int spectrumSliceIdx;

  rc = getParamTreeMzdb(db, &mzdbParamTree, errMsg);
  if (rc != SQLITE_OK)
    {
      errMsg = concat(2,"An error occured while trying to load the param tree: ",errMsg);
      return rc;
    }

  _logf(LOG_DEBUG,"Spectrum Slices bf %X\n", &spectrumSlices);

  rc = getSpectrumSlicesInRange(
    db,
    300, 800, 0, 2000,
    1,
    &spectrumSlices,
    &nbSpectrumSlices,
    errMsg
    );
  _logf(LOG_DEBUG,"Spectrum Slices af %X\n", &spectrumSlices);

  if (rc != SQLITE_OK)
    {
      errMsg = concat(2,"An error occured while trying to load the spectrum slices: ",errMsg);
      return rc;
    }
  else
    {
      spectrumSlicePtr = spectrumSlices;

      _logf(LOG_DEBUG,"Nb Spectrum Slices %d\n", nbSpectrumSlices);
      _logf(LOG_DEBUG,"Spectrum Slices %X\n", spectrumSlices);

      for (spectrumSliceIdx = 0; spectrumSliceIdx < nbSpectrumSlices; spectrumSliceIdx++)
        {
          //spectrumSlice = spectrumSlices[spectrumSliceIdx];


          //printf("Debug: %X %X\n",spectrumSlices, spectrumSlicePtr ); fflush(stdout);
          //printf("Debug: %d\n", spectrumSlicePtr->runSliceId );
          //fflush(stdout);
          if (spectrumSlicePtr->spectrum.data.nbPeaks > 0)
            {
              _logf(LOG_DEBUG,"Run Slice Id %d\n", spectrumSlicePtr->runSliceId);
              //printf("indice : %d\n", indice);
              _logf(LOG_DEBUG,"spectrum data nbPeak %d\n", spectrumSlicePtr->spectrum.data.nbPeaks);

              for (nPeak = 0; nPeak < spectrumSlicePtr->spectrum.data.nbPeaks; nPeak++)
                {
                  double mz = getXValue(spectrumSlicePtr->spectrum.data, nPeak);
                  double rt = getYValue(spectrumSlicePtr->spectrum.data, nPeak);
                  _logf(LOG_DEBUG,"Data peaks #%d : %f %f\n", nPeak, mz, rt);
                }
            }
          spectrumSlicePtr++;
          //indice++;
        }
    }
//    free(spectrumSlices);

  return rc;
}

int testParamTrees(sqlite3 *db, char **errMsg)
{
  int rc;
  MzdbParamTree mzdbParamTree;

  //TO DO :
  /*rc = getParamTreeChromatogram(db, &paramTree, errMsg);
     if ( rc != SQLITE_OK ) {
      printf("An error occured trying to return param Tree: '%s'\n", errMsg);
      exit(2);
     }
     else
     {
      printf("Param Tree Chromatogram: %s\n\n",paramTree);
     }

     testXmlBare(paramTree);

     rc = getParamTreeSpectrum(db, 1, &paramTree, errMsg);
     if (rc != SQLITE_OK ) {
      printf("An error occured trying to return param Tree: '%s'\n", errMsg);
      exit(2);
     }
     else
     {
      printf("Param Tree Spectrum: %s\n\n",paramTree);
     }

     testXmlBare(paramTree);*/


  rc = getParamTreeMzdb(db, &mzdbParamTree, errMsg);
  if (rc != SQLITE_OK)
    {
      errMsg = concat(2,"An error occured while trying to load the param tree: ",errMsg);
      return rc;
    }

  _logf(LOG_DEBUG,"mzdbParamTree.ms1BbMzWidth = %f , mzdbParamTree.ms1BbTimeWidth = %f\n", mzdbParamTree.ms1BbMzWidth, mzdbParamTree.ms1BbTimeWidth);
  _logf(LOG_DEBUG,"mzdbParamTree.msnBbMzWidth = %f, mzdbParamTree.msnBbTimeWidth = %f\n", mzdbParamTree.msnBbMzWidth, mzdbParamTree.msnBbTimeWidth);
  _logf(LOG_DEBUG,"mzdbParamTree.isLossLess = %d\n", mzdbParamTree.isLossless);
  _logf(LOG_DEBUG,"mzdbParamTree.originFileFormat = %s\n", mzdbParamTree.originFileFormat);

  return rc;
}

int testGetters(sqlite3 *db, char **errMsg)
{
  int rc;
  int myInt = 0;
  double rt = 0;
  long myLong = 0;
  char *modelVersion = NULL;
  char *mzDbVersion = NULL;
  float myFloat = 0;
  int *i = 0;

  rc = getModelVersion(db, &modelVersion, errMsg);
  if (rc != SQLITE_OK)
    {
      errMsg = concat(2,"An error occured while trying to load the Model Version: ", errMsg);
      return rc;
    }
  _logf(LOG_DEBUG,"Model: %s", modelVersion);

  rc = getPwizMzDbVersion(db, &mzDbVersion, errMsg);
  if (rc != SQLITE_OK)
    {
      concat(2,"An error occured while trying to load the PwizMzDbVersion: ", errMsg);
      return rc;
    }
  _logf(LOG_DEBUG,"MzDb Model: %s", mzDbVersion);

  rc = getLastTime(db, &rt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"RT: %f", rt);

  rc = getMaxMsLevel(db, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Max Ms Level: %d", myInt);

  rc = getCyclesCount(db, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Cycles Count: %d", myInt);

  rc = getSpectraCount(db, 2, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Spectra Count: %d", myInt);

  rc = getBoundingBoxesCount(db, 2, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes Count: %d", myInt);

  rc = getBoundingBoxFirstSpectrumId(db, 500, &myLong, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes First Spectrum Id: %d", myLong);

  rc = getBoundingBoxMinMz(db, 2, &myFloat, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes Minimum Mz: %f", myFloat);

  rc = getBoundingBoxMinTime(db, 150, &myFloat, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes Minimum Time: %f", myFloat);

  rc = getRunSliceId(db, 13, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes Run Slice ID: %d", myInt);

  rc = getMsLevelFromRunSliceIdManually(db, 10, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes Ms Level: %d", myInt);

  rc = getTableRecordsCount(db, "run_slice", &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"Bounding Boxes Table Records Count: %d", myInt);

  rc = getMzRange(db, 1, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;

  rc = getBoundingBoxMsLevel(db, 1, &myInt, errMsg);
  if (rc != SQLITE_OK) return rc;

  rc = getBoundingBoxesCountFromSequence(db, &i, *errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"getBoundingBoxesCountFromSequence: %d", i);

  rc = getDataEncodingsCountFromSequence(db, &i, *errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"getDataEncodingsCountFromSequence: %d", i);

  rc = getSpectraCountFromSequence(db, &i, *errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"getSpectraCountFromSequence: %d", i);

  rc = getRunSlicesCountFromSequence(db, &i, *errMsg);
  if (rc != SQLITE_OK) return rc;
  _logf(LOG_DEBUG,"getRunSlicesCountFromSequence: %d", i);
}

int testGetData(sqlite3 *db, char **errMsg)
{
  int rc;
  int i = 0;
  int myInt = 0;
  byte *myBlob = NULL;
  DataPoint64_32 *peaks64_32 = 0;
  FittedPeak *peaks = 0;

  rc = getBoundingBoxData(db, 5, &myBlob, &myInt, errMsg);
  displaySpectraContent64_32(myBlob, myInt);
  free(myBlob);

  printf(" \n\n");
  rc = getBoundingBoxData(db, 4, &myBlob, &myInt, errMsg);
  printf(" \n\n");
  printf("Results from Blob : \n");
  printf("spectrum id = %d\n\n", ((int*)myBlob)[0]);
  printf("nb peaks = %d\n\n", ((int*)myBlob)[1]);
  peaks64_32 = (DataPoint64_32*)((int*)myBlob + 2);
  for (i = 0; i < ((int*)myBlob)[1]; i++)
    {
      //int p0 = (int)&(peaks64_32[0]);
      //int pi = (int)&(peaks64_32[i]);
      //int pi1 = (int)&(peaks[i-1]);
      printf("x[%d] = %f  ", i, peaks64_32[i].x);
      printf("y[%d] = %f\n", i, peaks64_32[i].y);
      //printf("left[%d] = %f\n",i, peaks[i].leftHwhm);
      //printf("right[%d] = %f\n",i, peaks[i].rightHwhm);
      //printf("mem shift from start = %d  from prev = %d\n ", pi-p0, pi-pi1 );
    }
  free(myBlob);
  printf(" \n\n");

  rc = getChromatogramDataPoints(db, 1, &myBlob, errMsg);
  for (i = 0; i < ((int*)myBlob)[1]; i++)
    /*{
        printf(" %X", myBlob[i]);
       }*/
    printf(" \n\n");
  printf("Results from Blob : \n");
  printf("spectrum id = %d\n\n", ((int*)myBlob)[0]);
  printf("nb peaks = %d\n\n", ((int*)myBlob)[1]);
  peaks = (FittedPeak*)((int*)myBlob + 2);
  for (i = 0; i < ((int*)myBlob)[1]; i++)
    {
      int p0 = (int)&(peaks[0]);
      int pi = (int)&(peaks[i]);
      int pi1 = (int)&(peaks[i - 1]);
      printf("x[%d] = %f  ", i, peaks[i].x);
      printf("y[%d] = %f\n", i, peaks[i].y);
      printf("fpdsf");
      printf("mem shift from start = %d  from prev = %d\n ", pi - p0, pi - pi1);
    }
  free(myBlob);
  printf(" \n\n");
}

int testGettersOrDie(sqlite3 *db)
{
  char *modelVersion = NULL;
  char *mzDbVersion = NULL;
  double rt = 0;
  long myLong = 0;
  float myFloat = 0;
  int myInt = 0;
  int *i = 0;

  modelVersion = getModelVersionOrDie(db);
  printf("model version from getModelVersionOrDie : %s\n\n", modelVersion);
  mzDbVersion = getPwizMzDbVersionOrDie(db);
  printf("PwizMzDb from getPwizMzDbVersionOrDie : %s\n\n", mzDbVersion);
  rt = getLastTimeOrDie(db);
  printf("Last Time from getLastTimeOrDie : %lf\n\n", rt);
  myInt = getMaxMsLevelOrDie(db);
  printf("Max ms from getMaxMsLevelOrDie : %d\n\n", myInt);
  myInt = getBoundingBoxesCountOrDie(db, 1);
  printf("Bounding Boxes Count from getBoundingBoxesCountOrDie : %d\n\n", myInt);
  myInt = getCyclesCountOrDie(db);
  printf("Cycles Count from getCyclesCountOrDie : %d\n\n", myInt);
  myInt = getSpectraCountOrDie(db, 2);
  printf("Spectra Count from getCyclesCountOrDie : %d\n\n", myInt);
  myLong = getBoundingBoxFirstSpectrumIdOrDie(db, 15);
  printf("Bounding Box First Spectrum id from getBoundingBoxFirstSpectrumIdOrDie : %ld\n\n", myLong);
  myFloat = getBoundingBoxMinMzOrDie(db, 15);
  printf("Bounding Box Min Mz from getBoundingBoxMinMzOrDie : %f\n\n", myFloat);
  myFloat = getBoundingBoxMinTimeOrDie(db, 150);
  printf("Bounding Box Min Time from getBoundingBoxMinTimeOrDie : %f\n\n", myFloat);
  myInt = getRunSliceIdOrDie(db, 13);
  printf("Run Slice Id from getRunSliceIdOrDie : %d\n\n", myInt);
  myInt = getMsLevelFromRunSliceIdManuallyOrDie(db, 10);
  printf("Ms Level from getMsLevelFromRunSliceIdManuallyOrDie : %d\n\n", myInt);
  getBoundingBoxMsLevelOrDie(db, 13);
  myInt = getTableRecordsCountOrDie(db, "run_slice");
  printf("Table Records from getTableRecordsCountOrDie : %d\n\n", myInt);
  myInt = getBoundingBoxesCountFromSequenceOrDie(db);
  printf("Bounding Boxes count from getBoundingBoxesCountFromSequence : %d\n\n", myInt);
  myInt = getDataEncodingsCountFromSequenceOrDie(db);
  printf("Data Encodings Count from getDataEncodingsCountFromSequenceOrDie : %d\n\n", myInt);
  myInt = getSpectraCountFromSequenceOrDie(db);
  printf("Spectra Count from getSpectraCountFromSequenceOrDie : %d\n\n", myInt);
  myInt = getRunSlicesCountFromSequenceOrDie(db);
  printf("Run Slices Count from getRunSlicesCountFromSequenceOrDie : %d\n\n", myInt);
  getMzRangeOrDie(db, 2);
}
