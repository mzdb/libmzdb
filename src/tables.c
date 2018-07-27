#include "tables.h"

/**
 * @brief get_mzdb_tables get mzdb tables
 * @return a mzbd tables
 */
MZDB_TABLES get_mzdb_tables()
{
  MZDB_SPECTRUM_TABLE spectrum_table = {"id", "initial_id", "cylce"};
  MZDB_TABLES mzdb_tables;
  mzdb_tables.SPECTRUM = spectrum_table;
  return mzdb_tables;
}

/**
 * @brief get_mzbd_tables_names get the tables names
 * @return tables names
 */
MZDB_TABLE_NAMES get_mzbd_tables_names()
{
  MZDB_TABLE_NAMES tables_names = { "spectrum" };
  return tables_names;
}
