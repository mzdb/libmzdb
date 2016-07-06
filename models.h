#ifndef MODELS_H
#define MODELS_H
#include <stdlib.h>
#include "lib/sqlite/sqlite3.h"


#define ACQUISITIONMODEDDA "Data Dependant Acquisition (Thermo designation), Warning: in ABI this is called IDA (Information Dependant Acquisition)"
#define ACQUISITIONMODESWATH "ABI Swath acquisition or Thermo swath acquisition"
#define ACQUISITIONMODEMRM "Multiple reaction monitoring"
#define ACQUISITIONMODESRM "SRM (Single reaction monitoring) acquisition"
#define ACQUISITIONMODEUNKNOWN "unknown acquisition mode"

#define PSI_MS_32_BIT_FLOAT "*0521"
#define PSI_MS_64_BIT_FLOAT "*0523"
#define ACQUISITION_PARAMETER "*1954"
#define ISOLATION_WINDOW_TARGET_MZ "MS:*0827"
#define ISOLATION_WINDOW_LOWER_OFFSET "MS:*0828"
#define ISOLATION_WINDOW_UPPER_OFFSET "MS:*0829"
#define SELECTED_ION_MZ "MS:*0744"


//REPLACE ALL LIST BY ARRAYS

typedef struct {
  double BB_MZ_HEIGHT_MS1;
  double BB_MZ_HEIGHT_MSn;
  float BB_RT_WIDTH_MS1;
  float BB_RT_WIDTH_MSn;
} BbSizes;

typedef enum DataMode DataMode;
enum DataMode {
  PROFILE = -1,
  CENTROID = 12,
  FITTED = 20
};

typedef enum PeakEncoding PeakEncoding;
enum PeakEncoding {
  NO_LOSS_PEAK = 16,
  HIGH_RES_PEAK = 12,
  LOW_RES_PEAK = 8
};


#pragma pack(push)
#pragma pack(1)

typedef unsigned char byte;
typedef float float32;
typedef double float64;


typedef struct {
  float32 x;
  float32 y;
}  DataPoint32_32;

typedef struct {
  float64 x;
  float64 y;
}  DataPoint64_64;

typedef struct {
  float64 x;
  float32 y;
}  DataPoint64_32;


typedef struct {
  float64 x;
  float32 y;
  float32 leftHwhm;
  float32 rightHwhm;
} FittedPeak;

typedef enum DataPrecision {
  DATA_PRECISION_UNKNOWN = 0,
  DATA_PRECISION_64_64 = 1,
  DATA_PRECISION_64_32 = 2,
  DATA_PRECISION_32_32 = 3,
  DATA_PRECISION_FITTED_64_32 = 4,
} DataPrecision;

static char * dataPrecisionString[5] = { "DATA_PRECISION_UNKNOWN", "DATA_PRECISION_64_64", "DATA_PRECISION_64_32", "DATA_PRECISION_32_32",
                                         "DATA_PRECISION_FITTED_64_32" };



#pragma pack(pop)


typedef struct {
  DataPrecision dataPrecision;
  int nbPeaks;
  union {
    DataPoint64_64 *peaks64_64;
    DataPoint64_32 *peaks64_32;
    DataPoint32_32 *peaks32_32;
    FittedPeak *peaksFitted64_32;
    void* peaks;
  };
} SpectrumData;

typedef struct {
  //TO DO : Complete once ScanList and Precursor structs are created
  long id;
  int initialId;
  char *title;
  int cycle;
  float time;
  int msLevel;
  char *activationType;
  float tic;
  double basePeakMz;
  float basePeakIntensity;
  double precursorMz;
  int precursorCharge;
  int peaksCount;
  char *paramTree;
  char *scanList;
  char *precursorList;
  char *productList;
  int sharedParamTreeID;
  int instrumentConfigurationID;
  int sourceFileID;
  int runID;
  int dataProcessingID;
  int dataEncodingID;
  int bbFirstSpectrumId;
  int isHighResolution;     //True(1) or False(0)



  /*
     /** The spectrum list.
     protected ScanList scanList = null;

     /** The precursor: contains selected ions list
     protected Precursor precursor = null; */
} SpectrumHeader;


typedef struct {
  SpectrumHeader header;
  SpectrumData data;
} Spectrum;

typedef struct {
  double mz;
  float intensity;
  float leftHwhm;
  float rightHwhm;
  //float _nf;
} Peak;

typedef struct {
  Spectrum spectrum;
  int runSliceId;
} SpectrumSlice;

typedef struct {
  int id;
  int msLevel;
  int number;
  double beginMz;
  double endMz;
  int runId;
} RunSliceHeader;

typedef struct {
  int id;
  SpectrumSlice *spectrumSliceList;
} RunSliceData;

typedef struct {
  RunSliceHeader header;
  RunSliceData data;
} RunSlice;

typedef struct {
  double minMz;
  double maxMz;
} IsolationWindow;

typedef struct {
  double x;
  int y;
} DataPoint;

typedef enum ByteOrder ByteOrder;
enum ByteOrder {
  BIG_ENDIAN,
  LITTLE_ENDIAN
};

typedef struct {
  int id;
  DataMode dataMode;
  PeakEncoding peakEncoding;
  char *compression;
  ByteOrder byteOrder;
  int peakStructSize;
} DataEncoding;

typedef struct {
  DataPoint *dataPoints;
} Chromatogram;

/*typedef struct {

    int id;
    //dataMode;
    int runSliceId;
    long firstSpectrumId;
    long lastSpectrumId;

   } BoundingBox;*/


typedef struct {
  int id;
  long firstSpectrumId;
  long lastSpectrumId;
  int runSliceId;
  int msLevel;
  SpectrumSlice *spectrumSlices;
  int nbSpectrumSlices;
} BoundingBox;

/*
   typedef struct {

    int id;
    int nbPeak;
    DataPoint64_64 *peaks;
    int runSliceId;
    long firstSpectrumId;
    long lastSpectrumId;

   } BoundingBox64_64;


   typedef struct {

    int id;
    int nbPeak;
    DataPoint32_32 *peaks;
    int runSliceId;
    long firstSpectrumId;
    long lastSpectrumId;

   } BoundingBox32_32;
 */

typedef struct {
  char *cvRef;
  char *accession;
  char *name;
  char *value;
  char *unitCvRef;
  char *unitAccession;
  char *unitName;
} CvParam;

typedef struct {
  char *cvRef;
  char *accession;
  char *name;
  char *value;
  char *type;
}UserParam;

typedef struct {
  char *cvRef;
  char *accession;
  char *name;
  char *text;
  char *type;
}UserText;

typedef struct {
  CvParam *cvParam;
  UserParam *userParam;  //List<UserParam>
  UserText *userText;  //List<UserText>
}ParamTree;

typedef enum XicMethodType XicMethodType;
enum XicMethodType {
  MAX = 0,
  NEAREST = 1,
  SUM = 2
};

typedef struct {
  XicMethodType xicMethodType;
  int val;
}XicMethod;

/*typedef struct{

    int const max = 0;
    int const nearest = 1;
    int const sum = 2;
    int val;

   }XicMethod;*/

typedef struct {
  float ms1BbMzWidth;
  float msnBbMzWidth;
  float ms1BbTimeWidth;
  float msnBbTimeWidth;
  int isLossless;
  char *originFileFormat;
}MzdbParamTree;

/*
   typedef struct{

    int count;
    ScanParamTree *scanParamTree;

   }ScanList;

   typedef struct{

    ScanWindowsList *scanWindowsList;

   }ScanParamTree;

   typedef struct{

    int count;
    ParamTree paramTree;

   }ScanWindowsList;

   //Create ScanWindow containing a List<ParamTree>

   typedef struct{

    int count;
    ParamTree paramTree;

   }SelectedIonList;

   typedef struct{

    char *Ref ;
    IsolationWindow isolationWindow;
    SelectedIonList selectedIonList;
    Activation activation;

   }Precursor;

   //IsolationWindowParamTree extends AbstractParamTree, no variables, How do I do that?
   //Activation extends AbstractParamTree, no variables, How do I do that?



   typedef struct{

    ParamTree paramTree;

   } IsolationWindow;

   typedef struct{

    ParamTree paramTree;

   } Activation;

   //SelectedIon extends AbstractParamTree, no variables, How do I do that?
 */

#endif /* MODELS_H */
