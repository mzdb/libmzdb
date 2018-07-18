#ifndef MZDB_H
#define MZDB_H
#include "models.h"

//extern static int MZDB_VERBOSE_MODE;


/**
 * @brief open_mzdb_file open a mzdb file
 * @param filename the filename
 * @param pp_db the DB
 * @return SQLITE_OK if no arror occured
 */
int libmzdb_open_mzdb_file(const char* filename, sqlite3** pp_db);


/**
 * @brief close_mzbd_file close a mzbd file
 * @param pp_db the pointer of the DB
 * @return SQLITE_OK if no arror occured
 */
int libmzdb_close_mzdb_file(sqlite3* pp_db);



/**
 * @brief create_spectrum_header get a headers from the DB
 * @param stmt result of the query
 * @param spectrum_header the header to get
 * @param err_msg the error message if an error occured
 * @return SQLITE_OK if no error occured
 */
int libmzdb_create_spectrum_header(sqlite3_stmt* stmt, libmzdb_spectrum_header_t* libmzdb_spectrum_header, char** err_msg);


/**
 * @brief get_spectrum_headers get each spectrum from a DB
 * @param db the DB where the headers are stocked
 * @param spectrum_headers an array of each spectrum headers
 * @param err_msg the error message if an error occured
 * @return SQLITE_OK if no error occured
 */
int libmzdb_get_spectrum_headers(sqlite3* db, libmzdb_spectrum_header_t** spectrum_headers, int *spectrum_header_count, char** err_msg);

/**
 * @brief create_entity_cache create the entity cache, use to stock data pre computed
 * @param db the data base
 * @param libmzdb_entity_cache the result
 * @param err_msg the error message
 * @return SQLITE_OK if no error occured
 */
int libmzdb_create_entity_cache(sqlite3 *db, libmzdb_entity_cache_t** libmzdb_entity_cache, char **err_msg);




#endif










///**
// * @brief free_user_text free a user text
// * @param user_text the user text to free
// */
//void libmzdb_free_user_text(libmzdb_user_text_t* libmzdb_user_text);


///**
// * @brief free_user_param free a user param
// * @param user_param the user param to free
// */
//void libmzdb_free_user_param(libmzdb_user_param_t* libmzdb_user_param);

///**
// * @brief free_cv_param free a cv_param
// * @param cv_param the cv param to free
// */
//void libmzdb_free_cv_param(libmzdb_cv_param_t* libmzdb_cv_param);

///**
// * @brief free_param_tree free a param tree
// * @param param_tree the param tree to free
// */
//void libmzdb_free_param_tree(libmzdb_param_tree_t* libmzdb_param_tree);


//int openMzDbFile(
//  const char *filename,   //Database filename (UTF-8)
//  sqlite3 **ppDb          //OUT: SQLite db handle
//);

//int closeMzDbFile(sqlite3* ppDb);

//void freeSpectrumHeader(SpectrumHeader *spectrumHeaderPtr);

//int getSpectrumHeader(sqlite3_stmt *stmt,
//                      SpectrumHeader *spectrumHeaderPtr,
//                      char **errMsg
//);

//int getSpectrumHeaders(
//        sqlite3 *db,
//        SpectrumHeader **spectrumHeaders,
//        char **errMsg
//);











