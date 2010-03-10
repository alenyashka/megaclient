#ifndef MEGAPROTOCOL_H
#define MEGAPROTOCOL_H

class MegaProtocol {
    public:
    enum Commands {
        GET_TABLES_LIST,
        GET_RECORDS_LIST,
        ADD_TABLE,
        EDIT_TABLE,
        DEL_TABLE,
        ADD_RECORD,
        EDIT_RECORD,
        DEL_RECORD
    };
    enum Errors {
        TABLE_EXIST,
        RECORD_EXIST,
        RECORD_IS_READ_ONLY
    };
};
#endif // MEGAPROTOCOL_H
