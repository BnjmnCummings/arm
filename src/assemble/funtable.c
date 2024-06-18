#include "funtable.h"


static const ftable_t ftable = {
        {"add",  &data_process},
        {"adds", &data_process},
        {"sub",  &data_process},
        {"subs", &data_process},
        {"cmp",  &data_process},
        {"cmn",  &data_process},
        {"neg",  &data_process},
        {"negs", &data_process},
        {"and",  &data_process},
        {"ands", &data_process},
        {"bic",  &data_process},
        {"bics", &data_process},
        {"eor",  &data_process},
        {"eon",  &data_process},
        {"orr",  &data_process},
        {"orn",  &data_process},
        {"tst",  &data_process},
        {"mvn",  &data_process},
        {"mov",  &data_process},
        {"movn", &data_process},
        {"movk", &data_process},
        {"movz", &data_process},
        {"madd", &data_process},
        {"msub", &data_process},
        {"mul",  &data_process},
        {"mneg", &data_process},
        {"b",    &unc_branch},
        {"br",   &reg_branch},
        {"br.eq", &con_branch},
        {"br.ne", &con_branch},
        {"br.ge", &con_branch},
        {"br.lt", &con_branch},
        {"br.gt", &con_branch},
        {"br.le", &con_branch},
        {"br.al", &con_branch},
        {"ldr",  &load_store},
        {"str",  &load_store},
        {".int", &dot_int}
};

fun_pointer getFunction(char *mnemonic) {
    pair current_pair;
    for (int i = 0; i < NUM_FUNCTIONS; i++) {
        current_pair = ftable[i];
        if (strcmp(current_pair.mnemonic, mnemonic) == 0) {
            return current_pair.function;
        }
    }
}
