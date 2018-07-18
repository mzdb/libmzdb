//#define _CRT_SECURE_NO_WARNINGS // TODO: comment the define


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

#include "lib/sqlite/sqlite3.h"
#include "util.h"
#include "xml.h"
#include "models.h"
#include "sqlite.h"
#include "queries.h"

#define NO_ERR_MSG ""


int libmzdb_get_model_version(sqlite3* db, char** model_version, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT version FROM mzdb LIMIT 1", 0, &libmzdb_get_string_cb, (void **) model_version, err_msg);
}

char* libmzdb_get_model_version_or_die(sqlite3* db)
{
    char* version;
    char* err_msg;
    int rc = libmzdb_get_model_version(db, &version, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);

    return version;
}

int libmzdb_get_param_tree_chromatogram(sqlite3* db, char** param_tree_chro, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT param_tree FROM chromatogram", 0, &libmzdb_get_string_cb, (void **) param_tree_chro, err_msg);
}

char* libmzdb_get_param_tree_chromatogram_or_die(sqlite3* db)
{
    char* param_tree, *err_msg;
    int rc = libmzdb_get_param_tree_chromatogram(db, &param_tree, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return param_tree;
}

int libmzdb_get_param_tree_spectrum(sqlite3* db, int param_tree_id, char** param_tree_spectrum, char**err_msg)
{
    char* sql_str = sqlite3_mprintf("SELECT param_tree FROM spectrum WHERE id = %d", param_tree_id);
    int rc = libmzdb_execute_select_stmt(db, sql_str, 0, &libmzdb_get_string_cb, (void **) param_tree_spectrum, err_msg);
    sqlite3_free(sql_str);
    return rc;
}

char* libmzdb_get_param_tree_spectrum_or_die(sqlite3* db, int param_tree_id)
{
    char* param_tree_spectrum, *err_msg;
    int rc = libmzdb_get_param_tree_spectrum(db, param_tree_id, &param_tree_spectrum, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return param_tree_spectrum;
}


int libmzdb_get_param_tree_mzdb(sqlite3* db, libmzdb_mzdb_param_tree_t* param_tree_mzdb, char** err_msg)
{
    char* param_tree; int rc;
    if( (rc = libmzdb_execute_select_stmt(db, "SELECT param_tree FROM mzdb", 0, &libmzdb_get_string_cb, (void **) &param_tree, err_msg))!=SQLITE_OK) return rc;
    nodec_ptr_t xml;
    if( xml_parse(param_tree, &xml) == 0) return -1;
    memset((void *) param_tree_mzdb, 0, sizeof(libmzdb_mzdb_param_tree_t)); //reset the param tree mzdb
    xml_browse(xml, xml_browse_callback_mzdb_param_tree, (void *)param_tree_mzdb);
    return rc;
}


libmzdb_mzdb_param_tree_t libmzdb_get_param_tree_mzdb_or_die(sqlite3 *db)
{
    libmzdb_mzdb_param_tree_t param_tree_mzdb;
    char * err_msg;
    int rc = libmzdb_get_param_tree_mzdb(db, &param_tree_mzdb, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return param_tree_mzdb;
}

int libmzdb_get_pwiz_mzdb_version(sqlite3* db, char** mzdb_version, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT version FROM software WHERE name LIKE '%mzDB'", 0, &libmzdb_get_string_cb, (void **) mzdb_version, err_msg);
}

char* libmzdb_get_pwiz_mzdb_version_or_die(sqlite3 *db)
{
    char* version, *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_pwiz_mzdb_version(db, &version, &err_msg), &err_msg);
    return version;
}

int libmzdb_get_last_time(sqlite3* db, double* last_time, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT time FROM spectrum ORDER BY id DESC LIMIT 1", 0, &libmzdb_get_double_cb, (void **)last_time, err_msg);
}

double libmzdb_get_last_time_or_die(sqlite3* db)
{
    double last_time;
    char* err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_last_time(db, &last_time, &err_msg), &err_msg);
    return last_time;
}

int libmzdb_get_max_ms_level(sqlite3* db, int* max_ms_level, char** err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT max(ms_level) FROM run_slice", 0, &libmzdb_get_int_cb, (void **) max_ms_level, err_msg);
}

int libmzdb_get_max_ms_level_or_die(sqlite3 *db)
{
    int max_ms_level;
    char* err_msg;
    int rc = libmzdb_get_max_ms_level(db, &max_ms_level, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return max_ms_level;
}



//Uses the method get_table_records_count to specifically return the Table Records Count from the bounding_box table
int libmzdb_get_bounding_boxes_count_from_sequence(sqlite3 *db,int *bb_count,char **err_msg)
{
    return libmzdb_get_table_records_count(db, "bounding_box", bb_count, err_msg);
}

int libmzdb_get_bounding_boxes_count_from_sequence_or_die(sqlite3 *db)
{
    int bb_count;
    char *err_msg;
    int rc = libmzdb_get_bounding_boxes_count_from_sequence(db, &bb_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return bb_count;
}

//Return the Mz Range depending on the ms_level
int libmzdb_get_mz_range(sqlite3 *db, int ms_level,int **min_max_mz,char **err_msg)
{
    int min_mz;
    int max_mz;
    int rc;
    char *z_sql = sqlite3_mprintf("SELECT min(begin_mz), max(end_mz) FROM run_slice WHERE ms_level=%d", ms_level);

    if ((rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) &min_mz, err_msg))!= SQLITE_OK)
    {
        sqlite3_free(z_sql);
        return rc;
    }

    if ((rc = libmzdb_execute_select_stmt(db, z_sql, 1, &libmzdb_get_int_cb, (void **) &max_mz, err_msg))!= SQLITE_OK)
    {
        sqlite3_free(z_sql);
        return rc;
    }
    sqlite3_free(z_sql);

    static int mz_range[2];
    mz_range[0] = min_mz;
    mz_range[1] = max_mz;
    *min_max_mz = mz_range;
    return SQLITE_OK;
}

int *libmzdb_get_mz_range_or_die(sqlite3 *db, int ms_level)
{
    int *min_max_mz;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_mz_range(db, ms_level, &min_max_mz, &err_msg), &err_msg);
    return min_max_mz;
}

int libmzdb_get_bounding_boxes_count(sqlite3 *db, int bb_run_slice_id, int *bb_count, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT count(*) FROM bounding_box WHERE bounding_box.run_slice_id = %d",bb_run_slice_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) bb_count, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_bounding_boxes_count_or_die(sqlite3 *db, int bb_run_slice_id)
{
    int bb_count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_boxes_count(db,bb_run_slice_id, &bb_count, &err_msg), &err_msg);
    return bb_count;
}

int libmzdb_get_cycles_count(sqlite3 *db, int *cycles_count, char **err_msg)
{
    return libmzdb_execute_select_stmt(db, "SELECT cycle FROM spectrum ORDER BY id DESC LIMIT 1", 0, &libmzdb_get_int_cb, (void **) cycles_count, err_msg);
}

int libmzdb_get_cycles_count_or_die(sqlite3 *db)
{
    int cycles_count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_cycles_count(db, &cycles_count, &err_msg), &err_msg);
    return cycles_count;
}

int libmzdb_get_data_encodings_count_from_sequence(sqlite3 *db, int *count,char **err_msg)
{
    return libmzdb_get_table_records_count(db, "data_encoding", count, err_msg);
}

int libmzdb_get_data_encodings_count_from_sequence_or_die(sqlite3 *db)
{
    int count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_data_encodings_count_from_sequence(db, &count, &err_msg), &err_msg);
    return count;
}

int libmzdb_get_spectra_count_from_sequence(sqlite3 *db, int *spectra_count, char **err_msg)
{
    return libmzdb_get_table_records_count(db, "spectrum", spectra_count, err_msg);
}

int libmzdb_get_spectra_count_from_sequence_or_die(sqlite3 *db)
{
    int spectra_count;
    char *err_msg;
    int rc = libmzdb_get_spectra_count_from_sequence(db, &spectra_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return spectra_count;
}

int libmzdb_get_spectra_count(sqlite3 *db, int ms_level, int *spectra_count, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT count(id) FROM spectrum WHERE ms_level = %d", ms_level);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) spectra_count, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_spectra_count_or_die(sqlite3 *db, int ms_level)
{
    int spectra_count;
    char *err_msg;
    int rc = libmzdb_get_spectra_count(db, ms_level, &spectra_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return spectra_count;
}

int libmzdb_get_run_slices_count_from_sequence(sqlite3 *db, int *run_slices_count, char **err_msg)
{
    return libmzdb_get_table_records_count(db, "run_slice", run_slices_count, err_msg);
}

int libmzdb_get_run_slices_count_from_sequence_or_die(sqlite3 *db)
{
    int run_slices_count;
    char *err_msg;
    int rc = libmzdb_get_run_slices_count_from_sequence(db, &run_slices_count, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return run_slices_count;
}

int libmzdb_get_table_records_count(sqlite3 *db, char *table_name, int *table_records_count, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT seq FROM sqlite_sequence WHERE name = '%s'", table_name);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) table_records_count, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_table_records_count_or_die(sqlite3 *db, char *table_name)
{
    int table_records_count;
    char *err_msg;
    libmzdb_die_on_sqlite_error(libmzdb_get_table_records_count(db, table_name, &table_records_count, &err_msg), &err_msg);
    return table_records_count;
}


int libmzdb_get_bounding_box_data(sqlite3 *db, int bb_id, byte **blob, int *blob_length, char **err_msg)
{
    sqlite3_blob *blob_handle;
    int rc;
    *err_msg = "Error : get bbox data : cannot open the blob";
    if ((rc = sqlite3_blob_open(db, "main", "bounding_box", "data", bb_id, 0, &blob_handle)) != SQLITE_OK) return rc;

    *blob_length = sqlite3_blob_bytes(blob_handle);
    *blob = (byte*)calloc(*blob_length, sizeof(byte));
    rc = sqlite3_blob_read(blob_handle, *blob, *blob_length, 0);
    sqlite3_blob_close(blob_handle);
    *err_msg = NO_ERR_MSG;
    return rc;
}

byte *libmzdb_get_bounding_box_data_or_die(sqlite3 *db, int bb_id, int *blob_length)
{
    byte *blob = NULL;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_data(db, bb_id, &blob, blob_length, &err_msg), &err_msg);
    return blob;
}

int libmzdb_get_chromatogram_data_points(sqlite3 *db, int c_id, libmzdb_data_points_32_32_t **data, int *data_points_count, char **err_msg)
{
    byte* blob = NULL;
    char* z_sql = sqlite3_mprintf("SELECT data_points FROM chromatogram WHERE chromatogram.id = %d", c_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 3, &libmzdb_get_blob_cb, (void **) blob, err_msg);

    *data_points_count = sizeof(byte) / sizeof(libmzdb_data_points_32_32_t);
    *data = (libmzdb_data_points_32_32_t*) calloc((*data_points_count), sizeof(libmzdb_data_points_32_32_t));
    *data = (libmzdb_data_points_32_32_t*) blob; //FIXME

    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_chromatogram_data_points_or_die(sqlite3 *db, libmzdb_data_points_32_32_t** data_points,  int c_id)
{
    char *err_msg = NULL;
    int point_count;
    libmzdb_die_on_sqlite_error(libmzdb_get_chromatogram_data_points(db, c_id, data_points, &point_count, &err_msg), &err_msg);
    return point_count;
}

int libmzdb_get_bounding_box_first_spectrum_id(sqlite3 *db, int first_id, long *bb_first_spectrum_id, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT bb_first_spectrum_id FROM spectrum WHERE id = %d", first_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) bb_first_spectrum_id, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

long libmzdb_get_bounding_box_first_spectrum_id_or_die(sqlite3 *db, int first_id)
{
    long bb_first_spectrum_id;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_first_spectrum_id(db, first_id, &bb_first_spectrum_id, &err_msg), &err_msg);
    return bb_first_spectrum_id;
}

int libmzdb_get_bounding_box_min_mz(sqlite3 *db, int bb_r_tree_id, float *bb_min_mz, char **err_msg)
{
    char* z_sql = sqlite3_mprintf("SELECT min_mz FROM bounding_box_rtree WHERE bounding_box_rtree.id = %d", bb_r_tree_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_float_cb, (void **) bb_min_mz, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

float libmzdb_get_bounding_box_min_mz_or_die(sqlite3 *db, int bb_r_tree_id)
{
    float bbmin_mz;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_min_mz(db, bb_r_tree_id, &bbmin_mz, &err_msg), &err_msg);
    return bbmin_mz;
}

int libmzdb_get_bounding_box_min_time(sqlite3 *db, int bb_r_tree_id, float *bb_min_time, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT min_time FROM bounding_box_rtree WHERE bounding_box_rtree.id = %d", bb_r_tree_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_float_cb, (void **) bb_min_time, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

float libmzdb_get_bounding_box_min_time_or_die(sqlite3 *db, int bb_r_tree_id)
{
    float bb_min_time;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_bounding_box_min_time(db, bb_r_tree_id, &bb_min_time, &err_msg), &err_msg);
    return bb_min_time;
}

int libmzdb_get_run_slice_id(sqlite3 *db, int bb_id, int *run_slice_id, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT run_slice_id FROM bounding_box WHERE id = %d", bb_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) run_slice_id, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_run_slice_id_or_die(sqlite3 *db, int bb_id)
{
    int run_slice_id;
    char *err_msg = NULL;
    libmzdb_die_on_sqlite_error(libmzdb_get_run_slice_id(db, bb_id, &run_slice_id, &err_msg), &err_msg);
    return run_slice_id;
}

int libmzdb_get_ms_level_from_run_slice_id_manually(sqlite3 *db, int run_slice_id, int *ms_level, char **err_msg)
{
    char *z_sql = sqlite3_mprintf("SELECT ms_level FROM run_slice WHERE run_slice.id = %d", run_slice_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) ms_level, err_msg);
    sqlite3_free(z_sql);
    return rc;
}

int libmzdb_get_ms_level_from_run_slice_id_manually_or_die(sqlite3 *db, int run_slice_id)
{
    int ms_level;
    char *err_msg = NULL;
    int rc = libmzdb_get_ms_level_from_run_slice_id_manually(db, run_slice_id, &ms_level, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return ms_level;
}

int libmzdb_get_bounding_box_ms_level(sqlite3 *db, int bb_id, int *result_ms_level,char **err_msg)
{
    int *first_result = 0;
    char *z_sql = sqlite3_mprintf("SELECT run_slice_id FROM bounding_box WHERE id = %d", bb_id);
    int rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) &first_result, err_msg);
    sqlite3_free(z_sql); //mem leak?
    if(rc != SQLITE_OK) return rc;

    z_sql = sqlite3_mprintf("SELECT ms_level FROM run_slice WHERE run_slice.id = %d", first_result);
    rc = libmzdb_execute_select_stmt(db, z_sql, 0, &libmzdb_get_int_cb, (void **) result_ms_level, err_msg);
    sqlite3_free(z_sql);
    return SQLITE_OK;
}

int get_bounding_box_ms_level_or_die(sqlite3 *db, int bb_id)
{
    int result_ms_level;
    char *err_msg = NULL;
    int rc = libmzdb_get_bounding_box_ms_level(db, bb_id, &result_ms_level, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return result_ms_level;
}

int libmzdb_get_data_encoding_id(sqlite3 *db, int bounding_box_id, int *data_encoding_id, char **err_msg)
{
    char *sql_str_encoding_id = sqlite3_mprintf("SELECT s.data_encoding_id FROM spectrum s, bounding_box b WHERE b.id = %d AND b.first_spectrum_id = s.id", bounding_box_id);
    int rc = libmzdb_execute_select_stmt(db, sql_str_encoding_id, 0, &libmzdb_get_int_cb, (void **) data_encoding_id, err_msg);
    sqlite3_free(sql_str_encoding_id);

    return rc;
}

int libmzdb_get_data_encoding_id_or_die(sqlite3 *db, int bounding_box_id)
{
    int data_encoding_id;
    char *err_msg = NULL;
    int rc = libmzdb_get_data_encoding_id(db, bounding_box_id, &data_encoding_id, &err_msg);
    libmzdb_die_on_sqlite_error(rc, &err_msg);
    return data_encoding_id;
}

#include "windows.h"

/**
 * @brief _get_data_enconding_count private function : get the number of data encoding
 * @param db the database on which the SQL executes
 * @return the data encoding count
 */
int _get_data_enconding_count(sqlite3 *db)
{
    int rc;
    int stmt_step_res;
    sqlite3_stmt* stmt;

    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM data_encoding", -1, &stmt, NULL))!=SQLITE_OK) return -1;
    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return -1;
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt); //delete the stmt*/

    // FIXME: should work
    /*rc = execute_select_first_field_stmt(db, "SELECT count(id) FROM data_encoding", &get_int_cb, &count, NULL);
    if (rc!=SQLITE_OK) return -1;*/

    return count;
}

/**
 * @brief get_data_encodings get an array of each data encoding
 * @param db the database on which the SQL executes
 * @param data_encodings an array which contains each data encoding
 * @param de_count the number of data encoding
 * @return SQLITE_OK if no error occured
 */
int get_data_encodings(sqlite3 *db, libmzdb_data_encoding_t **data_encodings, int *de_count)
{
    int rc;
    int stmt_step_res;
    sqlite3_stmt* stmt;

    *de_count = _get_data_enconding_count(db);
    if(*de_count < 0) return SQLITE_ERROR;

    libmzdb_data_encoding_t* de = (libmzdb_data_encoding_t*) malloc(sizeof(libmzdb_data_encoding_t)* (*de_count));

    const unsigned char *mode, *byte_order; //*compression => was set and not used
    int mz_precision, intensity_precision, id;

    if ((rc= sqlite3_prepare_v2(db, "SELECT * FROM data_encoding", -1, &stmt, NULL))!=SQLITE_OK) return rc;

    int index = -1;
    while((stmt_step_res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        id = sqlite3_column_int(stmt, 0);
        mode = sqlite3_column_text(stmt, 1);
        //compression = sqlite3_column_text(stmt, 2);
        byte_order = sqlite3_column_text(stmt, 3);
        mz_precision = sqlite3_column_int(stmt, 4);
        intensity_precision = sqlite3_column_int(stmt, 5);

        index++;
        de[index].id = id;

        //printf("de mode str from loader: %s \n " , mode);

        if (strcmp((const char *) mode, "fitted") == 0) de[index].mode = FITTED;
        else if (strcmp((const char *) mode, "centroid") == 0) de[index].mode = CENTROID;
        else de[index].mode = PROFILE;

        //printf("de id from loader: %d \n " , de[index].id);
        //printf("de mode  from loader: %d \n " , de[index].mode);

        de[index].byte_order = (strcmp((const char *) byte_order, "little_endian") == 0) ? LITTLE_ENDIAN : BIG_ENDIAN;

        if (mz_precision == 32) de[index].peak_encoding = LOW_RES_PEAK;
        else if (intensity_precision == 32) de[index].peak_encoding = HIGH_RES_PEAK;
        else de[index].peak_encoding = NO_LOSS_PEAK;
    }

    *data_encodings = de;

    return SQLITE_OK;
}

int _get_spectra_data_encoding_ids(sqlite3 *db, int **spectra_data_encoding_ids)
{
    int rc;
    int stmt_step_res;
    sqlite3_stmt* stmt;
    int spectrum_id;
    int de_id;

    int spectra_count = libmzdb_get_spectra_count_from_sequence_or_die(db);
    int* local_spectra_data_encoding_ids = (int*) malloc(sizeof(int) * (1 + spectra_count) );
    memset(local_spectra_data_encoding_ids, -1, (1 + spectra_count) * sizeof(int)); // Set all values to -1

    if ((rc= sqlite3_prepare_v2(db, "SELECT id, data_encoding_id FROM spectrum", -1, &stmt, NULL))!=SQLITE_OK) {
        free(local_spectra_data_encoding_ids);
        return rc;
    }

    while((stmt_step_res = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        spectrum_id = sqlite3_column_int(stmt, 0);
        de_id = sqlite3_column_int(stmt, 1);
        local_spectra_data_encoding_ids[spectrum_id] = de_id;
    }

    *spectra_data_encoding_ids = local_spectra_data_encoding_ids;

    return SQLITE_OK;
}

int libmzdb_get_data_encodings_cache(sqlite3 *db, libmzdb_data_encodings_cache_t** data_encodings_cache)
{
    int rc;
    libmzdb_data_encodings_cache_t* cache = (libmzdb_data_encodings_cache_t*) malloc(sizeof(libmzdb_data_encodings_cache_t));

    if ((rc = get_data_encodings(db, &cache->data_encodings, &cache->de_size))!=SQLITE_OK) {
        free(cache);
        return rc;
    }

    int de_size = cache->de_size;
    libmzdb_data_encoding_t last_de = cache->data_encodings[de_size - 1];
    int mapping_size = last_de.id + 1;
    cache->mapping_size = mapping_size;

    int* data_encoding_id_mapping = (int*) malloc(sizeof(int) * mapping_size);
    memset(data_encoding_id_mapping, -1, mapping_size * sizeof(int)); // Set all values to -1

    for(int i=0; i<de_size; ++i) data_encoding_id_mapping[cache->data_encodings[i].id] = i;

    cache->data_encoding_id_mapping = data_encoding_id_mapping;

    int* spectra_data_encoding_ids;
    _get_spectra_data_encoding_ids(db, &spectra_data_encoding_ids);

    cache->spectrum_id_to_data_encoding_id = spectra_data_encoding_ids;

    *data_encodings_cache = cache;

    return SQLITE_OK;
}

libmzdb_data_encoding_t* libmzdb_get_data_encoding_from_cache(libmzdb_data_encodings_cache_t* data_encodings_cache, int data_encoding_id)
{
    if (data_encoding_id < 1 || data_encoding_id >= data_encodings_cache->mapping_size) return NULL;

    int de_index = data_encodings_cache->data_encoding_id_mapping[data_encoding_id];
    return &data_encodings_cache->data_encodings[de_index];

}

libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data(
        const byte* bb_bytes,
        const int peaks_start_pos,
        const int peaks_count,
        const libmzdb_data_encoding_t de,
        const double min_mz,
        const double max_mz
        ) {
    const libmzdb_data_mode_t data_mode = de.mode;
    const libmzdb_peak_encoding_t pe = de.peak_encoding;
    const int peak_size = data_mode == FITTED ? pe + 8 : pe;
    const int peaks_bytes_length = peaks_count * peak_size;

    int filtered_peaks_count = 0;
    int filtered_peaks_start_idx = 0;

    // If no m/z range is provided
    if (min_mz < 0 && max_mz < 0)
    {
        // Compute the peaks count for the whole spectrum slice
        filtered_peaks_count = peaks_count;
        // Set peaks_start_idx value to spectrum_slice_start_pos
        filtered_peaks_start_idx = peaks_start_pos;
        // Else determine the peaks_start_idx and peaks_count corresponding to provided m/z filters
    }
    else
    {
        // Determine the max m/z threshold to use
        double max_mz_threshold = (max_mz < 0) ? DBL_MAX : max_mz;
        //        int peaks_start_pos_as_ints = peaks_start_pos * 0.25;

        for (int i = 0; i < peaks_bytes_length; i += peak_size)
        {
            const int peak_start_pos = peaks_start_pos + i;
            double mz = 0.0;

            switch (pe)
            {
            case LOW_RES_PEAK:
                memcpy( &mz, &bb_bytes[peak_start_pos], 4); // sizeof(float)
                break;
            default:
                memcpy( &mz, &bb_bytes[peak_start_pos], 8); // sizeof(double)
                break;
            }

            // Check if we are in the desired m/z range
            if (mz >= min_mz && mz <= max_mz_threshold)
            {
                // Increment the number of peaks to read
                filtered_peaks_count++;
                // Determine the peaks start idx
                if (mz >= min_mz && filtered_peaks_start_idx == 0) filtered_peaks_start_idx = peak_start_pos;
            }
        }
    }

    // Create new arrays of primitives
    void* mz_array;
    void* intensity_array;
    float* lwhm_array = NULL;
    float* rwhm_array = NULL;

    // Allocate memory for arrays
    switch (pe)
    {
    case HIGH_RES_PEAK:
        mz_array = malloc(sizeof(double) * filtered_peaks_count);
        intensity_array = malloc(sizeof(float) * filtered_peaks_count);
        break;
    case LOW_RES_PEAK:
        mz_array = malloc(sizeof(float) * filtered_peaks_count);
        intensity_array = malloc(sizeof(float) * filtered_peaks_count);
        break;
    default : //case NO_LOSS_PEAK:
        mz_array = malloc(sizeof(double) * filtered_peaks_count);
        intensity_array = malloc(sizeof(double) * filtered_peaks_count);
        break;
    }

    if (data_mode == FITTED) {
        int size_of_hwhms = sizeof(float) * filtered_peaks_count;
        lwhm_array = (float*) malloc(size_of_hwhms);
        rwhm_array = (float*) malloc(size_of_hwhms);
    }


    for (int peak_idx = 0; peak_idx < filtered_peaks_count; peak_idx++ ) {

        int peak_bytes_index = filtered_peaks_start_idx + peak_idx * peak_size;

        // Read m/z and intensity values
        switch (pe)
        {
        case HIGH_RES_PEAK:
            memcpy( &((double*)mz_array)[peak_idx], &bb_bytes[peak_bytes_index], 8); // sizeof(double)
            memcpy( &((float*)intensity_array)[peak_idx], &bb_bytes[peak_bytes_index + 8], 4); // sizeof(float)
            break;
        case LOW_RES_PEAK:
            memcpy( &((float*)mz_array)[peak_idx], &bb_bytes[peak_bytes_index], 4); // sizeof(float)
            memcpy( &((float*)intensity_array)[peak_idx], &bb_bytes[peak_bytes_index + 4], 4); // sizeof(float)
            break;
        case NO_LOSS_PEAK:
            memcpy( &((double*)mz_array)[peak_idx], &bb_bytes[peak_bytes_index], 8); // sizeof(double)
            memcpy( &((double*)intensity_array)[peak_idx], &bb_bytes[peak_bytes_index + 8], 8); // sizeof(double)
            break;
        }

        // Read left and right HWHMs if needed
        if (data_mode == FITTED) {
            memcpy( &lwhm_array[peak_idx], &bb_bytes[peak_bytes_index + pe], 4); // sizeof(float)
            memcpy( &rwhm_array[peak_idx], &bb_bytes[peak_bytes_index + pe + 4], 4); // sizeof(float)
        }
    }

    libmzdb_spectrum_data_t* sd = (libmzdb_spectrum_data_t *) malloc( sizeof(libmzdb_spectrum_data_t) );
    //sd->data_encoding = (data_encoding_t*) malloc(sizeof(data_encoding_t));
    sd->data_encoding = de;


    switch (pe)
    {
    case HIGH_RES_PEAK:
        sd->mz_array_as_doubles = mz_array;
        sd->intensity_array_as_floats = intensity_array;
        break;
    case LOW_RES_PEAK:
        sd->mz_array_as_floats = mz_array;
        sd->intensity_array_as_floats = intensity_array;
        break;
    case NO_LOSS_PEAK:
        sd->mz_array_as_doubles = mz_array;
        sd->intensity_array_as_doubles = intensity_array;
        break;
    }
    if (data_mode == FITTED) {
        sd->lwhm_array = lwhm_array;
        sd->rwhm_array = rwhm_array;
    }

    sd->peak_count = peaks_count;
    // return the newly created spectrum data
    return sd;
}

libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data_at(
        const libmzdb_indexed_bounding_box_t indexed_bbox,
        libmzdb_data_encodings_cache_t cache,
        const int spectrum_slice_idx,
        const long spectrum_id,
        const double min_mz,
        const double max_mz
        ) {

    // Determine peak size in bytes
    const libmzdb_data_encoding_t de = *libmzdb_get_data_encoding_from_cache(&cache, cache.spectrum_id_to_data_encoding_id[spectrum_id]);

    // Retrieve the number of peaks
    const int peaks_count = indexed_bbox.peaks_counts[spectrum_slice_idx];

    // Skip spectrum id and peaks count (two integers)
    const int peaks_start_pos = indexed_bbox.slices_indexes[spectrum_slice_idx] + 8;

    // Instantiate a new SpectrumData for the corresponding spectrum slice
    return libmzdb_read_spectrum_slice_data(indexed_bbox.bb.blob, peaks_start_pos, peaks_count, de, min_mz, max_mz);
}

libmzdb_bounding_box_t libmzdb_build_bbox(sqlite3_stmt* stmt, int do_malloc)
{
    libmzdb_bounding_box_t bbox;
    //cur_bb.first_spectrum_id = first_spec_id; //first spectrum id

    libmzdb_get_int_cb(stmt, 0, 1, &(bbox.id)); //id
    libmzdb_get_int_cb(stmt, 2, 1, &(bbox.run_slice_id)); //run slice id
    libmzdb_get_int_cb(stmt, 3, 1, &(bbox.first_spectrum_id)); //first spectrum id : to check
    libmzdb_get_int_cb(stmt, 4, 1, &(bbox.last_spectrum_id)); //last spectrum id

    // PREVIOUS CODE //
    if (!do_malloc) {
        const void * blob = sqlite3_column_blob(stmt, 1); // get the blob's data
        bbox.blob = (byte*) blob;
        bbox.blob_size = sqlite3_column_bytes(stmt, 1); // get the blob size
    }
    // NEW CODE //
    else {
        /* The pointer returned by sqlite3_column_blob() points to memory
        ** that is owned by the statement handle (pStmt). It is only good
        ** until the next call to an sqlite3_XXX() function (e.g. the
        ** sqlite3_finalize() below) that involves the statement handle.
        ** So we need to make a copy of the blob into memory obtained from
        ** malloc() to return to the caller.
        */
        bbox.blob_size = sqlite3_column_bytes(stmt, 1); // get the blob size

        bbox.blob = (unsigned char*) malloc(bbox.blob_size);
        memcpy(bbox.blob, sqlite3_column_blob(stmt, 1), bbox.blob_size);
    }

    return bbox;
}

libmzdb_indexed_bounding_box_t libmzdb_index_bbox(libmzdb_bounding_box_t bbox, libmzdb_data_encodings_cache_t cache)
{
    libmzdb_indexed_bounding_box_t indexed_bbox;

    int* spectrum_id_to_data_encoding_id = cache.spectrum_id_to_data_encoding_id;
    int estimated_slices_count = 1 + bbox.last_spectrum_id - bbox.first_spectrum_id; //will always be a greater value than the slices count

    int size_of_array = sizeof(int) * estimated_slices_count;
    int* slice_indexes = (int*) malloc(size_of_array);
    int* spectra_ids = (int*) malloc(size_of_array);
    int* peaks_counts = (int*) malloc(size_of_array);

    int bytes_idx = 0; //loop index
    int peak_size, peak_count, spectrum_id;
    int slice_count = 0;
    libmzdb_data_encoding_t de;

    byte* blob = bbox.blob;
    // FIXME: avoid this? => see: https://stackoverflow.com/questions/8703047/how-to-convert-a-byte-array-into-double-in-c
    int* blob_as_ints = (int*)blob; //use to access to the spectrum id and the peak count, a blob will always be a multiple of 4
    int bytes_as_ints_offset = 0;
    int blob_size = bbox.blob_size;

    while (bytes_idx < blob_size) //for each spectrum slice store in the blob
    {
        spectrum_id = blob_as_ints[bytes_as_ints_offset]; //get the current spectrum id in the blob
        spectra_ids[slice_count] = spectrum_id; //stock the spectrum id
        peak_count = blob_as_ints[bytes_as_ints_offset + 1]; //get the current peak count in the blob
        peaks_counts[slice_count] = peak_count; //stock the peak count

        int de_id = spectrum_id_to_data_encoding_id[spectrum_id]; //get the data encoding id which match with the spectrum id in the cache
        de = *libmzdb_get_data_encoding_from_cache(&cache, de_id); //get the data encoding from the cache with its id

        peak_size = (de.mode == FITTED) ? de.peak_encoding + 8 : de.peak_encoding; //increase the peak size if the peaks are fitted
        slice_indexes[slice_count] = bytes_idx; //store the last byte index
        bytes_idx += 8 + (peak_size * peak_count); //shift the byte index with the size (in byte) of the current spectrum slice
        bytes_as_ints_offset = (bytes_idx / 4); //convert the byte index into integer index
        slice_count++;
    }

    // Set fields of indexed_bbox struct
    indexed_bbox.spectrum_slices_count = slice_count;
    indexed_bbox.spectra_ids = spectra_ids;
    indexed_bbox.slices_indexes = slice_indexes;
    indexed_bbox.peaks_counts = peaks_counts;
    indexed_bbox.bb = bbox;

    return indexed_bbox;
}

libmzdb_spectrum_data_t libmzdb_merge_spectrum_slices(libmzdb_spectrum_data_t* sd_slices, int bb_count, int peak_count)
{
    libmzdb_spectrum_data_t merged_sd;

    merged_sd.data_encoding = sd_slices[0].data_encoding;
    merged_sd.peak_count = peak_count;

    switch (merged_sd.data_encoding.peak_encoding)
    {
    case HIGH_RES_PEAK:
        merged_sd.mz_array_as_doubles = (double*) malloc(sizeof(double)*peak_count);
        merged_sd.intensity_array_as_floats = (float*) malloc(sizeof(float)*peak_count);
        break;
    case LOW_RES_PEAK:
        merged_sd.mz_array_as_floats = (float*) malloc(sizeof(float)*peak_count);
        merged_sd.intensity_array_as_floats = (float*) malloc(sizeof(float)*peak_count);
        break;
    case NO_LOSS_PEAK:
        merged_sd.mz_array_as_doubles = (double*) malloc(sizeof(double)*peak_count);
        merged_sd.intensity_array_as_doubles = (double*) malloc(sizeof(double)*peak_count);
        break;
    }
    if (merged_sd.data_encoding.mode == FITTED)
    {
        merged_sd.lwhm_array = (float*) malloc(sizeof(float)*peak_count);
        merged_sd.rwhm_array = (float*) malloc(sizeof(float)*peak_count);
    }

    libmzdb_spectrum_data_t curr_slice;
    int merged_sd_idx = 0;
    for (int i=0; i<bb_count; ++i)
    {
        curr_slice = sd_slices[i];
        switch (merged_sd.data_encoding.peak_encoding)
        {
        case HIGH_RES_PEAK:
            memcpy(&merged_sd.mz_array_as_doubles[merged_sd_idx], curr_slice.mz_array_as_doubles, curr_slice.peak_count * sizeof(double));
            memcpy(&merged_sd.intensity_array_as_floats[merged_sd_idx], curr_slice.intensity_array_as_floats, curr_slice.peak_count * sizeof(float));
            break;
        case LOW_RES_PEAK:
            memcpy(&merged_sd.mz_array_as_floats[merged_sd_idx], curr_slice.mz_array_as_floats, curr_slice.peak_count * sizeof(float));
            memcpy(&merged_sd.intensity_array_as_floats[merged_sd_idx], curr_slice.intensity_array_as_floats, curr_slice.peak_count * sizeof(float));
            break;
        case NO_LOSS_PEAK:
            memcpy(&merged_sd.mz_array_as_doubles[merged_sd_idx], curr_slice.mz_array_as_doubles, curr_slice.peak_count * sizeof(double));
            memcpy(&merged_sd.intensity_array_as_doubles[merged_sd_idx], curr_slice.intensity_array_as_doubles, curr_slice.peak_count * sizeof(double));
            break;
        }
        if (merged_sd.data_encoding.mode == FITTED)
        {
            memcpy(&merged_sd.lwhm_array[merged_sd_idx], curr_slice.lwhm_array, curr_slice.peak_count * sizeof(float));
            memcpy(&merged_sd.rwhm_array[merged_sd_idx], curr_slice.rwhm_array, curr_slice.peak_count * sizeof(float));
        }
        merged_sd_idx += curr_slice.peak_count;
    }
    return merged_sd;
}


int libmzdb_get_spectrum(sqlite3* db, long spectrum_id, libmzdb_entity_cache_t entity_cache, libmzdb_spectrum_t** result)
{
    sqlite3_stmt *stmt;
    int rc, stmt_step_res;
    int bb_count;
    char *err_msg;

    long first_spec_id = libmzdb_get_bounding_box_first_spectrum_id_or_die(db, spectrum_id);

    // Count the number of BBs to be loaded
    char *count_sql_str = sqlite3_mprintf("SELECT count(id) FROM bounding_box WHERE bounding_box.first_spectrum_id = %d", first_spec_id);
    rc = libmzdb_execute_select_stmt(db, count_sql_str, 0, &libmzdb_get_int_cb, (void **) &bb_count, &err_msg);
    sqlite3_free(count_sql_str);
    if (rc != SQLITE_OK) return rc;

    // Load BBs from the DB
    libmzdb_spectrum_data_t* sd_slices = (libmzdb_spectrum_data_t*) malloc(sizeof(libmzdb_spectrum_data_t) * bb_count); //Allocate memory for BBs
    char *sql_str = sqlite3_mprintf("SELECT * FROM bounding_box WHERE bounding_box.first_spectrum_id = %d", first_spec_id);
    rc = sqlite3_prepare_v2(db, sql_str, -1, &stmt, NULL);
    sqlite3_free(sql_str);
    if (rc != SQLITE_OK) return rc;

    libmzdb_data_encodings_cache_t de_cache = entity_cache.data_encodings_cache;
    const int min_mz = -1;
    const int max_mz = -1;
    int target_slice_idx = -1;
    int bb_index=0;

    while ((stmt_step_res=sqlite3_step(stmt)) == SQLITE_ROW) //for each bounding box, will collect the data of the spectrum
    {
        libmzdb_bounding_box_t cur_bb = libmzdb_build_bbox(stmt, 0);
        libmzdb_indexed_bounding_box_t indexed_bb = libmzdb_index_bbox(cur_bb, de_cache);

        // Search for slice index corresponding to spectrum_id
        if (target_slice_idx == -1)
            for (int cur_slice_idx=0; cur_slice_idx<indexed_bb.spectrum_slices_count; ++cur_slice_idx) //add a break ?
                if (indexed_bb.spectra_ids[cur_slice_idx] == spectrum_id)
                {
                    target_slice_idx = cur_slice_idx;
                    break;
                }
        // TODO: add log ERROR
        if (target_slice_idx == -1) return -1; //no target slice index has been found => ERROR

        //printf("spectrum_idx=%d\n",target_slice_idx);
        //printf("spectrum_id=%d\n",spectrum_id);

        /*printf("blob content: ");
        for (int i=0; i<indexed_bb.bb.blob_size; ++i)
          printf("%d ",indexed_bb.bb.blob[i]);

        printf("\n");*/

        // Add sd to sd_slices
        libmzdb_spectrum_data_t* buffer = libmzdb_read_spectrum_slice_data_at(indexed_bb, de_cache, target_slice_idx, spectrum_id, min_mz, max_mz);
        sd_slices[bb_index] = *buffer;
        free(buffer); //the buffer is not needed, but the content must not be free

        //free the indexed bbox content
        free(indexed_bb.peaks_counts);
        free(indexed_bb.slices_indexes);
        free(indexed_bb.spectra_ids);

        bb_index++;
    }
    sqlite3_finalize(stmt);

    //calculate the number of peak of the computed spectrum
    int peak_count = 0;
    for (int i=0; i<bb_count; ++i) peak_count += sd_slices[i].peak_count;

    libmzdb_spectrum_data_t sdata = libmzdb_merge_spectrum_slices(sd_slices, bb_count, peak_count);

    //free the spectrum data slices buffer

    for(int i=0; i<bb_count; ++i) libmzdb_free_spectrum_data_content(sd_slices[i]);
    free(sd_slices);

    // Create spectrum and return it
    libmzdb_spectrum_t* spectrum = (libmzdb_spectrum_t*) malloc(sizeof(libmzdb_spectrum_t));
    spectrum->header    = entity_cache.spectrum_headers[spectrum_id - 1];
    spectrum->data      = sdata;
    *result = spectrum; //store the spectrum in the result pointer

    return SQLITE_OK;
}




void libmzdb_free_spectrum_data_content(libmzdb_spectrum_data_t spectrum_data)
{
    switch (spectrum_data.data_encoding.peak_encoding)
    {
    case HIGH_RES_PEAK:
        if (spectrum_data.intensity_array_as_floats) free(spectrum_data.intensity_array_as_floats);
        if (spectrum_data.mz_array_as_doubles) free(spectrum_data.mz_array_as_doubles);
        break;
    case LOW_RES_PEAK:
        if (spectrum_data.intensity_array_as_floats) free(spectrum_data.intensity_array_as_floats);
        if (spectrum_data.mz_array_as_floats) free(spectrum_data.mz_array_as_floats);
        break;
    case NO_LOSS_PEAK:
        if (spectrum_data.intensity_array_as_doubles) free(spectrum_data.intensity_array_as_doubles);
        if (spectrum_data.mz_array_as_doubles) free(spectrum_data.mz_array_as_doubles);
        break;
    }

    if (spectrum_data.data_encoding.mode == FITTED)
    {
        if (spectrum_data.lwhm_array) free(spectrum_data.lwhm_array);
        if (spectrum_data.rwhm_array) free(spectrum_data.rwhm_array);
    }
}

void libmzdb_free_spectrum_header_content(libmzdb_spectrum_header_t spectrum_header)
{
    if(spectrum_header.title) free(spectrum_header.title);
    if(spectrum_header.activation_type) free(spectrum_header.activation_type);
    if(spectrum_header.scan_list_str) free(spectrum_header.scan_list_str);
    if(spectrum_header.precursor_list_str) free(spectrum_header.precursor_list_str);
    if(spectrum_header.product_list_str) free(spectrum_header.precursor_list_str);
    if(spectrum_header.param_tree_str) free(spectrum_header.param_tree_str); //TODO: Error
}

void libmzdb_free_spectrum(libmzdb_spectrum_t* spectrum)
{
    libmzdb_free_spectrum_data_content(spectrum->data);
    //free_spectrum_header_content(spectrum->header); => spectrum headers are saved in the entity cache
    free(spectrum);
}



















































































///**
// * @brief fill_peaks64_32_callback Callback for getAllFromBoundingBox. Specific to 64_32 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_peaks64_32_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int peak_count;
//    data_point_64_32_t *peaks64_32 = NULL;

//    spectrum_data->data_precision = DATA_PRECISION_64_32;
//    peak_count = ((int*)blob)[1];

//    spectrum_data->peak_count = peak_count;
//    spectrum_data->peaks64_32 = (data_point_64_32_t*) malloc(sizeof(data_point_64_32_t) * peak_count);
//    peaks64_32 = (data_point_64_32_t*)((int*)blob + 2);

//    memcpy(spectrum_data->peaks64_32, peaks64_32, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaks64_32[i].x = peaks64_32[i].x;
//    spectrum_data->peaks64_32[i].y = peaks64_32[i].y;
//  }
//  */
//}

////Not tested!
///**
// * @brief fill_peaks64_64_callback Callback for getAllFromBoundingBox. Specific to 64_64 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_peaks64_64_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int peak_count;
//    data_point_64_64_t *peaks64_64 = NULL;

//    spectrum_data->data_precision = DATA_PRECISION_64_64;

//    peak_count = ((int*)blob)[1];
//    // convert blob to 64_64 peaks
//    // id = ((int*)blob)[0]); // [unused]
//    spectrum_data->peak_count = peak_count;
//    spectrum_data->peaks64_64 = (data_point_64_64_t*)malloc(sizeof(data_point_64_64_t) * peak_count);
//    peaks64_64 = (data_point_64_64_t*)((int*)blob + 2);

//    memcpy(spectrum_data->peaks64_64, peaks64_64, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaks64_64[i].x = peaks64_64[i].x;
//    spectrum_data->peaks64_64[i].y = peaks64_64[i].y;
//  }
//  */
//}

///**
// * @brief fill_peaks32_32_callback Callback for getAllFromBoundingBox. Specific to 32_32 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_peaks32_32_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int peak_count;
//    data_point_32_32_t *peaks32_32;

//    spectrum_data->data_precision = DATA_PRECISION_32_32;
//    peak_count = ((int*)blob)[1];

//    spectrum_data->peak_count = peak_count;
//    spectrum_data->peaks32_32 = (data_point_32_32_t*)malloc(sizeof(data_point_32_32_t) * peak_count);
//    peaks32_32 = (data_point_32_32_t*)((int*)blob + 2);

//    memcpy(spectrum_data->peaks32_32, peaks32_32, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaks32_32[i].x = peaks32_32[i].x;
//    spectrum_data->peaks32_32[i].y = peaks32_32[i].y;
//  }
//  */
//}

////Not tested
////TO DO : more callback for other data_encoding types when they are used
///**
// * @brief fill_fitted_peaks64_32_callback Callback for getAllFromBoundingBox. Specific to fitted 64_32 peaks
// * @param blob
// * @param spectrum_data
// */
//void fill_fitted_peaks64_32_callback(byte *blob, spectrum_peaks_t *spectrum_data)
//{
//    int i;
//    int peak_count;
//    fitted_peak_t *fitted_peaks;

//    spectrum_data->data_precision = DATA_PRECISION_64_32;
//    peak_count = ((int*)blob)[1];

//    spectrum_data->peak_count = peak_count;
//    spectrum_data->fitted_peaks = (fitted_peak_t*) malloc(sizeof(fitted_peak_t) * peak_count);
//    fitted_peaks = (fitted_peak_t*)((int*)blob + 2);

//    memcpy(spectrum_data->fitted_peaks, fitted_peaks, peak_count);
//    /*
//  for (i = 0; i < peak_count; i++)
//  {
//    spectrum_data->peaksFitted64_32[i].x = fittedPeaks64_32[i].x;
//    spectrum_data->peaksFitted64_32[i].y = fittedPeaks64_32[i].y;
//  }
//  */
//}

////return NULL if DataEncoding is not managed
////TO DO : Create cache to avoid sqlQuerie at each call
///**
// * @brief get_fill_peaks_callback Function returning a callback pointer depending on the Data Encoding Id
// * @param data_precision
// * @return
// */
//fill_peaks_callback_ptr get_fill_peaks_callback(data_precision_enum data_precision)
//{
//    switch (data_precision)
//    {
//    case DATA_PRECISION_32_32: return fill_peaks32_32_callback;
//    case DATA_PRECISION_64_32: return fill_peaks64_32_callback;
//    case DATA_PRECISION_64_64: return fill_peaks64_64_callback;
//    }
//    return NULL;
//}

//int _get_data_point_memory_size(data_precision_enum data_precision)
//{
//    switch (data_precision)
//    {
//    case DATA_PRECISION_32_32:return sizeof(data_point_32_32_t);
//    case DATA_PRECISION_64_32:return sizeof(data_point_64_32_t);
//    case DATA_PRECISION_64_64:return sizeof(data_point_64_64_t);
//    case DATA_PRECISION_FITTED_64_32:return sizeof(fitted_peak_t);
//    }
//    return 0;
//}


//double get_x_value(spectrum_peaks_t spectrum_data, int index)
//{
//    switch (spectrum_data.data_precision)
//    {
//    case DATA_PRECISION_32_32:return spectrum_data.peaks32_32[index].x;
//    case DATA_PRECISION_64_32:return spectrum_data.peaks64_32[index].x;
//    case DATA_PRECISION_64_64:return spectrum_data.peaks64_64[index].x;
//    }
//    return 0;
//}

//double get_y_value(spectrum_peaks_t spectrum_data, int index)
//{
//    switch (spectrum_data.data_precision)
//    {
//    case DATA_PRECISION_32_32:return spectrum_data.peaks32_32[index].y;
//    case DATA_PRECISION_64_32:return spectrum_data.peaks64_32[index].y;
//    case DATA_PRECISION_64_64:return spectrum_data.peaks64_64[index].y;
//    }
//    return 0;
//}

///**
// * @brief get_left_hwhm_value get left hwhm value value from spectrum_data_t depending of its DATA_PRECISION (only needed in fitted mode)
// * @param spectrum_data
// * @param index
// * @param data_precision
// * @return
// */
//// TODO: private
//double get_left_hwhm_value(spectrum_peaks_t spectrum_data, int index, data_precision_enum data_precision)
//{
//    return (data_precision==DATA_PRECISION_FITTED_64_32) ? spectrum_data.fitted_peaks[index].left_hwhm : 0;
//}

///**
// * @brief get_right_hwhm_value get RightHwhmValue value from spectrum_data_t depending of its DATA_PRECISION (only needed in fitted mode)
// * @param spectrum_data
// * @param index
// * @param data_precision
// * @return
// */
//// TODO: private
//double get_right_hwhm_value(spectrum_peaks_t spectrum_data, int index, data_precision_enum data_precision)
//{
//    return (data_precision==DATA_PRECISION_FITTED_64_32) ? spectrum_data.fitted_peaks[index].right_hwhm : 0;
//}


//int get_bounding_box_data_v1(sqlite3 *db, int bb_id, byte **blob, int *blob_length, char **err_msg)
//{
//    sqlite3_blob *blob_handle;
//    int rc;
//    if ((rc = sqlite3_blob_open(db, "main", "bounding_box", "data", bb_id, 0, &blob_handle)) != SQLITE_OK) return rc;

//    *blob_length = sqlite3_blob_bytes(blob_handle);
//    *blob = (byte*)calloc(*blob_length, sizeof(byte));
//    rc = sqlite3_blob_read(blob_handle, *blob, *blob_length, 0);
//    sqlite3_blob_close(blob_handle);
//    return rc;
//}

//int get_data_encoding_id_(sqlite3 *db, int spectrum_id, int *data_encoding_id, char **err_msg)
//{
//    char *sql_str_encoding_id = sqlite3_mprintf("SELECT data_encoding_id FROM spectrum WHERE id=%d", spectrum_id);
//    int rc = execute_select_stmt(db, sql_str_encoding_id, 0, &get_int_cb, data_encoding_id, err_msg);
//    sqlite3_free(sql_str_encoding_id);
//    return rc;
//}

//int get_data_encoding_id_or_die_(sqlite3 *db, int spectrum_id)
//{
//    char* err_msg = NULL;
//    int res;
//    int rc = get_data_encoding_id(db, spectrum_id, &res, &err_msg);
//    die_on_sqlite_error(rc, err_msg);
//    return res;
//}

//int _get_bbox_count(sqlite3* db, int* bb_count)
//{
//    sqlite3_stmt* stmt;
//    int rc, stmt_step_res;

//    if ((rc= sqlite3_prepare_v2(db, "SELECT count(id) FROM bounding_box", -1, &stmt, NULL))!=SQLITE_OK) return rc;
//    if ((stmt_step_res= sqlite3_step(stmt))!=SQLITE_ROW) return rc;

//    *bb_count = sqlite3_column_int(stmt, 0);
//    sqlite3_finalize(stmt); //delete the stmt

//    return rc;
//}

//indexed_bounding_box_t index_bbox_old(bounding_box_t bbox, data_encodings_cache_t de_cache)
//{
//    indexed_bounding_box_t indexed_bb;
//    _index_each_blob_id(bbox, &indexed_bb, de_cache); //calculate the mapping
//    return indexed_bb;
//}







//int _index_each_blob_id(const bounding_box_t bbox, indexed_bounding_box_t *indexed_bbox, const data_encodings_cache_t cache)
//{
//    int* spectrum_id_to_data_encoding_id = cache.spectrum_id_to_data_encoding_id;
//    int estimated_slices_count = 1 + bbox.last_spectrum_id - bbox.first_spectrum_id; //will always be a greater value than the slices count

//    int size_of_array = sizeof(int) * estimated_slices_count;
//    int* slice_indexes = (int*) malloc(size_of_array);
//    int* spectra_ids = (int*) malloc(size_of_array);
//    int* peaks_counts = (int*) malloc(size_of_array);

//    int bytes_idx = 0; //loop index
//    int peak_size, peak_count, spectrum_id;
//    int slice_count = 0;
//    data_encoding_t de;

//    byte* blob = bbox.blob;
//    // FIXME: avoid this? => see: https://stackoverflow.com/questions/8703047/how-to-convert-a-byte-array-into-double-in-c
//    int* blob_as_ints = (int*)blob; //use to access to the spectrum id and the peak count, a blob will always be a multiple of 4
//    int bytes_as_ints_offset = 0;
//    int blob_size = bbox.blob_size;

//    while (bytes_idx < blob_size) //for each spectrum slice store in the blob
//    {
//        spectrum_id = blob_as_ints[bytes_as_ints_offset]; //get the current spectrum id in the blob
//        spectra_ids[slice_count] = spectrum_id; //stock the spectrum id
//        peak_count = blob_as_ints[bytes_as_ints_offset + 1]; //get the current peak count in the blob
//        peaks_counts[slice_count] = peak_count; //stock the peak count

//        int de_id = spectrum_id_to_data_encoding_id[spectrum_id]; //get the data encoding id which match with the spectrum id in the cache
//        de = *get_data_encoding_from_cache(&cache, de_id); //get the data encoding from the cache with its id

//        peak_size = (de.mode == FITTED) ? de.peak_encoding + 8 : de.peak_encoding; //increase the peak size if the peaks are fitted
//        slice_indexes[slice_count] = bytes_idx; //store the last byte index
//        bytes_idx += 8 + (peak_size * peak_count); //shift the byte index with the size (in byte) of the current spectrum slice
//        bytes_as_ints_offset = (bytes_idx / 4); //convert the byte index into integer index
//        slice_count++;
//    }

//    // Set fields of indexed_bbox struct
//    indexed_bbox->spectrum_slices_count = slice_count;
//    indexed_bbox->spectra_ids = spectra_ids;
//    indexed_bbox->slices_indexes = slice_indexes;
//    indexed_bbox->peaks_counts = peaks_counts;
//    indexed_bbox->bb = bbox;

//    // return OK
//    return 0;
//}
