#ifndef QUERIES_H
#define QUERIES_H

#include "lib/sqlite/sqlite3.h"
#include "models.h"

// this pragma is needed to pack all data structure without 8bits alignment
#pragma pack(push)
#pragma pack(1)

/**
 * @brief get_model_version return the whole statement to find the model Version
 * @param db the database on which the SQL executes
 * @param model_version the model version retrieved from the mzDB
 * @param err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_model_version(sqlite3* db, char** model_version, char** err_msg);

/**
 * @brief get_model_version_or_die only return the model Version from the previous statement
 * @param db the database on which the SQL executes
 * @return the model version
 */
char* libmzdb_get_model_version_or_die(sqlite3* db);

/**
 * @brief get_param_tree_chromatogram stock the Param Tree values from Chromatogram into param_tree_chro
 * @param db the database on which the SQL executes
 * @param param_tree_chro the param tree of the chromatogram
 * @param err_msg  write the error messages here
 * @return SQLITE_OK if no error occured
 */
int libmzdb_get_param_tree_chromatogram(sqlite3* db, char** param_tree_chro, char** err_msg);
/**
 * @brief get_param_tree_chromatogram_or_die only return the paramTreeChro from the previous statement
 * @param db the database on which the SQL executes
 * @return the param tree
 */
char* libmzdb_get_param_tree_chromatogram_or_die(sqlite3* db);

/**
 * @brief get_param_tree_spectrum stock the Param Tree values from Spectrum into paramTreeSpectrum
 * @param db the database on which the SQL executes
 * @param param_tree_id the param tree id
 * @param param_tree_spectrum the param tree spectrum
 * @param err_msg err_msg  write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_param_tree_spectrum(sqlite3* db, int param_tree_id, char** param_tree_spectrum, char**err_msg);

/**
 * @brief get_param_tree_spectrum_or_die only return the get param tree spectrum from the previous statement
 * @param db the database on which the SQL executes
 * @return the spectrum param tree
 */
char* libmzdb_get_param_tree_spectrum_or_die(sqlite3* db, int param_tree_id);

/**
 * @brief get_param_tree_mzdb stock the Param Tree values from Mzdb into param_tree_mzdb
 * @param db the database on which the SQL executes
 * @param param_tree_mzdb the param tree
 * @param err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_param_tree_mzdb(sqlite3* db, libmzdb_mzdb_param_tree_t* param_tree_mzdb, char** err_msg);

/**
 * @brief getParamTreeMzdb_or_die only return the paramTreeMzdb values from the previous statement
 * @param db the database on which the SQL executes
 * @return the param tree MZDB
 */
libmzdb_mzdb_param_tree_t libmzdb_get_param_tree_mzdb_or_die(sqlite3 *db);

/**
 * @brief get_pwiz_mzdb_version return the whole statement to find the Pwiz mzdb version
 * @param db the database on which the SQL executes
 * @param mzdb_version the version
 * @param err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_pwiz_mzdb_version(sqlite3* db, char** mzdb_version, char** err_msg);

/**
 * @brief get_pwiz_mzdb_version_or_die only return the PwizMzDbVersion from the previous statement
 * @param db the database on which the SQL executes
 * @return the version
 */
char* libmzdb_get_pwiz_mzdb_version_or_die(sqlite3 *db);

/**
 * @brief get_last_time return the whole statement to find the last Time from spectrum table
 * @param db the database on which the SQL executes
 * @param last_time the last time
 * @param err_msg err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_last_time(sqlite3* db, double* last_time, char** err_msg);

/**
 * @brief get_last_time_or_die only return the lastTime from the previous statement
 * @param db the database on which the SQL executes
 * @return the last time
 */
double libmzdb_get_last_time_or_die(sqlite3* db);

/**
 * @brief get_max_ms_level return the whole statement to find the max_ms_level from run_slice table
 * @param db the database on which the SQL executes
 * @param  the max ms level
 * @param err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_max_ms_level(sqlite3* db, int* , char** err_msg);

/**
 * @brief get_max_ms_level_or_die only return the max_ms_level from the previous statement
 * @param db the database on which the SQL executes
 * @return the max ms level
 */
int libmzdb_get_max_ms_level_or_die(sqlite3 *db);

/**
 * @brief get_bounding_boxes_count_from_sequence uses the method get_table_records_count to specifically return the Table Records Count from the bounding_box table
 * @param db the database on which the SQL executes
 * @param bb_count the bounding boxe count
 * @param err_msg err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_bounding_boxes_count_from_sequence(sqlite3 *db,int *bb_count,char **err_msg);

/**
 * @brief get_bounding_boxes_count_from_sequence_or_die only return the Bounding Boxes count from the previous statement
 * @param db the database on which the SQL executes
 * @return the bounding box count
 */
int libmzdb_get_bounding_boxes_count_from_sequence_or_die(sqlite3 *db);

/**
 * @brief get_mz_range hte mz range depending on the ms level
 * @param db the database on which the SQL executes
 * @param ms_level the ms level
 * @param min_max_mz the min
 * @param err_msg err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_mz_range(sqlite3 *db, int ms_level,int **min_max_mz,char **err_msg);

/**
 * @brief get_mz_range_or_die
 * @param db the database on which the SQL executes
 * @param ms_level the ms level
 * @return the mz_range
 */
int *libmzdb_get_mz_range_or_die(sqlite3 *db, int ms_level);

/**
 * @brief get_bounding_boxes_count return the whole statement to find the BoundingBoxesCount from bounding_box table where the id is given by the user
 * @param db the database on which the SQL executes
 * @param bb_run_slice_id hte bb run slice id
 * @param bb_count the result
 * @param err_msg err_msg write error messages here
 * @return SQLITE_OK if no error
 */
int libmzdb_get_bounding_boxes_count(sqlite3 *db, int bb_run_slice_id, int *bb_count, char **err_msg);

/**
 * @brief get_bounding_boxes_count_or_die Only return the bb_count from the previous statement
 * @param db the database on which the SQL executes
 * @return the bounding boxes count
 */
int libmzdb_get_bounding_boxes_count_or_die(sqlite3 *db, int bb_run_slice_id);

/**
 * @brief get_cycles_count Return the whole statement to find the Cycles Count from spectrum table
 * @param db the database on which the SQL executes
 * @param cycles_count
 * @param err_msg
 * @return SQLITE_OK if no error
 */
int libmzdb_get_cycles_count(sqlite3 *db, int *cycles_count, char **err_msg);

/**
 * @brief get_cycles_count_or_die Only return the cycles_count from the previous statement
 * @param db the database on which the SQL executes
 * @return
 */
int libmzdb_get_cycles_count_or_die(sqlite3 *db);

/**
 * @brief get_data_encodings_count_from_sequence Uses the method get_table_records_count to specifically return the Data Encodings Count from the data_encoding table
 * @param db the database on which the SQL executes
 * @param count
 * @param err_msg
 * @return
 */
int libmzdb_get_data_encodings_count_from_sequence(sqlite3 *db, int *count,char **err_msg);

/**
 * @brief get_data_encodings_count_from_sequence_or_die Only return the Data Encodings count from the previous statement
 * @param db the database on which the SQL executes
 * @return
 */
int libmzdb_get_data_encodings_count_from_sequence_or_die(sqlite3 *db);

/**
 * @brief get_spectra_count_from_sequence Uses the method get_table_records_count to specifically return the Spectra Count from the spectrum table
 * @param db the database on which the SQL executes
 * @param spectra_count
 * @param err_msg
 * @return
 */
int libmzdb_get_spectra_count_from_sequence(sqlite3 *db, int *spectra_count, char **err_msg);

/**
 * @brief get_spectra_count_from_sequence_or_die Only return the Spectra count from the previous statement
 * @param db the database on which the SQL executes
 * @return
 */
int libmzdb_get_spectra_count_from_sequence_or_die(sqlite3 *db);

/**
 * @brief get_spectra_count Return the whole statement to find the Spectra Count from spectrum table where the ms_level is given by the user
 * @param db the database on which the SQL executes
 * @param ms_level
 * @param spectra_count
 * @param err_msg
 * @return
 */
int libmzdb_get_spectra_count(sqlite3 *db, int ms_level, int *spectra_count, char **err_msg);

/**
 * @brief get_spectra_count_or_die Only return the spectra_count from the previous statement
 * @param db the database on which the SQL executes
 * @param ms_level
 * @return
 */
int libmzdb_get_spectra_count_or_die(sqlite3 *db, int ms_level);


/**
 * @brief get_run_slices_count_from_sequence Uses the method get_table_records_count to specifically return the Run Slices Count from the run_slice table
 * @param db the database on which the SQL executes
 * @param run_slices_count
 * @param err_msg
 * @return
 */
int libmzdb_get_run_slices_count_from_sequence(sqlite3 *db, int *run_slices_count, char **err_msg);

/**
 * @brief get_run_slices_count_from_sequence_or_die Only return the Run Slices count from the previous statement
 * @param db the database on which the SQL executes
 * @return
 */
int libmzdb_get_run_slices_count_from_sequence_or_die(sqlite3 *db);

/**
 * @brief get_table_records_count Return the sequence from a specific table where the user gives the table name
 * @param db the database on which the SQL executes
 * @param table_name
 * @param table_records_count
 * @param err_msg
 * @return
 */
int libmzdb_get_table_records_count(sqlite3 *db, char *table_name, int *table_records_count, char **err_msg);


/**
 * @brief get_table_records_count_or_die Only return the table record count from the previous statement
 * @param db the database on which the SQL executes
 * @param table_name
 * @return
 */
int libmzdb_get_table_records_count_or_die(sqlite3 *db, char *table_name);

/**
 * @brief get_bounding_box_data Return blobs from the bounding box table where the bounding box id is given by the user.
 * @param db the database on which the SQL executes
 * @param bb_id
 * @param blob
 * @param blob_length
 * @param err_msg
 * @return
 */
int libmzdb_get_bounding_box_data(sqlite3 *db, int bb_id, byte **byte, int *blob_length, char **err_msg);

/**
 * @brief get_bounding_box_data_or_die Only return the data contained in a bounding box from the previous statement
 * @param db the database on which the SQL executes
 * @param bb_id
 * @param blob_length
 * @return
 */
byte *libmzdb_get_bounding_box_data_or_die(sqlite3 *db, int bb_id, int *blob_length);

/**
 * @brief get_chromatogram_data_points Gets an array of points of the chromatogram specified with c_id
 * @param db the database on which the SQL executes
 * @param c_id the chromatogram id
 * @param data the array of data points
 * @param data_points_count the length of the array
 * @param err_msg the error message
 * @return
 */
int libmzdb_get_chromatogram_data_points(sqlite3 *db, int c_id, libmzdb_data_points_32_32_t **data, int *data_points_count, char **err_msg);

/**
 * @brief get_chromatogram_data_points_or_die
 * @param db the database on which the SQL executes
 * @param c_id
 * @return
 */
int libmzdb_get_chromatogram_data_points_or_die(sqlite3 *db, libmzdb_data_points_32_32_t** data_points, int c_id);

/**
 * @brief get_bounding_box_first_spectrum_id Return the whole statement to find the bounding box first spectrum id from spectrum table where the id is given by the user
 * @param db the database on which the SQL executes
 * @param first_id
 * @param bb_first_spectrum_id
 * @param err_msg
 * @return
 */
int libmzdb_get_bounding_box_first_spectrum_id(sqlite3 *db, int first_id, long *bb_first_spectrum_id, char **err_msg);

/**
 * @brief get_bounding_box_first_spectrum_id_or_die Only return the bb_first_spectrum_id from the previous statement
 * @param db the database on which the SQL executes
 * @param first_id
 * @return
 */
long libmzdb_get_bounding_box_first_spectrum_id_or_die(sqlite3 *db, int first_id);

/**
 * @brief get_bounding_box_min_mz Return the whole statement to find the bounding box minimum Mz from bounding_box_rtree table where the bounding box rtree id is given by the user
 * @param db the database on which the SQL executes
 * @param bb_r_tree_id
 * @param bbmin_mz
 * @param err_msg
 * @return
 */
int libmzdb_get_bounding_box_min_mz(sqlite3 *db, int bb_r_tree_id, float *bb_min_mz, char **err_msg);

/**
 * @brief get_bounding_box_min_mz_or_die Only return the bbmin_mz from the previous statement
 * @param db the database on which the SQL executes
 * @param bb_r_tree_id
 * @return
 */
float libmzdb_get_bounding_box_min_mz_or_die(sqlite3 *db, int bb_r_tree_id);

/**
 * @brief get_bounding_box_min_time Return the whole statement to find the bounding box minimum time from bounding_box_rtree table where the bounding box rtree id is given by the user
 * @param db the database on which the SQL executes
 * @param bb_r_tree_id
 * @param bb_min_time
 * @param err_msg
 * @return
 */
int libmzdb_get_bounding_box_min_time(sqlite3 *db, int bb_r_tree_id, float *bb_min_time, char **err_msg);

/**
 * @brief get_bounding_box_min_time_or_die Only return the bb_min_time from the previous statement
 * @param db the database on which the SQL executes
 * @param bb_r_tree_id
 * @return
 */
float libmzdb_get_bounding_box_min_time_or_die(sqlite3 *db, int bb_r_tree_id);

/**
 * @brief get_run_slice_id Return the whole statement to find the run Slice Id from bounding_box table where the id is given by the user
 * @param db the database on which the SQL executes
 * @param bb_id
 * @param run_slice_id
 * @param err_msg
 * @return
 */
int libmzdb_get_run_slice_id(sqlite3 *db, int bb_id, int *run_slice_id, char **err_msg);

/**
 * @brief get_run_slice_id_or_die Only return the Run Slice ID from the previous statement
 * @param db the database on which the SQL executes
 * @param bb_id
 * @return
 */
int libmzdb_get_run_slice_id_or_die(sqlite3 *db, int bb_id);


/**
 * @brief get_ms_level_from_run_slice_id_manually
 * Return the whole statement to find the Ms Level from run_slice table where the run_slice_id is given by the user
 * The user must type the result from "get_run_slice_id" himself to obtain the related Ms Level
 * @param db the database on which the SQL executes
 * @param run_slice_id
 * @param ms_level
 * @param err_msg
 * @return
 */
int libmzdb_get_ms_level_from_run_slice_id_manually(sqlite3 *db, int run_slice_id, int *ms_level, char **err_msg);

/**
 * @brief get_ms_level_from_run_slice_id_manually_or_die
 * @param db the database on which the SQL executes
 * @param run_slice_id
 * @return
 */
int libmzdb_get_ms_level_from_run_slice_id_manually_or_die(sqlite3 *db, int run_slice_id);

/**
 * @brief get_bouding_box_ms_level
 * Combine last two functions in one
 * Get the run slice Id from the bounding_box_id and its associated ms_level
 * @param db the database on which the SQL executes
 * @param bb_id
 * @param result_ms_level
 * @param err_msg
 * @return
 */
int libmzdb_get_bounding_box_ms_level(sqlite3 *db, int bb_id, int *result_ms_level,char **err_msg);

/**
 * @brief get_bouding_box_ms_level_or_die Only return the bounding box ms level from the previous statement
 * @param db the database on which the SQL executes
 * @param bb_id
 * @return
 */
int libmzdb_get_bouding_box_ms_level_or_die(sqlite3 *db, int bb_id);

/**
 * @brief get_data_encoding_id get the data_encoding id from spectrum where the bounding box ID is given by the user
 * @param db the database on which the SQL executes
 * @param bounding_box_id
 * @param data_encoding_id
 * @param err_msg
 * @return
 */
int libmzdb_get_data_encoding_id(sqlite3 *db, int bounding_box_id, int *data_encoding_id, char **err_msg);

/**
 * @brief get_data_encoding_id_or_die Only return the Data Encoding ID from the previous statement
 * @param db the database on which the SQL executes
 * @param bounding_box_id
 * @return
 */
int libmzdb_get_data_encoding_id_or_die(sqlite3 *db, int bounding_box_id);

/**
 * @brief get_data_precision_size_in_bytes get the size of each data Precision (used for storage allocation)
 * @param data_precision
 * @return
 */
int libmzdb_get_data_precision_size_in_bytes(libmzdb_data_precision_enum libmzdb_data_precision);

/**
 * @brief get_spectrum_data
 * @param db
 * @param spectrum_id
 * @param entity_cache
 * @param result
 * @return
 */
int libmzdb_get_spectrum_data(sqlite3* db, long spectrum_id, const libmzdb_entity_cache_t libmzdb_entity_cache, libmzdb_spectrum_data_t **result);

/**
 * @brief get_data_encodings_cache
 * @param db
 * @param data_encodings_cache
 * @return
 */
int libmzdb_get_data_encodings_cache(sqlite3 *db, libmzdb_data_encodings_cache_t** libmzdb_data_encodings_cache);

libmzdb_data_encoding_t* libmzdb_get_data_encoding_from_cache(libmzdb_data_encodings_cache_t* libmzdb_data_encodings_cache, int data_encoding_id);

/**
 * @brief read_spectrum_slice_data_at
 * @param indexed_bbox
 * @param cache
 * @param spectrum_slice_idx
 * @param spectrum_id
 * @param min_mz
 * @param max_mz
 * @return
 */
libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data_at(
        const libmzdb_indexed_bounding_box_t indexed_bbox,
        libmzdb_data_encodings_cache_t cache,
        const int spectrum_slice_idx,
        const long spectrum_id,
        const double min_mz,
        const double max_mz
);

/**
 * @brief read_spectrum_slice_data
 * @param bb_bytes
 * @param peaks_start_pos
 * @param peaks_count
 * @param de
 * @param min_mz
 * @param max_mz
 * @return
 */
libmzdb_spectrum_data_t* libmzdb_read_spectrum_slice_data(
        const byte* bb_bytes,
        const int peaks_start_pos,
        const int peaks_count,
        const libmzdb_data_encoding_t de,
        const double min_mz,
        const double max_mz
);

/**
 * @brief merge_spectrum_slices
 * @param sd_slices
 * @param bb_count
 * @param peak_count
 * @return
 */
libmzdb_spectrum_data_t libmzdb_merge_spectrum_slices(libmzdb_spectrum_data_t* sd_slices, int bb_count, int peak_count);

/**
 * @brief build_bbox
 * @param stmt
 * @return
 */
libmzdb_bounding_box_t libmzdb_build_bbox(sqlite3_stmt* stmt, int do_malloc);

/**
 * @brief index_bbox
 * @param bbox
 * @param de_cache
 * @return
 */
libmzdb_indexed_bounding_box_t libmzdb_index_bbox(libmzdb_bounding_box_t bbox, libmzdb_data_encodings_cache_t de_cache);

/**
 * @brief free_spectrum_data_content
 * @param spectrum_data
 */
void libmzdb_free_spectrum_data_content(libmzdb_spectrum_data_t libmzdb_spectrum_data);

/**
 * @brief free_spectrum_header_content
 * @param spectrum_header
 */
void libmzdb_free_spectrum_header_content(libmzdb_spectrum_header_t libmzdb_spectrum_header);

/**
 * @brief free_spectrum
 * @param spectrum
 */
void libmzdb_free_spectrum(libmzdb_spectrum_t* libmzdb_spectrum);

int libmzdb_get_spectrum(sqlite3* db, long spectrum_id, libmzdb_entity_cache_t entity_cache, libmzdb_spectrum_t** result);




#endif //QUERIES_H





















