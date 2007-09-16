#include "QueryResult.h"

QueryResult::QueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount) : mResult(res), mFieldCount(FieldCount), mRowCount(RowCount), mRow(0)
{
    mCurrentRow = new Field[FieldCount];
}

QueryResult::~QueryResult()
{
    delete [] mCurrentRow;
    mysql_free_result(mResult);
}

bool QueryResult::NextRow()
{
    MYSQL_ROW row = mysql_fetch_row(mResult);
    if(row == NULL)
        return false;

    for(uint32 i = 0; i < mFieldCount; ++i)
        mCurrentRow[i].SetValue(row[i]);

    return true;
}

