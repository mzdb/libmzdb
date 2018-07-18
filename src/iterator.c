#include "iterator.h"
#include "queries.h"
#include "sqlite.h"

#include <stdio.h>

#define WAIT_HAS_NEXT   0
#define WAIT_NEXT       1

#define BUFFER_MAX_SIZE 1024 //

//#include <windows.h>
//#include <psapi.h>

//// TODO: move to util.c
//void libmzdb_memdisp(char* msg)
//{
//    PROCESS_MEMORY_COUNTERS_EX pmc;
//    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
//    SIZE_T mem = pmc.PrivateUsage;
//    printf("%s => mem: %llu\n", msg, mem/1024);
//    //Sleep(100);
//}


void meminit_null_callback(sqlite3_stmt* stmt, void* arg, void** res){} //this callback will be use in the meminit, has no effect

//this function is use to init the memory
void libmzdb_meminit(sqlite3* db) //FIXME: SegFault // rename: init_io_cache
{
    // TODO: only read bytes from disk
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT * FROM bounding_box", -1, &stmt, NULL);
    libmzdb_stmt_iterator_for_each(stmt, &meminit_null_callback, NULL, NULL);
    sqlite3_finalize(stmt);
}

/*==============================STMT ITERATOR=======================================*/

libmzdb_stmt_iterator_t* libmzdb_stmt_iterator_create(sqlite3_stmt *stmt)
{
    libmzdb_stmt_iterator_t* stmt_it = (libmzdb_stmt_iterator_t*) malloc(sizeof(libmzdb_stmt_iterator_t));
    stmt_it->stmt = stmt;
    stmt_it->is_iterator_checked = WAIT_HAS_NEXT; //wait only a has next, next will be ignore
    return stmt_it;
}

int libmzdb_stmt_iterator_dispose(libmzdb_stmt_iterator_t *it)
{
    free(it);
    return IT_DONE;
}

sqlite3_stmt* libmzdb_stmt_iterator_next(libmzdb_stmt_iterator_t *it)
{
    if (it->is_iterator_checked == WAIT_HAS_NEXT)
        if (libmzdb_stmt_iterator_has_next(it) == IT_ERROR) return NULL; //if the checker wait a "has_next", call the has_next

    it->is_iterator_checked = WAIT_HAS_NEXT; // reset the checker
    return it->stmt;
}

int libmzdb_stmt_iterator_has_next(libmzdb_stmt_iterator_t *it)
{
    if (it->is_iterator_checked == WAIT_NEXT) return it->last_rc; //use as a barrier, lock the "has_next" while "next" has not been called
    it->is_iterator_checked = WAIT_NEXT; //lock the has_next and wait for a next

    int rc = it->last_rc = sqlite3_step(it->stmt);
    switch (rc)
    {
    case SQLITE_ROW:    return IT_STEP;
    case SQLITE_DONE:   return IT_DONE;
    default:            return IT_ERROR;
    }
}

int libmzdb_stmt_iterator_for_each(sqlite3_stmt *stmt, void (*callback) (sqlite3_stmt*, void*, void**), void *arg, void **result)
{
    libmzdb_stmt_iterator_t* it = libmzdb_stmt_iterator_create(stmt);
    int rc = IT_DONE;
    while((rc = libmzdb_stmt_iterator_has_next(it)) == IT_STEP)
    {
        if(!(libmzdb_stmt_iterator_next(it))) return IT_ERROR;
        callback(it->stmt, arg, result);
    }
    libmzdb_stmt_iterator_dispose(it);
    return rc;
}


/*==============================BBOX ITERATOR====================================*/

libmzdb_bbox_iterator_t* libmzdb_bbox_iterator_create(sqlite3_stmt* stmt, libmzdb_entity_cache_t entity_cache)
{
    libmzdb_bbox_iterator_t* bb_it = (libmzdb_bbox_iterator_t*) malloc(sizeof(libmzdb_bbox_iterator_t));
    bb_it->stmt_it = libmzdb_stmt_iterator_create(stmt);
    bb_it->entity_cache = entity_cache;
    return bb_it;
}

void libmzdb_bbox_iterator_dispose(libmzdb_bbox_iterator_t* bb_it)
{
    libmzdb_stmt_iterator_dispose(bb_it->stmt_it);
    free(bb_it);
}

int libmzdb_bbox_iterator_next(libmzdb_bbox_iterator_t* bb_it, libmzdb_bounding_box_t* bbox)
{
    sqlite3_stmt* stmt = libmzdb_stmt_iterator_next(bb_it->stmt_it);
    if(!stmt) return IT_ERROR;

    //build the next bbox
    *bbox = libmzdb_build_bbox(stmt, 1);

    return IT_OK;
}

int libmzdb_bbox_iterator_has_next(libmzdb_bbox_iterator_t* bbox_it)
{
    libmzdb_stmt_iterator_t* stmt_it = bbox_it->stmt_it;
    return libmzdb_stmt_iterator_has_next(stmt_it);
}


int libmzdb_bbox_iterator_for_each(sqlite3_stmt* stmt, libmzdb_entity_cache_t entity_cache, void (*callback) (libmzdb_bounding_box_t, void*, void**), void* arg, void **result)
{
    libmzdb_bbox_iterator_t* it = libmzdb_bbox_iterator_create(stmt, entity_cache);

    int rc = IT_DONE;
    libmzdb_bounding_box_t current;
    //current.blob = NULL;
    while((rc = libmzdb_bbox_iterator_has_next(it)) == IT_STEP)
    {
        if(libmzdb_bbox_iterator_next(it, &current) != IT_OK) return IT_ERROR;
        callback(current, arg, result);
        //current.blob is free with the sqlite3_step, sqlite3_finalize ...
    }
    libmzdb_bbox_iterator_dispose(it);
    return rc;
}

/*==============================SPECTRUM ITERATOR====================================*/

int _spectrum_iterator_load_bb_row(libmzdb_spectrum_iterator_t* it, libmzdb_indexed_bounding_box_t** indexed_bb_row)
{
    //printf("bb id=%d\n", it->first_bb->id);

    libmzdb_entity_cache_t entity_cache = it->bbox_it->entity_cache;
    libmzdb_indexed_bounding_box_t indexed_first_bb = libmzdb_index_bbox(*it->first_bb, entity_cache.data_encodings_cache);

    // Allocate memory for BBs
    libmzdb_indexed_bounding_box_t* indexed_bbs = (libmzdb_indexed_bounding_box_t*) malloc(sizeof(libmzdb_indexed_bounding_box_t) * it->run_slices_count);
    indexed_bbs[0] = indexed_first_bb;
        //dump_blob_content(indexed_bbs[0].bb.blob, indexed_bbs[0].bb.blob_size);
    // Load all BBs having the same first spectrum id (<=> load a row of BBs)
    libmzdb_bounding_box_t current_bbox;
    int current_first_spectrum_id = indexed_first_bb.spectra_ids[0];
    int bb_count = 1;

    //printf("before while\n");
    //dump_blob_content(indexed_bbs[0].bb.blob, 64);

    while ((it->has_next_bb = libmzdb_bbox_iterator_has_next(it->bbox_it)) == IT_STEP)
    {
        //printf("inside while\n");
        //dump_blob_content(indexed_bbs[0].bb.blob, 64);

        libmzdb_bbox_iterator_next(it->bbox_it, &current_bbox); //TODO: check bbox validity
        libmzdb_indexed_bounding_box_t cur_indexed_bb = libmzdb_index_bbox(current_bbox, entity_cache.data_encodings_cache);

        //dump_blob_content(cur_indexed_bb.bb.blob, cur_indexed_bb.bb.blob_size);

        if (cur_indexed_bb.spectra_ids[0] != current_first_spectrum_id)
        {
            free(it->first_bb); //free the previous first bb

            //create and init the next first bb
            it->first_bb = (libmzdb_bounding_box_t*) malloc(sizeof(libmzdb_bounding_box_t));
            *(it->first_bb) = current_bbox;

            current_first_spectrum_id = cur_indexed_bb.spectra_ids[0];
            break;
        }
        else indexed_bbs[bb_count++] = cur_indexed_bb;
    }
    //printf("loaded bb count=%d\n", bb_count);

    //dump_blob_content(indexed_bbs[0].bb.blob, indexed_bbs[0].bb.blob_size);

    *indexed_bb_row = indexed_bbs;

    return bb_count;
}

int _spectrum_iterator_bb_row_to_spectrum_buffer(libmzdb_spectrum_iterator_t* it, libmzdb_indexed_bounding_box_t* indexed_bb_row, int bb_count, libmzdb_spectrum_t* spectrum_buffer, int sb_offset)
{
    libmzdb_entity_cache_t entity_cache = it->bbox_it->entity_cache;

    // Compute the peak_count sum for each spectrum
    int spectra_count = indexed_bb_row[0].spectrum_slices_count;
    int* spectrum_peak_counts = (int *) calloc(spectra_count, sizeof(int));

    //for each spectrum store in each bbox, sum the result of their peak count
    for (int bb_idx=0; bb_idx<bb_count; ++bb_idx)
        for (int spectra_idx=0; spectra_idx<spectra_count; ++spectra_idx)
            spectrum_peak_counts[spectra_idx] += indexed_bb_row[bb_idx].peaks_counts[spectra_idx];

    //create a new 2D array of (spectrum, bbox) => for each spectrum idx, there is an array of spectrum slice
    //spectrum_data_t** bbs_sd_slices = (spectrum_data_t**) malloc(sizeof(spectrum_data_t*) * spectra_count);
    //for(int i=0; i<spectra_count; ++i) bbs_sd_slices[i] = (spectrum_data_t*) malloc(sizeof(spectrum_data_t) * bb_count);

    libmzdb_data_encodings_cache_t de_cache = entity_cache.data_encodings_cache;

    int spectrum_buffer_index;
    //initialise the array of spectrum and build the spectrum by merging each spectrum slice
    for (int spectrum_idx=0; spectrum_idx<spectra_count; ++spectrum_idx)
    {
        libmzdb_spectrum_data_t* spectrum_slices = (libmzdb_spectrum_data_t*) malloc(sizeof(libmzdb_spectrum_data_t) * bb_count);

        //for each bounding box, get the spectrum slice of the current spectrum index
        for (int bb_idx=0; bb_idx<bb_count; ++bb_idx)
        {
            libmzdb_indexed_bounding_box_t indexed_bb = indexed_bb_row[bb_idx];
            int spectrum_id = indexed_bb.spectra_ids[spectrum_idx];
            //printf("spectrum_idx=%d\n",spectrum_idx);
            //printf("spectrum_id=%d\n",spectrum_id);

            //const libmzdb_data_encoding_t de = *libmzdb_get_data_encoding_from_cache(&de_cache, de_cache.spectrum_id_to_data_encoding_id[spectrum_id]);
            //printf("de=%d\n", de.peak_encoding);

            // TODO: use the stack luke

            libmzdb_spectrum_data_t* sd_ptr = libmzdb_read_spectrum_slice_data_at(indexed_bb, de_cache, spectrum_idx, spectrum_id, -1, -1);
            //bbs_sd_slices[spectrum_idx][bb_idx] = *sd_ptr;
            spectrum_slices[bb_idx] = *sd_ptr;

            //printf("first m/z=%f\n",sd_ptr->mz_array_as_doubles[0]);

            free(sd_ptr); //free the pointer but not the content set in the array
        }

        //printf("before merge\n");

        spectrum_buffer_index = spectrum_idx + sb_offset; //move to the next spectrum index
        spectrum_buffer[spectrum_buffer_index].data = libmzdb_merge_spectrum_slices(spectrum_slices, bb_count, spectrum_peak_counts[spectrum_idx]);
        spectrum_buffer[spectrum_buffer_index].header = entity_cache.spectrum_headers[indexed_bb_row->spectra_ids[spectrum_idx] - 1];

        //printf("after merge\n");

        //free all the data in the array of spectrum slices, each slice has been copied and merged
        for(int j=0; j<bb_count; ++j) libmzdb_free_spectrum_data_content(spectrum_slices[j]);
        free(spectrum_slices);
    }

    //printf("end of _spectrum_iterator_bb_row_to_spectrum_buffer\n");

//    //build each spectrum by merging each spectrum slice
//    int spectrum_buffer_index;
//    for (int spectrum_idx=0; spectrum_idx<spectra_count; ++spectrum_idx)
//    {
//        spectrum_buffer_index = spectrum_idx + sb_offset; //move to the next spectrum index
//        spectrum_buffer[spectrum_buffer_index].data = merge_spectrum_slices(bbs_sd_slices[spectrum_idx], bb_count, spectrum_peak_counts[spectrum_idx]);
//        spectrum_buffer[spectrum_buffer_index].header = entity_cache.spectrum_headers[indexed_bb_row->spectra_ids[spectrum_idx] - 1];
//    }

    //free all the data in the 2D array, each spectrum has been copied and merged
    /*for(int i=0; i<spectra_count; ++i)
    {
        for(int j=0; j<bb_count; ++j) free_spectrum_data_content(bbs_sd_slices[i][j]);
        free(bbs_sd_slices[i]);
    }
    free(bbs_sd_slices);*/

    return spectra_count;
}

int _spectrum_id_comparator(const void * a, const void * b) {
   return ( ((libmzdb_spectrum_t*)a)->header.id - ((libmzdb_spectrum_t*)b)->header.id );
}


void _spectrum_iterator_fill_spectrum_buffer(libmzdb_spectrum_iterator_t* it)
{
    //Sleep(1000);
    libmzdb_entity_cache_t entity_cache = it->bbox_it->entity_cache;
    libmzdb_spectrum_t* spectrum_buffer = (libmzdb_spectrum_t*) malloc(sizeof(libmzdb_spectrum_t) * BUFFER_MAX_SIZE); // aribitray MAX size (should be < ~50)
    libmzdb_indexed_bounding_box_t* indexed_bbs;
    int spectrum_count = 0;
    int bb_count;
    int next_ms_level;
    int ms1_row_count = 0;

    //int spectrum_count = entity_cache.spectrum_header_count;

    //the loop will stop if the next ms level is a ms level 1 and if a ms level 1 has already been process
    //=> will collect one ms level 1 and each ms level > 1 (before or after the ms level 1)
    int previous_first_spectrum_id = -1;
    while (ms1_row_count <= 1 && previous_first_spectrum_id != it->first_bb->first_spectrum_id) {

        previous_first_spectrum_id = it->first_bb->first_spectrum_id;

        //this will load a bb row and convert each of them into spectrum data
        bb_count = _spectrum_iterator_load_bb_row(it, &indexed_bbs); //load each bb in the row, will update the it->first_bb
        spectrum_count += _spectrum_iterator_bb_row_to_spectrum_buffer(it, indexed_bbs, bb_count, spectrum_buffer, spectrum_count);

        //printf("before free each indexed bbox ms1_row_count=%d\n", ms1_row_count);

        //free each indexed bbox
        for(int i=0; i<bb_count; ++i)
        {
            free(indexed_bbs[i].peaks_counts);
            free(indexed_bbs[i].slices_indexes);
            free(indexed_bbs[i].spectra_ids);
            free(indexed_bbs[i].bb.blob);
        }
        free(indexed_bbs);

        next_ms_level = entity_cache.spectrum_headers[it->first_bb->first_spectrum_id - 1].ms_level;

        if (next_ms_level == 1 || it->is_single_ms_level_mode ) ms1_row_count += 1;

        //printf("after free each indexed bbox ms1_row_count=%d\n", ms1_row_count);


    }

    //printf("beofre qsort\n");

    //sort the array with the spectrum id
    qsort(spectrum_buffer, spectrum_count, sizeof(libmzdb_spectrum_t), _spectrum_id_comparator);

    //for each spectrum in the previous buffer, free it
    for(int i=0; i<it->spectrum_count; ++i) libmzdb_free_spectrum_data_content(it->spectrum_buffer[i].data);
    free(it->spectrum_buffer); //free the buffer

    it->spectrum_buffer = spectrum_buffer; //replace the freed buffer
    it->spectrum_count = spectrum_count;
    it->spectrum_idx = 0; //reset the current spectrum index use by the next function

    //printf("end of _spectrum_iterator_fill_spectrum_buffer\n");
}

#define SQLQUERY_ALLMSLEVELS "SELECT bounding_box.* FROM bounding_box, spectrum WHERE spectrum.id = bounding_box.first_spectrum_id"
#define SQLQUERY_SINGLEMSLEVEL "SELECT bounding_box.* FROM bounding_box, spectrum WHERE spectrum.id = bounding_box.first_spectrum_id AND spectrum.ms_level= %d"

libmzdb_spectrum_iterator_t* libmzdb_spectrum_iterator_create(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache)
{
    sqlite3_stmt* stmt;

    //initialise the query (all ms level or only one), and preprare it for the iterator
    char* sql_str = (ms_level <= 0) ? SQLQUERY_ALLMSLEVELS: sqlite3_mprintf(SQLQUERY_SINGLEMSLEVEL, ms_level);

    int rc = sqlite3_prepare_v2(db, sql_str, -1, &stmt, NULL);

    if(ms_level > 0) sqlite3_free((void *)sql_str);

    if (rc != SQLITE_OK)
    {
        sqlite3_finalize(stmt);
        return NULL;
    }

    libmzdb_spectrum_iterator_t* it = (libmzdb_spectrum_iterator_t*) malloc(sizeof(libmzdb_spectrum_iterator_t));
    it->bbox_it = libmzdb_bbox_iterator_create(stmt, entity_cache);
    it->is_single_ms_level_mode = (ms_level > 0) ? 1 : 0;


    if (libmzdb_bbox_iterator_has_next(it->bbox_it) != IT_STEP) //if there isn't bounding to process, return NULL
    {
        libmzdb_spectrum_iterator_dispose(it);
        return NULL;
    }

    //init the first bb
    it->first_bb = (libmzdb_bounding_box_t*) malloc(sizeof(libmzdb_bounding_box_t));
    if (libmzdb_bbox_iterator_next(it->bbox_it, it->first_bb) != IT_OK) //if next failed, retrun NULL
    {
        libmzdb_spectrum_iterator_dispose(it);
        return NULL;
    }

    //create and fill the spectrum buffer
    it->spectrum_buffer = NULL;
    it->run_slices_count = libmzdb_get_run_slices_count_from_sequence_or_die(db);

    _spectrum_iterator_fill_spectrum_buffer(it);

    return it;
}

void libmzdb_spectrum_iterator_dispose(libmzdb_spectrum_iterator_t* spectrum_it)
{
    //printf("before spectrum_iterator_dispose\n");

    //free the first bb and its content
    free(spectrum_it->first_bb);

    //free the spectrum buffer and its content
    for(int i=0; i<spectrum_it->spectrum_count; ++i)
        libmzdb_free_spectrum_data_content(spectrum_it->spectrum_buffer[i].data); //free only the data, headers depend on the entity cache
    free(spectrum_it->spectrum_buffer);

    //free the bbox iterator
    libmzdb_bbox_iterator_dispose(spectrum_it->bbox_it);

    free(spectrum_it);
}

int libmzdb_spectrum_iterator_next(libmzdb_spectrum_iterator_t* it, libmzdb_spectrum_t* result)
{

    int spectrum_idx_cpy = it->spectrum_idx;
    (it->spectrum_idx)++;

    libmzdb_spectrum_t out = it->spectrum_buffer[spectrum_idx_cpy]; //get the spectrum at the saved idx

    if (it->spectrum_idx >= it->spectrum_count) //if no more spectrum, fill the next bounding box
    {
        if (it->has_next_bb) _spectrum_iterator_fill_spectrum_buffer(it);

        else //if there isn't next bouding box, free the first bb
        {
            free(it->first_bb); //TOCHECK : sqlblob are free by sqlite3 ?
            it->first_bb = NULL; //has next will return false if first bb is NULL
        }
    }
    *result = out;

    return IT_OK;
}

int libmzdb_spectrum_iterator_has_next(libmzdb_spectrum_iterator_t* it)
{
    return (it->first_bb) ? (it->spectrum_idx < it->spectrum_count && it->has_next_bb) ? 1 : 0 : 0;
}

int libmzdb_spectrum_iterator_for_each(sqlite3* db, int ms_level, libmzdb_entity_cache_t entity_cache, void (*callback) (libmzdb_spectrum_t, void*, void**), void* arg, void **result)
{
    libmzdb_spectrum_iterator_t* it = libmzdb_spectrum_iterator_create(db, ms_level, entity_cache);
    int rc = IT_DONE;
    libmzdb_spectrum_t current;

//    spectrum_iterator_has_next(it);
//    if(spectrum_iterator_next(it, &current) != IT_OK) return IT_ERROR;
//    callback(current, arg, result);

    while((rc = libmzdb_spectrum_iterator_has_next(it)) == IT_STEP)
    {
        if(libmzdb_spectrum_iterator_next(it, &current) != IT_OK) return IT_ERROR;
        callback(current, arg, result);
    }
    libmzdb_spectrum_iterator_dispose(it);

    return rc;
}














































//void _spectrum_iterator_init_spectrum_slices_buffer(spectrum_iterator_t* it)
//{
//    entity_cache_t entity_cache = it->bbox_it->entity_cache;
//    indexed_bounding_box_t indexed_first_bb = index_bbox(*it->first_bb, entity_cache.data_encodings_cache);

//    if (it->first_spectrum_slices != NULL)
//    {
//        it->spectrum_slice_buffer = it->first_spectrum_slices;
//        it->spectrum_slice_buffer_count = it->first_spectrum_slices_count;
//    }
//    else
//    {
//        it->spectrum_slice_buffer = bbox_to_slices(indexed_first_bb, entity_cache);
//        it->spectrum_slice_buffer_count = indexed_first_bb.spectrum_slices_count;
//    }

//    it->spectrum_slice_idx = 0;

//    bounding_box_t current_bbox;
//    indexed_bounding_box_t indexed_current_bb;
//    spectrum_t* s_slices;
//    int s_slices_count;

//    while((it->bb_has_next = bbox_iterator_has_next(it->bbox_it)) == IT_STEP)
//    {
//        bbox_iterator_next(it->bbox_it, &current_bbox); //to check
//        indexed_current_bb = index_bbox(current_bbox, entity_cache.data_encodings_cache);

//        s_slices = bbox_to_slices(indexed_current_bb, entity_cache);
//        s_slices_count = indexed_current_bb.spectrum_slices_count;

//        if (s_slices[0].header.id == it->spectrum_slice_buffer[0].header.id)
//        {
//            for(int i=0; i<s_slices_count; ++i)
//            {
//                it->spectrum_slice_buffer[i].header = s_slices[i].header; //set the header

//                //printf("header: %d \n", s_slices[i].header.id);

//                spectrum_data_t* merged_sd = (spectrum_data_t*) malloc(sizeof(spectrum_data_t));

//                spectrum_data_t slice_data_at_i = s_slices[i].data;
//                spectrum_data_t buffered_slice_data_at_i = it->spectrum_slice_buffer[i].data;

//                merged_sd->data_encoding = slice_data_at_i.data_encoding;
//                merged_sd->peak_count = slice_data_at_i.peak_count + buffered_slice_data_at_i.peak_count;

//                malloc_spectrum_data_peaks(merged_sd);

//                // TODO: factorize me with code of get_spectrum
//                switch (merged_sd->data_encoding->peak_encoding)
//                {
//                case HIGH_RES_PEAK:



//memcpy(merged_sd->mz_array_as_doubles, buffered_slice_data_at_i.mz_array_as_doubles, buffered_slice_data_at_i.peak_count * sizeof(double));
//                    memcpy(&merged_sd->mz_array_as_doubles[buffered_slice_data_at_i.peak_count], slice_data_at_i.mz_array_as_doubles, slice_data_at_i.peak_count * sizeof(double));

//                    memcpy(merged_sd->intensity_array_as_floats, buffered_slice_data_at_i.intensity_array_as_floats, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->intensity_array_as_floats[buffered_slice_data_at_i.peak_count], slice_data_at_i.intensity_array_as_floats, slice_data_at_i.peak_count * sizeof(float));

//                    break;

//                case LOW_RES_PEAK:

//                    memcpy(merged_sd->mz_array_as_floats, buffered_slice_data_at_i.mz_array_as_floats, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->mz_array_as_floats[buffered_slice_data_at_i.peak_count], slice_data_at_i.mz_array_as_floats, slice_data_at_i.peak_count * sizeof(float));

//                    memcpy(merged_sd->intensity_array_as_floats, buffered_slice_data_at_i.intensity_array_as_floats, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->intensity_array_as_floats[buffered_slice_data_at_i.peak_count], slice_data_at_i.intensity_array_as_floats, slice_data_at_i.peak_count * sizeof(float));

//                    break;

//                case NO_LOSS_PEAK:

//                    memcpy(merged_sd->mz_array_as_doubles, buffered_slice_data_at_i.mz_array_as_doubles, buffered_slice_data_at_i.peak_count * sizeof(double));
//                    memcpy(&merged_sd->mz_array_as_doubles[buffered_slice_data_at_i.peak_count], slice_data_at_i.mz_array_as_doubles, slice_data_at_i.peak_count * sizeof(double));

//                    memcpy(merged_sd->intensity_array_as_doubles, buffered_slice_data_at_i.intensity_array_as_doubles, buffered_slice_data_at_i.peak_count * sizeof(double));
//                    memcpy(&merged_sd->intensity_array_as_doubles[buffered_slice_data_at_i.peak_count], slice_data_at_i.intensity_array_as_doubles, slice_data_at_i.peak_count * sizeof(double));

//                    break;
//                }

//                if (merged_sd->data_encoding->mode == FITTED)
//                {
//                    memcpy(merged_sd->lwhm_array, buffered_slice_data_at_i.lwhm_array, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->lwhm_array[buffered_slice_data_at_i.peak_count], slice_data_at_i.lwhm_array, slice_data_at_i.peak_count * sizeof(float));

//                    memcpy(merged_sd->rwhm_array, buffered_slice_data_at_i.rwhm_array, buffered_slice_data_at_i.peak_count * sizeof(float));
//                    memcpy(&merged_sd->rwhm_array[buffered_slice_data_at_i.peak_count], slice_data_at_i.rwhm_array, slice_data_at_i.peak_count * sizeof(float));
//                }

//                it->spectrum_slice_buffer[i].data = *merged_sd;
//            }
//        }
//        else
//        {
//            *(it->first_bb) = current_bbox;


//            //free(it->first_spectrum_slices);
//            it->first_spectrum_slices = s_slices;
//            //indexed need to be free ?
//            break;
//        }
//    }

//}

