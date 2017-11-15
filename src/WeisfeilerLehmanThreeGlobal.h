/**********************************************************************
 * Copyright (C) 2017 Christopher Morris <christopher.morris@udo.edu>
 *
 * This file is part of globalwl.
 *
 * globalwl can not be copied and/or distributed without the express
 * permission of Christopher Morris.
 *********************************************************************/

#ifndef WLFAST_WEISFEILERLEHMANTHREEGLOBAL_H
#define WLFAST_WEISFEILERLEHMANTHREEGLOBAL_H

#include <unordered_map>

#include "AuxiliaryMethods.h"
#include "Graph.h"

using Triple = tuple<Node, Node, Node>;

using namespace GraphLibrary;
using namespace AuxiliaryMethods;

namespace WeisfeilerLehmanThreeGlobal {
    class WeisfeilerLehmanThreeGlobal {
    public:
        WeisfeilerLehmanThreeGlobal(const GraphDatabase &graph_database);

        // Compute Gram Matrix for each graph in the graph database.
        GramMatrix compute_gram_matrix(const uint num_iterations, const bool use_labels, const bool use_iso_type);

        ~WeisfeilerLehmanThreeGlobal();

    private:
        // Compute labels for a graph.
        ColorCounter
        compute_colors(const Graph &g, const uint num_iterations, const bool use_labels, const bool use_iso_type);

        // Manages graphs.
        GraphDatabase m_graph_database;

        // Manage indices of of labels in feature vectors.
        ColorCounter m_label_to_index;

        // Counts number of distinct labels over all graphs.
        uint m_num_labels;
    };
}

#endif //WLFAST_WEISFEILERLEHMANTHREEGLOBAL_H
