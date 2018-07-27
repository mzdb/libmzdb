#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#include "lib/sqlite/sqlite3.h"
#include "queries.h"
#include "mzdb.h"

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

    //*err_msg = "Error: get spectrum failed headers (count query)";
    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) return rc;
    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return rc;



    const int count = sqlite3_column_int(stmt, 0); //the length of the spectrum_headers array
    sqlite3_finalize(stmt); //delete the stmt



    //*err_msg = "Error: get spectrum headers failed (* query)";
    libmzdb_spectrum_header_t* h_arr = (libmzdb_spectrum_header_t *) malloc(count * sizeof(libmzdb_spectrum_header_t));
    if ((rc= sqlite3_prepare_v2(db, "SELECT * FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) {
        sqlite3_finalize(stmt);
        free(h_arr);
        return rc;
    }

    //*err_msg = "Error: get spectrum headers failed (init of each headers)";

    int index=-1;
    while((stmt_step_res=sqlite3_step(stmt))==SQLITE_ROW)
    {
        libmzdb_create_spectrum_header(stmt, &h_arr[++index], err_msg); //init the current header
    }

    sqlite3_finalize(stmt); //delete the stmt
    if (stmt_step_res != SQLITE_DONE) return stmt_step_res;

    //*err_msg = "";
    *spectrum_headers = h_arr;
    *spectrum_header_count = count;

    return SQLITE_OK;
}

int libmzdb_create_entity_cache(sqlite3 *db, libmzdb_entity_cache_t** entity_cache, char **err_msg)
{
    int rc;

    libmzdb_data_encodings_cache_t* de_cache;
    if ((rc = libmzdb_get_data_encodings_cache(db, &de_cache))!=SQLITE_OK) {
        *err_msg = "Error: get_data_encodings_cache failed";
        return rc;
    }

    libmzdb_spectrum_header_t *headers;
    int header_count;
    if ((rc = libmzdb_get_spectrum_headers(db, &headers, &header_count, err_msg)) != SQLITE_OK) return rc;

    libmzdb_entity_cache_t* cache = (libmzdb_entity_cache_t*) malloc(sizeof(libmzdb_entity_cache_t));
    *entity_cache = cache;

    cache->data_encodings_cache = *de_cache;
    cache->spectrum_headers = headers;
    cache->spectrum_header_count = header_count;

    return SQLITE_OK;
}
