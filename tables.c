#include "tables.h"

MZDB_TABLES getMzDbTables()
{
  MZDB_SPECTRUM_TABLE spectrumTable = { "id", "initial_id", "cycle" };
  MZDB_TABLES mzdbTables;

  mzdbTables.SPECTRUM = spectrumTable;

  return mzdbTables;
}

MZDB_TABLE_NAMES getMzDbTableNames()
{
  MZDB_TABLE_NAMES mzdbTableNames = { "spectrum" };

  return mzdbTableNames;
}
