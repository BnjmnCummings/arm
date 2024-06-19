#include "funtable.h"
#include "binbuilder.h"


static const pair ftable[] = {
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
        {"b.eq", &con_branch},
        {"b.ne", &con_branch},
        {"b.ge", &con_branch},
        {"b.lt", &con_branch},
        {"b.gt", &con_branch},
        {"b.le", &con_branch},
        {"b.al", &con_branch},
        {"ldr",  &load_store},
        {"str",  &load_store},
        {".int", &dot_int}
};

//returns the 'binbuilder' function associated with a given mnemonic
fun_pointer get_bin_function(char *mnemonic) {
    pair current_pair;
    for (int i = 0; i < NUM_FUNCTIONS; i++) {
        current_pair = ftable[i];
        if (strcmp(current_pair.mnemonic, mnemonic) == 0) {
            return current_pair.function;
        }
    }
    return NULL;
}
