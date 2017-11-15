/**********************************************************************
 * Copyright (C) 2017 Christopher Morris <christopher.morris@udo.edu>
 *
 * This file is part of globalwl.
 *
 * globalwl can not be copied and/or distributed without the express
 * permission of Christopher Morris.
 *********************************************************************/

#ifndef WLFAST_WEISFEILERLEHMANTWOGLOBAL_H
#define WLFAST_WEISFEILERLEHMANTWOGLOBAL_H

#include <cmath>
#include <unordered_map>
#include <queue>

#include "Graph.h"

using Pair = tuple<Node, Node>;
using Triple = tuple<Node, Node, Node>;

using namespace GraphLibrary;

namespace WeisfeilerLehmanTwoGlobal {
    class WeisfeilerLehmanTwoGlobal {
    public:
        WeisfeilerLehmanTwoGlobal(const GraphDatabase &graph_database);

        // Compute Gram matrix for the 3-LWL.
        GramMatrix compute_gram_matrix(const uint num_iterations, const bool use_labels, const bool use_iso_type);

        ~WeisfeilerLehmanTwoGlobal();

    private:

        // Compute lables for each graph in graph database.
        ColorCounter
        compute_colors(const Graph &g, const uint num_iterations, const bool use_labels, const bool use_iso_type);

        GraphDatabase m_graph_database;
        ColorCounter m_label_to_index;
        int m_num_labels;
    };
}

#endif //WLFAST_WEISFEILERLEHMANTWOGLOBAL_H
