#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#include "lib/sqlite/sqlite3.h"
#include "queries.h"
#include "mzdb.h"

// TODO

int libmzdb_open_mzdb_file(const char* filename, sqlite3** pp_db)
{
    // TODO: use int32_t instead of int/long everywhere in the project
    // See: https://stackoverflow.com/questions/11438794/is-the-size-of-c-int-2-bytes-or-4-bytes/11438838
    assert(sizeof(int) == 4); // check that mzDB files can be decoded on this platform
    return sqlite3_open_v2(filename, pp_db, SQLITE_OPEN_READONLY, NULL);
}

int libmzdb_close_mzdb_file(sqlite3* pp_db)
{
    return sqlite3_close(pp_db);
}

int libmzdb_create_spectrum_header(sqlite3_stmt* stmt, libmzdb_spectrum_header_t* spectrum_header, char** err_msg)
{
    if(!spectrum_header) return -1; //add warning log
    libmzdb_spectrum_header_t* h = spectrum_header; //esthetic variable
    //init the header
    h->id = sqlite3_column_int(stmt, 0);
    h->initial_id = sqlite3_column_int(stmt, 1);
    h->title = strdup((const char *) sqlite3_column_text(stmt, 2));
    h->cycle = sqlite3_column_int(stmt, 3);
    h->time = (float)sqlite3_column_double(stmt, 4);
    h->ms_level = sqlite3_column_int(stmt, 5);
    h->activation_type = strdup( (const char *) sqlite3_column_text(stmt, 6));
    h->tic = (float)sqlite3_column_double(stmt, 7);
    h->base_peak_mz = sqlite3_column_double(stmt, 8);
    h->base_peak_intensity = (float)sqlite3_column_double(stmt, 9);
    h->precursor_mz = sqlite3_column_double(stmt, 10);
    h->precursor_charge = sqlite3_column_int(stmt, 11);
    h->peaks_count = sqlite3_column_int(stmt, 12);
    h->param_tree_str = strdup((const char *)sqlite3_column_text(stmt, 13)); //TODO: check!
    h->scan_list_str = strdup((const char *)sqlite3_column_text(stmt, 14));
    h->precursor_list_str = strdup((const char *)sqlite3_column_text(stmt, 15));
    h->product_list_str = strdup((const char *)sqlite3_column_text(stmt, 16));
    h->shared_param_tree_id = sqlite3_column_int(stmt, 17);
    h->instrument_configuration_id = sqlite3_column_int(stmt, 18);
    h->source_file_id = sqlite3_column_int(stmt, 19);
    h->run_id = sqlite3_column_int(stmt, 20);
    h->data_processing_id = sqlite3_column_int(stmt, 21);
    h->data_encoding_id = sqlite3_column_int(stmt, 22);
    h->bb_first_spectrum_id = sqlite3_column_int(stmt, 23);
    return SQLITE_OK;
}

int libmzdb_get_spectrum_headers(sqlite3* db, libmzdb_spectrum_header_t** spectrum_headers, int* spectrum_header_count, char** err_msg)
{
    sqlite3_stmt* stmt;
    int rc /* sql result state */, stmt_step_res /* step result state */;

    //get the length of the spectrum_headers array
    *err_msg = "Error: get spectrum failed headers (count query)"; //to improve
    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) return rc;
    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return rc;

    const int count = sqlite3_column_int(stmt, 0); //the length of the spectrum_headers array
    sqlite3_finalize(stmt); //delete the stmt

    *err_msg = "Error: get spectrum headers failed (* query)";
    libmzdb_spectrum_header_t* h_arr = (libmzdb_spectrum_header_t *) malloc(count * sizeof(libmzdb_spectrum_header_t));
    if ((rc= sqlite3_prepare_v2(db, "SELECT * FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) {
        sqlite3_finalize(stmt);
        free(h_arr);
        return rc;
    }

    *err_msg = "Error: get spectrum headers failed (init of each headers)";

    int index=-1;
    while((stmt_step_res=sqlite3_step(stmt))==SQLITE_ROW)
    {
        libmzdb_create_spectrum_header(stmt, &h_arr[++index], err_msg); //init the current header
    }

    sqlite3_finalize(stmt); //delete the stmt
    if (stmt_step_res != SQLITE_DONE) return stmt_step_res;

    *err_msg = "";
    *spectrum_headers = h_arr;
    *spectrum_header_count = count;

    return SQLITE_OK;
}

int libmzdb_create_entity_cache(sqlite3 *db, libmzdb_entity_cache_t** entity_cache, char **err_msg)
{
    int rc;

    //printf("Before get_data_encodings_cache\n");

    libmzdb_data_encodings_cache_t* de_cache;
    if ((rc = libmzdb_get_data_encodings_cache(db, &de_cache))!=SQLITE_OK) {
        *err_msg = "Error: get_data_encodings_cache failed";
        return rc;
    }
    //printf("Before get_spectrum_headers\n");

    libmzdb_spectrum_header_t *headers;
    int header_count;
    if ((rc = libmzdb_get_spectrum_headers(db, &headers, &header_count, err_msg)) != SQLITE_OK) return rc;

    //printf("Before malloc\n");

    libmzdb_entity_cache_t* cache = (libmzdb_entity_cache_t*) malloc(sizeof(libmzdb_entity_cache_t));
    *entity_cache = cache;

    cache->data_encodings_cache = *de_cache;
    cache->spectrum_headers = headers;
    cache->spectrum_header_count = header_count;

    return SQLITE_OK;
}

//end TODO

















///* get all spectrum headers from a database */
//int getSpectrumHeaders(
//        sqlite3 *db,
//        SpectrumHeader **spectrumHeaders,
//        char **errMsg

//        )
//{
//    sqlite3_stmt *stmt;   // sql statement
//    SpectrumHeader *spectrumHeader = NULL;   // current spectrum header to treat
//    int rc;   // sql result state
//    int stmtStepRc;   // step result state
//    int spectrumIndex;
//    int spectrumCount;

//    const char *sqlString = "SELECT * FROM spectrum";

//    //printf("SQL: %s \n\n",sqlString);

//    sqlite3_prepare_v2(db, "SELECT count(id) FROM spectrum", -1, &stmt, NULL);
//    stmtStepRc = sqlite3_step(stmt);
//    spectrumCount = sqlite3_column_int(stmt, 0);
//    sqlite3_finalize(stmt);

//    // Prepare SQL query
//    rc = sqlite3_prepare_v2(db, sqlString, -1, &stmt, NULL);

//    // Check prepare is OK
//    if (rc != SQLITE_OK)
//        return rc;
//    /*
//    //Count rows -> faster with prepare/step/finalize
//    spectrumCount = 0;
//    stmtStepRc = sqlite3_step(stmt);
//    while ( stmtStepRc == SQLITE_ROW ) {

//        stmtStepRc = sqlite3_step(stmt);
//        spectrumCount++;

//    }

//    sqlite3_reset(stmt);*/




//    //printf("Number of counts : %d\n", spectrumCount);
//    *spectrumHeaders = (SpectrumHeader*)malloc(spectrumCount * sizeof(SpectrumHeader));

//    // loop over rows
//    spectrumIndex = 0;
//    stmtStepRc = sqlite3_step(stmt);
//    printf("number of values : %d\n", sqlite3_data_count(stmt));
//    while (stmtStepRc == SQLITE_ROW)
//    {
//        spectrumHeader = (*spectrumHeaders) + spectrumIndex;
//        // treat one row (one spectrum)
//        getSpectrumHeader(stmt, spectrumHeader, errMsg);

//        stmtStepRc = sqlite3_step(stmt);
//        spectrumIndex++;
//    }

//    if (stmtStepRc != SQLITE_DONE)
//    {
//        return rc;
//    }




//    /* int rc = sqlite3_blob_open(db, "main", "spectrum", "id", index, 0, &blobHandle);
//     if(rc != SQLITE_OK)
//     {
//   * errMsg = "Error occured : Opening Blob 'data' in table 'bounding_box'";
//       return rc;
//     }
//     blobLength = sqlite3_blob_bytes(blobHandle);
//     blob = (byte*)calloc(blobLength,sizeof(byte));

//     rc = sqlite3_blob_read(blobHandle, blob,blobLength, 0);
//     intArray = (int*)blob;
//     printf("ID : %d\n", intArray[0]);
//     printf("Initial ID : %d\n", intArray[1]);
//     sqlite3_blob_close(blobHandle);*/
//    return rc;
//}







///* get one spectrum header from a n SQL statement */
//int getSpectrumHeader(sqlite3_stmt *stmt,
//                      SpectrumHeader *spectrumHeaderPtr,
//                      char **errMsg
//                      )
//{
//    if (spectrumHeaderPtr == NULL)
//    {
//        return SQLITE_ERROR;
//    }
//    spectrumHeaderPtr->id = sqlite3_column_int(stmt, 0);
//    spectrumHeaderPtr->initialId = sqlite3_column_int(stmt, 1);
//    spectrumHeaderPtr->title = _strdup(sqlite3_column_text(stmt, 2));
//    spectrumHeaderPtr->cycle = sqlite3_column_int(stmt, 3);
//    spectrumHeaderPtr->time = (float)sqlite3_column_double(stmt, 4);
//    spectrumHeaderPtr->msLevel = sqlite3_column_int(stmt, 5);
//    spectrumHeaderPtr->activationType = _strdup(sqlite3_column_text(stmt, 6));
//    spectrumHeaderPtr->tic = (float)sqlite3_column_double(stmt, 7);
//    spectrumHeaderPtr->basePeakMz = sqlite3_column_double(stmt, 8);
//    spectrumHeaderPtr->basePeakIntensity = (float)sqlite3_column_double(stmt, 9);
//    spectrumHeaderPtr->precursorMz = sqlite3_column_double(stmt, 10);
//    spectrumHeaderPtr->precursorCharge = sqlite3_column_int(stmt, 11);
//    spectrumHeaderPtr->peaksCount = sqlite3_column_int(stmt, 12);
//    spectrumHeaderPtr->paramTree = _strdup(sqlite3_column_text(stmt, 13));
//    spectrumHeaderPtr->scanList = _strdup(sqlite3_column_text(stmt, 14));
//    spectrumHeaderPtr->precursorList = _strdup(sqlite3_column_text(stmt, 15));
//    spectrumHeaderPtr->productList = _strdup(sqlite3_column_text(stmt, 16));
//    spectrumHeaderPtr->sharedParamTreeID = sqlite3_column_int(stmt, 17);
//    spectrumHeaderPtr->instrumentConfigurationID = sqlite3_column_int(stmt, 18);
//    spectrumHeaderPtr->sourceFileID = sqlite3_column_int(stmt, 19);
//    spectrumHeaderPtr->runID = sqlite3_column_int(stmt, 20);
//    spectrumHeaderPtr->dataProcessingID = sqlite3_column_int(stmt, 21);
//    spectrumHeaderPtr->dataEncodingID = sqlite3_column_int(stmt, 22);
//    spectrumHeaderPtr->bbFirstSpectrumId = sqlite3_column_int(stmt, 23);

//    printf("Spectrum ID : %d\n", spectrumHeaderPtr->id);
//    printf("Spectrum initial ID : %d\n", spectrumHeaderPtr->initialId);
//    printf("Spectrum Title : %s\n", spectrumHeaderPtr->title);
//    printf("Spectrum cycle : %d\n", spectrumHeaderPtr->cycle);
//    printf("Spectrum time : %f\n", spectrumHeaderPtr->time);
//    printf("Spectrum Ms Level : %d\n", spectrumHeaderPtr->msLevel);
//    printf("Spectrum Activation Type : %s\n", spectrumHeaderPtr->activationType);
//    printf("Spectrum TIC : %f\n", spectrumHeaderPtr->tic);
//    printf("Spectrum Base Peak Mz : %f\n", spectrumHeaderPtr->basePeakMz);
//    printf("Spectrum Base Peak Intensity : %f\n", spectrumHeaderPtr->basePeakIntensity);
//    printf("Spectrum Main Precursor Mz : %f\n", spectrumHeaderPtr->precursorMz);
//    printf("Spectrum Main Precursor Charge : %d\n", spectrumHeaderPtr->precursorCharge);
//    printf("Spectrum Data Points Count : %d\n", spectrumHeaderPtr->peaksCount);
//    printf("Spectrum Param Tree : %s\n", spectrumHeaderPtr->paramTree);
//    printf("Spectrum Scan List : %s\n", spectrumHeaderPtr->scanList);
//    printf("Spectrum Precursor List : %s\n", spectrumHeaderPtr->precursorList);
//    printf("Spectrum Product List : %s\n", spectrumHeaderPtr->productList);
//    printf("Spectrum Shared Param Tree ID : %d\n", spectrumHeaderPtr->sharedParamTreeID);
//    printf("Spectrum Instrument Configuration ID : %d\n", spectrumHeaderPtr->instrumentConfigurationID);
//    printf("Spectrum Source File ID : %d\n", spectrumHeaderPtr->sourceFileID);
//    printf("Spectrum Run ID : %d\n", spectrumHeaderPtr->runID);
//    printf("Spectrum Data Processing ID : %d\n", spectrumHeaderPtr->dataProcessingID);
//    printf("Spectrum Data Encoding ID : %d\n", spectrumHeaderPtr->dataEncodingID);
//    printf("Spectrum Bounding Box First Spectrum ID : %d\n\n", spectrumHeaderPtr->bbFirstSpectrumId);

//    return SQLITE_OK;
//}

//void freeSpectrumHeader(SpectrumHeader *spectrumHeaderPtr)
//{
//    if (spectrumHeaderPtr == NULL)
//        return;

//    if (spectrumHeaderPtr->title != NULL)
//        free(spectrumHeaderPtr->title);

//    if (spectrumHeaderPtr->activationType != NULL)
//        free(spectrumHeaderPtr->activationType);

//    if (spectrumHeaderPtr->paramTree != NULL)
//        free(spectrumHeaderPtr->paramTree);

//    if (spectrumHeaderPtr->scanList != NULL)
//        free(spectrumHeaderPtr->scanList);

//    if (spectrumHeaderPtr->precursorList != NULL)
//        free(spectrumHeaderPtr->precursorList);

//    if (spectrumHeaderPtr->productList != NULL)
//        free(spectrumHeaderPtr->productList);
//}

//int openMzDbFile(
//        const char *filename,   /* Database filename (UTF-8) */
//        sqlite3 **ppDb          /* OUT: SQLite db handle */
//        ) {
//    return sqlite3_open(filename, ppDb);
//}

//int closeMzDbFile(sqlite3* ppDb) {
//    return sqlite3_close(ppDb);
//}

