#ifndef TABLES_H
#define TABLES_H

typedef struct {
  char *ID; // 2 character array
  char *INITIAL_ID; // 2 character array
  char *CYCLE; // 2 character array
} MZDB_SPECTRUM_TABLE;

typedef struct {
  MZDB_SPECTRUM_TABLE SPECTRUM;
} MZDB_TABLES;

typedef struct {
  char *SPECTRUM;
} MZDB_TABLE_NAMES;

MZDB_TABLES getMzDbTables();

MZDB_TABLE_NAMES getMzDbTableNames();

#endif /* TABLES_H */
