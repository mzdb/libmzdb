#ifndef ITERATOR_H
#define ITERATOR_H

#include "lib/sqlite/sqlite3.h"
#include "models.h"

#define IT_ERROR -1
#define IT_OK 0
#define IT_STEP 1
#define IT_DONE 2

#define ALL_MS_LEVELS -1


/**
 * @brief meminit Use to init the memory (FIXME SegFault), will increase the time of process for the next queries
 * @param db The database on which the SQL executes
 */
void libmzdb_meminit(sqlite3* db);



/*==============================STMT ITERATOR=======================================*/

typedef struct libmzdb_stmt_iterator
{
    sqlite3_stmt* stmt;
    int last_rc;
    int is_iterator_checked; //check if the function step has been called
}libmzdb_stmt_iterator_t;

/**
 * @brief stmt_iterator_create Create the statement iterator
 * @param stmt The queries
 * @return The iterator
 */
libmzdb_stmt_iterator_t *libmzdb_stmt_iterator_create(sqlite3_stmt* stmt);

/**
 * @brief stmt_iterator_dispose Destroy the stmt iterator
 * @param it The iterator to destroy
 * @return IT_DONE if no error occured
 */
int libmzdb_stmt_iterator_dispose(libmzdb_stmt_iterator_t* it);

/**
 * @brief stmt_iterator_next Get the next object (column) of the queries
 * @param it The iterator
 * @return NULL if an error occured else the stmt to process
 */
sqlite3_stmt* libmzdb_stmt_iterator_next(libmzdb_stmt_iterator_t* it);

/**
 * @brief stmt_iterator_has_next Check if there is still column to process
 * @param it The iterator
 * @return IT_ERROR if an error occured, IT_STEP if there is still column to process else IT_DONE
 */
int libmzdb_stmt_iterator_has_next(libmzdb_stmt_iterator_t* it);

/**
 * @brief stmt_iterator_for_each For each column of the queries, call a callback with the stmt as argument
 * @param stmt The queries to execute
 * @param arg Possible argument for the callback
 * @param result Possible location for the result of the callback
 * @return IT_ERROR if an error occured, else IT_DONE
 */
int libmzdb_stmt_iterator_for_each(sqlite3_stmt *stmt, void (*callback) (sqlite3_stmt*, void*, void**), void *arg, void **result);


/*==============================BBOX ITERATOR====================================*/


typedef struct libmzdb_bbox_iterator
{
    libmzdb_stmt_iterator_t* stmt_it;
    libmzdb_entity_cache_t entity_cache;
} libmzdb_bbox_iterator_t;

/**
 * @brief bbox_iterator_create Create the bounding box iterator
 * @param stmt The Queries to execute
 * @param de_cache The data encoding cache
 * @return The bounding box iterator
 */
libmzdb_bbox_iterator_t* libmzdb_bbox_iterator_create(sqlite3_stmt* stmt, libmzdb_entity_cache_t entity_cache);

/**
 * @brief bbox_iterator_dispose Destroy a bounding box iterator
 * @param bb_it The bounding box iterator
 */
void libmzdb_bbox_iterator_dispose(libmzdb_bbox_iterator_t* bb_it);

/**
 * @brief bbox_iterator_next Get the next BBox
 * @param bb_it The bounding box iterator
 * @param bbox The pointer of the next Bbox
 * @return It_ERROR if an error occured else IT_OK
 */
int libmzdb_bbox_iterator_next(libmzdb_bbox_iterator_t* bb_it, libmzdb_bounding_box_t* bbox);

/**
 * @brief bbox_iterator_has_next Check if the iterator has at least one next bbox waiting to process
 * @param bbox_it The bounding box iterator
 * @return IT_ERROR if an error occured, IT_STEP if there is still column to process else IT_DONE
 */
int libmzdb_bbox_iterator_has_next(libmzdb_bbox_iterator_t* bbox_it);

/**
 * @brief bbox_iterator_for_each For each bouding box call a callback
 * @param stmt The query to execute
 * @param entity_cache The entity cache
 * @param arg The possible argument for the callback
 * @param result The possible result for the callback
 * @return IT_ERROR if an error occured, else IT_DONE
 */
int libmzdb_bbox_iterator_for_each(sqlite3_stmt* stmt, libmzdb_entity_cache_t libmzdb_entity_cache, void (*callback) (libmzdb_bounding_box_t, void*, void**), void* arg, void **result);


/*==============================SPECTRUM ITERATOR====================================*/


typedef struct libmzdb_spectrum_iterator
{
    libmzdb_bbox_iterator_t* bbox_it;
    libmzdb_bounding_box_t* first_bb;

    int spectrum_idx;
    libmzdb_spectrum_t* spectrum_buffer;
    int spectrum_count;

    int has_next_bb;
    int run_slices_count;

    int is_single_ms_level_mode;

    int is_iterator_checked; //check if the function has_next/next has been called consecutively
    int last_has_next_value;

} libmzdb_spectrum_iterator_t;

/**
 * @brief spectrum_iterator_create Create a spectrum iterator
 * @param db The database on which the SQL executes
 * @param stmt The query to execute
 * @param entity_cache The entity cache
 * @return NULL if an error occured, else the spectrum iterator
 */
libmzdb_spectrum_iterator_t* libmzdb_spectrum_iterator_create(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache);

/**
 * @brief spectrum_iterator_dispose Destroy the spectrum it
 * @param spectrum_it The spectrum iterator
 */
void libmzdb_spectrum_iterator_dispose(libmzdb_spectrum_iterator_t* spectrum_it);

/**
 * @brief spectrum_iterator_next Get the next spectrum
 * @param it The spectrum iterator
 * @param result the next spectrum
 * @return It_OK if no error occured
 */
int libmzdb_spectrum_iterator_next(libmzdb_spectrum_iterator_t* it, libmzdb_spectrum_t* result);

/**
 * @brief spectrum_iterator_has_next Check if there is at least one spectrum waiting to process in the iterator
 * @return IT_STEP if a spectrum waits to be process else IT_DONE
 */
int libmzdb_spectrum_iterator_has_next(libmzdb_spectrum_iterator_t* it);

/**
 * @brief spectrum_iterator_for_each For each spectrum, call a callback
 * @param db The database on which the SQL executes
 * @param stmt The query to execute
 * @param entity_cache The entity cache
 * @param arg Possible argument for the callback
 * @param result Possible result for the callback
 * @return IT_ERROR if an error occured, else IT_DONE
 */
int libmzdb_spectrum_iterator_for_each(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache, void (*callback) (libmzdb_spectrum_t, void*, void**), void* arg, void **result);

#endif //ITERATOR_H
