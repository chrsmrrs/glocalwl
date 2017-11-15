/**********************************************************************
 * Copyright (C) 2017 Christopher Morris <christopher.morris@udo.edu>
 *
 * This file is part of globalwl.
 *
 * globalwl can not be copied and/or distributed without the express
 * permission of Christopher Morris.
 *********************************************************************/

#include <cstdio>
#include "src/AuxiliaryMethods.h"
#include "src/WeisfeilerLehmanThreeLocal.h"

using namespace std;

int main() {

    string graph_database_name = "ENZYMES";
    string kernel = "LWL3";
    cout << graph_database_name << " " << kernel << endl;
    GraphDatabase gdb = AuxiliaryMethods::read_graph_txt_file(graph_database_name);

    WeisfeilerLehmanThreeLocal::WeisfeilerLehmanThreeLocal wl(gdb);


    GramMatrix gm = wl.compute_gram_matrix(3, true, false);

    AuxiliaryMethods::write_gram_matrix(gm, "ENZYMES");

    return 0;
}

