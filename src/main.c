#define _CRT_SECURE_NO_WARNINGS

#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "log.h"
#include "models.h"
#include "mzdb.h"
#include "queries.h"
#include "tables.h"
#include "test.h"
#include "util.h"
#include "xml.h"
#include "lib/sqlite/sqlite3.h"

#define BUFFSIZE        8192
char Buff[BUFFSIZE];
int Depth;
clock_t startClock, endClock;
double cpu_time_used;

static void _logSlice(int spectrumId, int nbPeaks)
{
  _logf(LOG_DEBUG,"spectrum id = %d", spectrumId);
  _logf(LOG_DEBUG,"nb peaks = %d", nbPeaks);
}

static void _logBlob(void *blob, int blobLength, void *data, int nbSpectrum)
{
  _logf(LOG_DEBUG,"difference between data and blob: %d", (int)data - (int)blob);
  _logf(LOG_DEBUG,"blob length: %d", blobLength);
  _logf(LOG_DEBUG,"nb spectra: %d", nbSpectrum);
}

//Display the values contained in a DataPoint64_32 structure (translate the value inside of a BLOB encoded in centroid mode 64_32)
void displaySpectraContent64_32(void *blob, int blobLength)
{
  DataPoint64_32 *peaks64_32;
  void *data;
  int nbPeaks;
  int spectrumId;
  int spectraCount;
  int i;

  _log(LOG_DEBUG,"Results from Blob:");
  data = blob;
  spectraCount = 0;

  while ((int)data - (int)blob < blobLength)
    {
      spectrumId = ((int*)data)[0];
      nbPeaks = ((int*)data)[1];
      _logSlice(spectrumId, nbPeaks);

      peaks64_32 = (DataPoint64_32*)((int*)data + 2);
      for (i = 0; i < ((int*)data)[1]; i++)
        {
          _logf(LOG_DEBUG,"x[%d] = %f - y[%d] = %f", i, peaks64_32[i].x, i, peaks64_32[i].y);
        }
      data = (byte*)data + 2 * sizeof(int) + nbPeaks * sizeof(DataPoint64_32);
      spectraCount++;
    }

  _logBlob(blob,blobLength,data,spectraCount);
}

// Display the values contained in a DataPoint32_32 structure (translate the value inside of a BLOB encoded in centroid mode 32_32)
void displaySpectraContent32_32(void *blob, int blobLength)
{
  DataPoint32_32 *peaks32_32;
  void *data;
  int nbPeaks;
  int spectrumId;
  int spectraCount;
  int i;

  _logf(LOG_DEBUG,"Results from Blob:");
  data = blob;
  spectraCount = 0;

  while ((int)data - (int)blob < blobLength)
    {
      spectrumId = ((int*)data)[0];
      nbPeaks = ((int*)data)[1];
      _logSlice(spectrumId, nbPeaks);

      peaks32_32 = (DataPoint32_32*)((int*)data + 2);
      for (i = 0; i < ((int*)data)[1]; i++)
        {
          _logf(LOG_DEBUG,"x[%d] = %f - y[%d] = %f", i, peaks32_32[i].x, i, peaks32_32[i].y);
        }
      data = (byte*)data + 2 * sizeof(int) + nbPeaks * sizeof(DataPoint32_32);
      spectraCount++;
    }

  _logBlob(blob,blobLength,data,spectraCount);
}

//Not tested!
//Display the values contained in a DataPoint64_64 structure (translate the value inside of a BLOB encoded in centroid mode 64_64)
void displaySpectraContent64_64(void *blob, int blobLength)
{
  DataPoint64_64 *peaks64_64;
  void *data;
  int nbPeaks;
  int spectrumId;
  int spectraCount;
  int i;

  _logf(LOG_DEBUG,"Results from Blob:");
  data = blob;
  spectraCount = 0;

  while ((int)data - (int)blob < blobLength)
    {
      spectrumId = ((int*)data)[0];
      nbPeaks = ((int*)data)[1];
      _logSlice(spectrumId, nbPeaks);

      peaks64_64 = (DataPoint64_64*)((int*)data + 2);
      for (i = 0; i < ((int*)data)[1]; i++)
        {
          /*int p0 = (int)&(peaks64_32[0]);
             int pi = (int)&(peaks64_32[i]);
             int pi1 = (int)&(peaks64_32[i-1]);*/
          _logf(LOG_DEBUG,"x[%d] = %f - y[%d] = %f", i, peaks64_64[i].x, i, peaks64_64[i].y);
          //printf("mem shift from start = %d  from prev = %d\n ", pi-p0, pi-pi1 );
        }
      data = (byte*)data + 2 * sizeof(int) + nbPeaks * sizeof(DataPoint64_64);
      spectraCount++;
    }

  _logBlob(blob,blobLength,data,spectraCount);
}

int main(int argc, char* argv[])
{
  sqlite3 *db;
  char *errMsg = NULL;
  int rc;

  startClock = clock();

  //Enter the database name which you wish to work on
  rc = openMzDbFile("../data/OVEMB150205_12.raw.0.9.8.mzDB", &db);
  //rc = sqlite3_open("../data/OFCAG150908_01_0.9.8_fitted.raw.mzDB", &db);

  if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR, "Can't open mzDB file: %s", sqlite3_errmsg(db));
      exit(EXIT_FAILURE);
    }
  else
    {
      _log(LOG_INFO, "Opened mzDB file successfully !");
    }

  rc = testGetters(db, &errMsg);
  if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR,"An error occured in testSlices: '%s'\n", errMsg);
      exit(EXIT_FAILURE);
    }

  testParamTrees(db, &errMsg);
  if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR,"An error occured in testSlices: '%s'\n", errMsg);
      exit(EXIT_FAILURE);
    }

  testSlices(db, &errMsg);
  if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR,"An error occured in testSlices: '%s'\n", errMsg);
      exit(EXIT_FAILURE);
    }

  testGetData(db, &errMsg);
  if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR,"An error occured in testSlices: '%s'\n", errMsg);
      exit(EXIT_FAILURE);
    }

  testGettersOrDie(db);
  if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR,"An error occured in testSlices: '%s'\n", errMsg);
      exit(EXIT_FAILURE);
    }

  /*    rc = getAllFromBoundingBox(db, 5, &boundingBox, &errMsg);

     if (rc != SQLITE_OK) {
      printf("An error occured trying to return param tree: '%s'\n", errMsg);
      exit(2);
     }
     else
     {
      printf("bounding box id: %d\n",boundingBox.id);
      printf("bounding box nb peaks: %d\n",boundingBox.nbPeak);
      printf("bounding box run slice id: %d\n",boundingBox.runSliceId);
      if ( boundingBox.nbPeak >0) printf("bounding box 1st peaks: %f %f\n",boundingBox.peaks32_32[5].x,boundingBox.peaks32_32[5].y);
      printf("bounding box First Spectrum id: %d\n",boundingBox.firstSpectrumId);
      printf("bounding box Last Spectrum id: %d\n",boundingBox.lastSpectrumId);
      printf("Data Encoding : %s\n", dataPrecisionString[boundingBox.dataPrecision]);
     }*/
  /*switch(myInt){
      case 1:

          break;

      case 2:
          rc = getAllFromBoundingBox(db, 5, &boundingBox, &fillPeaks32_32Callback, &errMsg);

          if (rc != SQLITE_OK) {
              printf("An error occured trying to return param tree: '%s'\n", errMsg);
              exit(2);
          }
           else
          {
              printf("bounding box id: %d\n",boundingBox.id);
              printf("bounding box run slice id: %d\n",boundingBox.runSliceId);
              printf("bounding box nb peaks: %d\n",boundingBox.nbPeak);
              if ( boundingBox.nbPeak >0) printf("bounding box 1st peaks: %f %f\n",boundingBox.peaks32_32[0].x,boundingBox.peaks32_32[0].y);
              printf("bounding box First Spectrum id: %d\n",boundingBox.firstSpectrumId);
              printf("bounding box Last Spectrum id: %d\n",boundingBox.lastSpectrumId);
          }
          break;

      default:
          printf("Valeur de Data Encoding non reconnue");
          break;
     }*/
  /*
     rc = getAllFromBoundingBox(db, &boundingBox, &fillPeaks64_32Callback, &errMsg);

     if (rc != SQLITE_OK) {
      printf("An error occured trying to return param tree: '%s'\n", errMsg);
      exit(2);
     }
     else
     {
      printf("bounding box id: %d\n",boundingBox.id);
      printf("bounding box run slice id: %d\n",boundingBox.runSliceId);
      printf("bounding box nb peaks: %d\n",boundingBox.nbPeak);
      if ( boundingBox.nbPeak >0) printf("bounding box 1st peaks: %f %f\n",boundingBox.peaks64_32[12].x,boundingBox.peaks64_32[12].y);
      printf("bounding box First Spectrum id: %d\n",boundingBox.firstSpectrumId);
      printf("bounding box Last Spectrum id: %d\n",boundingBox.lastSpectrumId);
     }*/


  /*Tests sur structures
     strcpy(text1.cvRef, "test");
     strcpy(text1.accession, "access");
     strcpy(text1.name, "nom");
     strcpy(text1.text, "blablabla");
     strcpy(text1.type, "1");
     printf("valeur cvRef : %s\n valeur accession : %s\n nom User Text : %s\n  message : %s\n type : %s\n\n", text1.cvRef, text1.accession, text1.name, text1.text,text1.type);
     box1.dataMode[0] = NO_LOSS_PEAK;
     box1.dataMode[1] = HIGH_RES_PEAK;
     box1.dataMode[2] = LOW_RES_PEAK;
     for(i = 0 ; i < 3; i++)
     {
         printf("%d\n", box1.dataMode[i]);
     }*/
  //Fin tests sur structures

  //mySpectrumHeaders = NULL;
  //rc = getSpectrumHeaders(db, &mySpectrumHeaders, &errMsg);

  //printf("Test XML bare");

  //Speed test
  /*start = clock();
     for(i = 0; i < 250 ; i++)
     {
          rc = getSpectrumHeaders(db, &mySpectrumHeaders, &errMsg);
     }
     end = clock();
     printf("time = %f", ((double) (end - start)) / CLOCKS_PER_SEC);*/

  /*if (rc != SQLITE_OK)
    {
      _logf(LOG_ERROR, "SQL error: %s\n", errMsg);
      sqlite3_free(errMsg);
    }
  else
    {
      fprintf(stdout, "Operation done successfully\n");
    }*/

  endClock = clock();
  _logf(LOG_DEBUG,"time = %f", ((double)(endClock - startClock)) / CLOCKS_PER_SEC);

  closeMzDbFile(db);

  return EXIT_SUCCESS;
}


