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


/**
 * @brief get_mzdb_tables get mzdb tables
 * @return a mzbd tables
 */
MZDB_TABLES get_mzdb_tables();

/**
 * @brief get_mzbd_tables_names get the tables names
 * @return tables names
 */
MZDB_TABLE_NAMES get_mzbd_tables_names();

#endif /* TABLES_H */
