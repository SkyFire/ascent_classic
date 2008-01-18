/************************************************************************/
/* Copyright (C) 2006 Burlex                                            */
/************************************************************************/

#ifndef _QUERYRESULT_H
#define _QUERYRESULT_H

#include "DatabaseEnv.h"

class QueryResult
{
    public:
        QueryResult(MYSQL_RES* res, uint32 FieldCount, uint32 RowCount);
        ~QueryResult();

        inline Field* Fetch() { return mCurrentRow; }
        bool NextRow();

        int GetFieldCount() const { return mFieldCount; }
        uint64 GetRowCount() const { return mRowCount; }

        const Field & operator [] (int index) { return mCurrentRow[index]; }

    protected:
        Field *mCurrentRow;
        uint32 mFieldCount;
        uint32 mRowCount;
        uint32 mRow;

        MYSQL_RES* mResult;
};

#endif
