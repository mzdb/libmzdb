//#define _CRT_SECURE_NO_WARNINGS


#include <float.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <windows.h>

#include "log.h"
#include "models.h"
#include "mzdb.h"
#include "queries.h"
#include "tables.h"
#include "test.h"
#include "util.h"
#include "xml.h"
#include "lib/sqlite/sqlite3.h"

#include "iterator.h"


void print_spectrum(libmzdb_spectrum_t spectrum, void* arg, void* res)
{
    printf("\n#Spectrum : %d --------------------------------------------------------\n\n", spectrum.header.id);

    double d_in, d_mz;
    float f_in, f_mz;

    for(int i=0; i<spectrum.data.peak_count; ++i)
    {
        switch (spectrum.data.data_encoding.peak_encoding)
        {
        case HIGH_RES_PEAK:
            printf("==> HR ");
            f_in = spectrum.data.intensity_array_as_floats[i];
            d_mz = spectrum.data.mz_array_as_doubles[i];

            printf("\tIntensity : ");
            if (f_in <= -0.5 || isinf(f_in) || f_in > 10e10) printf("FATAL ERROR\t\t\t\t\t\t"); else printf("%f\t\t\t\t\t\t", f_in);

            printf("MZ: ");
            if (d_mz <= -0.5 || isinf(d_mz) || d_mz > 10e10) printf("FATAL ERROR\t"); else printf("%f\t\t", d_mz);
            break;

        case LOW_RES_PEAK:
            printf("==> LR ");
            f_in = spectrum.data.intensity_array_as_floats[i];
            f_mz = spectrum.data.mz_array_as_floats[i];

            printf("\tIntensity : ");
            if (f_in <= -0.5 || isinf(f_in)) printf("FATAL ERROR\t\t\t\t\t\t"); else printf("%f\t\t\t\t\t\t", f_in);

            printf("MZ: ");
            if (f_mz <= -0.5 || isinf(f_mz)) printf("FATAL ERROR\t"); else printf("%f\t\t", f_mz);
            break;

        case NO_LOSS_PEAK:
            printf("==> NL ");
            d_in = spectrum.data.intensity_array_as_doubles[i];
            d_mz = spectrum.data.mz_array_as_doubles[i];

            printf("\tIntensity: ");
            if (d_in <= -0.5 || isinf(d_in) || d_in > 10e10) printf("FATAL ERROR\t\t\t\t\t\t"); else printf("%f\t\t\t\t\t\t", d_in);

            printf("MZ: ");
            if (d_mz <= -0.5 || isinf(d_mz) || f_in > 10e10) printf("FATAL ERROR\t"); else printf("%f\t\t", d_mz);
            break;
        };
        printf("\n");
        fflush(stdout);
        //Sleep(10);
    }
}

struct data
{
    sqlite3 *db;
    libmzdb_entity_cache_t* entity_c;
};

void cmp_spectrum(libmzdb_spectrum_t spectrum, void* arg, void* res)
{
    print_spectrum(spectrum, NULL, NULL);

    struct data * d = (struct data* ) arg;

    libmzdb_spectrum_t* s;
    libmzdb_get_spectrum(d->db, spectrum.header.id, *d->entity_c, &s);
    print_spectrum(*s, NULL, NULL);
    libmzdb_free_spectrum(s);

    printf("\n\n\n\n\n\n\n");
}

int main(void)
{
    sqlite3 *db;;
    int rc;

    //char* mzdb_file_path = "D:/Dev/libmzdb/qtcreator_workspace/data/OVEMB150205_12.raw.mzDB";
    char* mzdb_file_path = "D:/LCMS/mzdb_files/TT66_julie_04.wiff.mzDB";

    rc = libmzdb_open_mzdb_file(mzdb_file_path, &db);

    libmzdb_entity_cache_t* entity_cache;
    libmzdb_create_entity_cache(db, &entity_cache, NULL);

    libmzdb_meminit(db);

    //    libmzdb_spectrum_t* s2;
    //    libmzdb_get_spectrum(db, 46, *entity_cache, &s2);
    //    print_spectrum_id(*s2, NULL, NULL);

    struct data d;
    d.db = db;
    d.entity_c = entity_cache;

    double start=clock();
    libmzdb_spectrum_iterator_for_each(db, 1, *entity_cache, &cmp_spectrum, &d, NULL);
    double stop=clock();
    printf("time at the end : %d s\n",(stop-start)/CLOCKS_PER_SEC);

    return 0;
}








//int main(void)
//{

//    sqlite3 *db;;
//    int rc;
//    int failed_count =0;

//    char* debug_str; char* err_msg;
//    double debug_dbl;
//    int debug_int;
//    float debug_flt;
//    long debug_long;



//    int * debug_arr_int;

//    printf("\nTest-- Open-Close rc\n");

//    //char* mzdb_file_path = "D:/Dev/libmzdb/qtcreator_workspace/data/OVEMB150205_12.raw.0.9.8.mzDB"; - BROKEN
//    //char* mzdb_file_path = "D:/Dev/libmzdb/qtcreator_workspace/data/OVEMB150205_12.raw.mzDB";
//    char* mzdb_file_path = "D:/LCMS/mzdb_files/TT66_julie_04.wiff.mzDB";
//    rc = open_mzdb_file(mzdb_file_path, &db);
//    failed_count+= rc_check("Try to open the DB", rc);

//    rc = close_mzdb_file(db);
//    failed_count+= rc_check("Try to close the DB", rc);

//    rc = open_mzdb_file(mzdb_file_path, &db);

//    printf("\nTest-- Getter rc\n");

//    rc = get_model_version(db, &debug_str, &err_msg);
//    failed_count+= rc_check("Try to get the model version", rc);


//    rc= get_param_tree_chromatogram(db, &debug_str, &err_msg);
//    failed_count+= rc_check("Try to get the param tree chromatogram", rc);


//    rc = get_pwiz_mzdb_version(db, &debug_str, &err_msg);
//    failed_count+= rc_check("Try to get the pwiz mzdb version", rc);

//    rc = get_last_time(db, &debug_dbl, &err_msg);
//    failed_count+= rc_check("Try to get the last time", rc);


//    rc = get_max_ms_level(db, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the max ms level", rc);

//    rc = get_cycles_count(db, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the cycle count", rc);

//    rc = get_spectra_count(db, 2, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the spectra count", rc);

//    rc = get_bounding_boxes_count(db, 2, &debug_int, &err_msg);
//    failed_count+= rc_check("Try to get the bbox count", rc);

//    rc = get_bounding_box_first_spectrum_id(db, 500, &debug_long, &err_msg);
//    failed_count+= rc_check("Try to get the first spectrum id", rc);

//    rc = get_bounding_box_min_mz(db, 2, &debug_flt, &err_msg);
//    failed_count+=rc_check("Try to get the bbox min mz", rc);

//    rc = get_bounding_box_min_time(db, 150, &debug_flt, &err_msg);
//    failed_count+= rc_check("Try to get the bbox min time", rc);


//    rc = get_run_slice_id(db, 13, &debug_int, &err_msg);
//    failed_count+= rc_check("Try to get the run slice id", rc);

//    rc = get_ms_level_from_run_slice_id_manually(db, 10, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the ms level from run slice id (manually)", rc);

//    rc = get_table_records_count(db, "run_slice", &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the table records count", rc);

//    rc = get_mz_range(db, 1, &debug_arr_int, &err_msg);
//    failed_count+=rc_check("Try to get the mz range", rc);

//    rc = get_bounding_box_ms_level(db, 1, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the bbox ms level", rc);

//    rc = get_bounding_boxes_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the bbox count from sequence", rc);

//    rc = get_data_encodings_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the data encodings from sequence", rc);

//    rc = get_spectra_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+=rc_check("Try to get the spectra count from sequence", rc);

//    rc = get_run_slices_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Try to get the run slices count from sequence", rc);








//    printf("\nTest-- Getter value\n");

//    rc = get_model_version(db, &debug_str, &err_msg);
//    failed_count+= rc_check("Check the model version", strcmp(debug_str, "0.7"));


//    rc= get_param_tree_chromatogram(db, &debug_str, &err_msg);
//    failed_count+= rc_check("Check the param tree chromatogram [TODO]", -1);


//    rc = get_pwiz_mzdb_version(db, &debug_str, &err_msg);
//    failed_count+= rc_check("Check the pwiz mzdb version", strcmp(debug_str, "0.9.8"));

//    rc = get_last_time(db, &debug_dbl, &err_msg);
//    failed_count+= rc_check("Check the last time", debug_dbl-240.863494873047);


//    rc = get_max_ms_level(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the max ms level", debug_int-2);

//    rc = get_cycles_count(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the cycle count", debug_int-158);

//    rc = get_spectra_count(db, 2, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the spectra count", debug_int-1035);

//    rc = get_bounding_boxes_count(db, 2, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the bbox count", debug_int-15);

//    rc = get_bounding_box_first_spectrum_id(db, 500, &debug_long, &err_msg);
//    failed_count+= rc_check("Check the first spectrum id", debug_long-480);


//    //to check
//    rc = get_bounding_box_min_mz(db, 2, &debug_flt, &err_msg);
//    failed_count+= rc_check("Check the bbox min mz [TODO]", -1);

//    rc = get_bounding_box_min_time(db, 150, &debug_flt, &err_msg);
//    failed_count+= rc_check("Check the bbox min time [TODO]", -1);

//    rc = get_run_slice_id(db, 13, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the run slice id", debug_int-10);

//    rc = get_ms_level_from_run_slice_id_manually(db, 10, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the ms level from run slice id (manually)", debug_int-1);

//    rc = get_table_records_count(db, "run_slice", &debug_int, &err_msg);
//    failed_count+= rc_check("Check the table records count", debug_int-161);

//    rc = get_mz_range(db, 1, &debug_arr_int, &err_msg);
//    failed_count+= rc_check("Check the mz range", debug_arr_int[0]-400+debug_arr_int[1]-1200);

//    rc = get_bounding_box_ms_level(db, 1, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the bbox ms level", debug_int-2);

//    rc = get_bounding_boxes_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the bbox count from sequence", debug_int-3406);

//    rc = get_data_encodings_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the data encodings from sequence", debug_int-2);

//    rc = get_spectra_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the spectra count from sequence", debug_int-1193);

//    rc = get_run_slices_count_from_sequence(db, &debug_int, &err_msg);
//    failed_count+= rc_check("Check the run slices count from sequence", debug_int-161);


//    printf("\nTest-- Spectrum test\n"); //here the result are not sure at 100%

//    debug_int = get_data_encoding_id_or_die(db, 5);
//    failed_count+= rc_check("Check the data encoding id", debug_int-1);


//    //    debug_int = get_data_precision_size_in_bytes(get_data_precision(db, debug_int));
//    //    failed_count+= rc_check("Check the struct size", debug_int-sizeof(data_point64_32_t));

//    printf("\nTest-- Log test\n");

//    set_log_level(LOG_DEBUG);
//    failed_count+= rc_check("Check the set log level", get_log_level()-LOG_DEBUG);

//    set_log_file(stderr);
//    failed_count+= rc_check("Check the set log file", get_log_file()-stderr);

//    failed_count+= rc_check("Check the flog system (manually check)", 0);
//    failed_count+= rc_check("Check the flogf system (manually check)", 0);

//    printf("\n\tFailed : %d\n", failed_count);



//    entity_cache_t* entity_cache;
//    create_entity_cache(db, &entity_cache, NULL);

//    time_t start, stop;
//    spectrum_t* spectrum;
//    //    start=clock();
//    //    for(int x=1; x<1193; ++x)
//    //        if (get_spectrum(db, x, *entity_cache, &spectrum)!= SQLITE_OK)
//    //            printf("\nError get spectrum data in main function\n");
//    //    stop=clock();
//    //    printf("time at the end : %d s\n",(stop-start)/CLOCKS_PER_SEC);

////    if (get_spectrum(db, 28, *entity_cache, &spectrum)!= SQLITE_OK)
////        printf("\nError get spectrum data in main function\n");
////    else
////        printf("\nget spectrum 28 data in main function\n");


////    int * ok;
////    free(ok);

//    sqlite3_stmt* stmt1, *stmt2;

//    rc = sqlite3_prepare_v2(db, "SELECT * FROM bounding_box", -1, &stmt1, NULL);

//    start=clock();
//    stmt_iterator_for_each(stmt1, &print_stmtbbox_id, NULL, NULL);
//    stop=clock();
//    printf("time at the end : %d s\n",(stop-start)/CLOCKS_PER_SEC);

//    sqlite3_finalize(stmt1);


//    rc = sqlite3_prepare_v2(db, "SELECT * FROM bounding_box", -1, &stmt2, NULL);

//    start=clock();
//    bbox_iterator_for_each(stmt2, *entity_cache, &print_bbox_id, NULL, NULL);
//    stop=clock();
//    printf("time at the end : %d s\n",(stop-start)/CLOCKS_PER_SEC);

//    sqlite3_finalize(stmt2);

//    //rc = sqlite3_prepare_v2(db, "SELECT bounding_box.* FROM bounding_box, spectrum WHERE spectrum.id = bounding_box.first_spectrum_id AND spectrum.ms_level = 1", -1, &stmt1, NULL);
//    rc = sqlite3_prepare_v2(db, "SELECT bounding_box.* FROM bounding_box", -1, &stmt1, NULL);

//    start=clock();
//    spectrum_iterator_for_each(db, stmt1, *entity_cache, &print_spectrum_id, NULL, NULL);
//    stop=clock();
//    printf("time at the end : %d s\n",(stop-start)/CLOCKS_PER_SEC);

//     sqlite3_finalize(stmt1);

//    start=clock();
//    spectrum_t* s;
//    for(int i=1; i<=47668; ++i) {
//        printf("i: %d/47668 \n", i);
//        get_spectrum(db, i, entity_cache, &s);
//        free_spectrum(s);
//    }
//    stop=clock();
//    printf("time at the end : %d s\n",(stop-start)/CLOCKS_PER_SEC);

//    //Sleep(20000); // Sleep 20 seconds
//    return 0;
//}
