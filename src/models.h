#ifndef MODELS_H
#define MODELS_H
#include <stdlib.h>
#include "lib/sqlite/sqlite3.h"

#define LIBMZDB_ACQUISITION_MODE_DDA "Data Dependant Acquisition (Thermo designation), Warning: in ABI this is called IDA (Information Dependant Acquisition)"
#define LIBMZDB_ACQUISITION_MODE_SWATH "ABI Swath acquisition or Thermo swath acquisition"
#define LIBMZDB_ACQUISITION_MODE_MRM "Multiple reaction monitoring"
#define LIBMZDB_ACQUISITION_MODE_SRM "SRM (Single reaction monitoring) acquisition"
#define LIBMZDB_ACQUISITION_MODE_UNKNOWN "unknown acquisition mode"

#define LIBMZDB_PSI_MS_32_BIT_FLOAT "*0521"
#define LIBMZDB_PSI_MS_64_BIT_FLOAT "*0523"
#define LIBMZDB_ACQUISITION_PARAMETER "*1954"
#define LIBMZDB_ISOLATION_WINDOW_TARGET_MZ "MS:*0827"
#define LIBMZDB_ISOLATION_WINDOW_LOWER_OFFSET "MS:*0828"
#define LIBMZDB_ISOLATION_WINDOW_UPPER_OFFSET "MS:*0829"
#define SELECTED_ION_MZ "MS:*0744"

//the acquisition mode
typedef enum libmzdb_acquisition_mode { DDA, SWATH, MRM, SRM, UNKNOW} libmzdb_acquisition_mode_enum;

//an array of each acquisition mode decription, match with the enumeration
static char* _libmzdb_acquisition_mode_desc[5] =
{
    LIBMZDB_ACQUISITION_MODE_DDA,
    LIBMZDB_ACQUISITION_MODE_SWATH,
    LIBMZDB_ACQUISITION_MODE_MRM,
    LIBMZDB_ACQUISITION_MODE_SRM,
    LIBMZDB_ACQUISITION_MODE_UNKNOWN
};
#define LIBMZDB_ACQUISITION_MODE_TO_STRING(_index_) _libmzdb_acquisition_mode_desc[_index_];

//custom data type
typedef unsigned char byte;
//typedef float float32_t;
//typedef double float64_t;

typedef enum libmzdb_data_precision //use to know which precision is use
{
    DATA_PRECISION_UNKNOWN        = 0,
    DATA_PRECISION_64_64          = 1,
    DATA_PRECISION_64_32          = 2,
    DATA_PRECISION_32_32          = 3,
    DATA_PRECISION_FITTED_64_32   = 4,
} libmzdb_data_precision_enum;

//an array of each data precision decription, match with the enumeration
static char * _libmzdb_data_precision_str[5] =
{
    "DATA_PRECISION_UNKNOWN",
    "DATA_PRECISION_64_64",
    "DATA_PRECISION_64_32",
    "DATA_PRECISION_32_32",
    "DATA_PRECISION_FITTED_64_32"
};
#define LIBMZDB_DATA_PRECESION_TO_STRING(_index_) _libmzdb_data_precision_str[_index_];

typedef struct libmzdb_data_point_32_32
{
    float x;
    float y;
} libmzdb_data_point_32_32_t;

typedef struct libmzdb_data_point_64_32
{
    double x;
    float y;
} libmzdb_data_point_64_32_t;


typedef struct libmzdb_data_point_64_64
{
    double x;
    double y;
} libmzdb_data_point_64_64_t;

typedef struct libmzdb_fitted_peak
{
    double x;
    float y;
    float left_hwhm;
    float right_hwhm;
} libmzdb_fitted_peak_t;

//param_tree.h
typedef struct libmzdb_cv_param
{
    char* cv_ref;
    char* accession;
    char* name;
    char* value;
    char* unit_cv_ref;
    char* unit_accession;
    char* unit_name;
} libmzdb_cv_param_t;

typedef struct libmzdb_user_param
{
    char* cv_ref;
    char* accession;
    char* name;
    char* value;
    char* type;
}libmzdb_user_param_t;

typedef struct libmzdb_user_text
{
    char* cv_ref;
    char* accession;
    char* name;
    char* text;
    char* type;
} libmzdb_user_text_t;

typedef struct libmzdb_param_tree
{
    libmzdb_cv_param_t* cv_params;
    libmzdb_user_param_t* user_params;
    libmzdb_user_text_t* user_text;
}libmzdb_param_tree_t;

typedef struct libmzdb_mzdb_param_tree
{
    float ms1_bb_mz_width;
    float msn_bb_mz_width;
    float ms1_bb_time_width;
    float msn_bb_time_width;
    int is_loss_less;
    char* origin_file_format;
}libmzdb_mzdb_param_tree_t;

typedef enum libmzdb_data_mode { PROFILE = -1, CENTROID = 12, FITTED = 20 } libmzdb_data_mode_t;
typedef enum libmzdb_peak_encoding { LOW_RES_PEAK = 8, HIGH_RES_PEAK = 12, NO_LOSS_PEAK = 16 } libmzdb_peak_encoding_t;
typedef enum libmzdb_byte_order { BIG_ENDIAN, LITTLE_ENDIAN } libmzdb_byte_order_t;

typedef struct libmzdb_data_encoding
{
    int id;
    libmzdb_data_mode_t mode;
    libmzdb_peak_encoding_t peak_encoding;
    char* compression;
    libmzdb_byte_order_t byte_order;
    //int peak_struct_size;
} libmzdb_data_encoding_t;

typedef struct libmzdb_data_encodings_cache
{
    libmzdb_data_encoding_t* data_encodings; // length = number of data_encodings
    int de_size;
    int* data_encoding_id_mapping; //[ id => idx] ; length of MAX(id) + 1
    int mapping_size;
    int* spectrum_id_to_data_encoding_id; //[ spectrum_id => data_encoding_id] ; length of MAX(spectrum_id) + 1
    int spectrum_count;
} libmzdb_data_encodings_cache_t;

// pragma usage (specific to MS Compiler):
// - http://www.cplusplus.com/forum/general/14659/
// - https://stackoverflow.com/questions/3318410/pragma-pack-effect
// - https://msdn.microsoft.com/en-us/library/2e70t5y1.aspx

// Exact fit - no padding
#pragma pack(push)
#pragma pack(1)

typedef struct libmzdb_spectrum_peaks
{
    libmzdb_data_precision_enum data_precision;
    int peak_count;
    union {
        libmzdb_data_point_64_64_t* peaks64_64;
        libmzdb_data_point_64_32_t* peaks64_32;
        libmzdb_data_point_32_32_t* peaks32_32;
        libmzdb_fitted_peak_t* fitted_peaks;
        //peak_t* peaks; // upt to peak* -- ???
    };
} libmzdb_spectrum_peaks_t;

//back to whatever the previous packing mode was
#pragma pack(pop)

typedef struct libmzdb_data_points_32_32
{
    float* x_list;
    float* y_list;
} libmzdb_data_points_32_32_t;

typedef struct libmzdb_data_points_64_32
{
    double* x_list;
    float*  y_list;
} libmzdb_data_points_64_32_t;

typedef struct libmzdb_data_points_64_64
{
    double* x_list;
    double* y_list;
} libmzdb_data_points_64_64_t;

typedef struct libmzdb_spectrum_data
{
    libmzdb_data_encoding_t data_encoding;
    int peak_count;
    union {
        double* mz_array_as_doubles;
        float* mz_array_as_floats;
    };
    union {
        double* intensity_array_as_doubles;
        float* intensity_array_as_floats;
    };
    float* lwhm_array; // warning: can be NULL
    float* rwhm_array; // warning: can be NULL
} libmzdb_spectrum_data_t;

typedef struct libmzdb_spectrum_header
{
    int id;
    int initial_id;
    char* title;
    int cycle;
    float time;
    int ms_level;
    char* activation_type;
    float tic;
    double base_peak_mz;
    float base_peak_intensity;
    double precursor_mz;
    int precursor_charge;
    int peaks_count;
    char* param_tree_str;
    char* scan_list_str; //xml reference
    char* precursor_list_str; //xml reference
    char* product_list_str; //xml reference
    int shared_param_tree_id;
    int instrument_configuration_id;
    int source_file_id;
    int run_id;
    int data_processing_id;
    int data_encoding_id;
    int bb_first_spectrum_id;
    int is_high_resolution;
} libmzdb_spectrum_header_t;


typedef struct libmzdb_spectrum
{
    libmzdb_spectrum_header_t header;
    libmzdb_spectrum_data_t data;
} libmzdb_spectrum_t;

typedef struct libmzdb_spectrum_slice
{
    libmzdb_spectrum_t spectrum;
    int run_slice_id;
} libmzdb_spectrum_slice_t;

typedef struct libmzdb_run_slice_header
{
    int id;
    int ms_level;
    int number;
    double begin_mz;
    double end_mz;
    int run_id;
} libmzdb_run_slice_header_t;

typedef struct libmzdb_run_slice_data
{
    int id;
    libmzdb_spectrum_slice_t * spectrum_slice;
} libmzdb_run_slice_data_t;

typedef struct libmzdb_run_slice
{
    libmzdb_run_slice_header_t header;
    libmzdb_run_slice_data_t data;
} libmzdb_run_slice_t;

typedef struct libmzdb_bbox_sizes
{
    double BB_MZ_HEIGHT_MS1;
    double BB_MZ_HEIGHT_MSn;
    float BB_RT_WIDTH_MS1;
    float BB_RT_WIDTH_MSn;
} libmzdb_bbox_sizes_t;

typedef struct libmzdb_bounding_box
{
    int id;
    int first_spectrum_id;
    int last_spectrum_id;
    int run_slice_id;
    //int ms_level;
    byte* blob;
    int blob_size;
} libmzdb_bounding_box_t;

typedef struct libmzdb_indexed_bounding_box
{
    libmzdb_bounding_box_t bb;

    //spectrum_slice_t* spectrum_slices;

    int spectrum_slices_count; // number of spectrum slices in the blob
    int* spectra_ids; // list of spectra ids in the blob
    int* slices_indexes; // list of spectrum slice starting positions in the blob
    int* peaks_counts; // number of peaks in each spectrum slice of the blob
} libmzdb_indexed_bounding_box_t;

typedef libmzdb_data_point_64_32_t* libmzdb_chromatogram_t;

typedef enum libmzdb_xic_method { MAX= 0, NEAREST= 1, SUM= 2 } libmzdb_xic_method_enum;

typedef struct libmzdb_isolation_window
{
    double min_mz;
    double max_mz;
} libmzdb_isolation_window_t;

typedef struct libmzdb_entity_cache {
    libmzdb_data_encodings_cache_t data_encodings_cache;
    libmzdb_spectrum_header_t* spectrum_headers;
    int spectrum_header_count;
} libmzdb_entity_cache_t;

#endif /* MODELS_H */
