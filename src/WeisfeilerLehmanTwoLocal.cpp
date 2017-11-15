/**********************************************************************
 * Copyright (C) 2017 Christopher Morris <christopher.morris@udo.edu>
 *
 * This file is part of globalwl.
 *
 * globalwl can not be copied and/or distributed without the express
 * permission of Christopher Morris.
 *********************************************************************/

#include "AuxiliaryMethods.h"
#include "WeisfeilerLehmanTwoLocal.h"

namespace WeisfeilerLehmanTwoLocal {
    WeisfeilerLehmanTwoLocal::WeisfeilerLehmanTwoLocal(const GraphDatabase &graph_database) : m_graph_database(
            graph_database), m_label_to_index(), m_num_labels(0) {}

    GramMatrix WeisfeilerLehmanTwoLocal::compute_gram_matrix(const uint num_iterations,
                                                             const bool use_labels,
                                                             bool use_iso_type) {
        vector<ColorCounter> color_counters;
        color_counters.reserve(m_graph_database.size());


        for (auto &graph: m_graph_database) {
            color_counters.push_back(compute_colors(graph, num_iterations, use_labels, use_iso_type));
        }


        ulong num_graphs = m_graph_database.size();
        vector<S> nonzero_compenents;

        for (ulong i = 0; i < num_graphs; ++i) {
            ColorCounter c = color_counters[i];

            for (const auto &j: c) {
                Label key = j.first;
                uint value = j.second;
                uint index = m_label_to_index.find(key)->second;
                nonzero_compenents.push_back(move(S(i, index, value)));
            }
        }

        GramMatrix feature_vectors(num_graphs, m_num_labels);
        feature_vectors.setFromTriplets(nonzero_compenents.begin(), nonzero_compenents.end());


        GramMatrix gram_matrix(num_graphs, num_graphs);
        gram_matrix = feature_vectors * feature_vectors.transpose();

        return gram_matrix;
    }


    ColorCounter
    WeisfeilerLehmanTwoLocal::compute_colors(const Graph &g, const uint num_iterations, const bool use_labels,
                                             const bool use_iso_type) {
        size_t num_nodes = g.get_num_nodes();

        unordered_map<tuple<uint, uint>, uint> pair_to_int;
        vector<Pair> pair;

        // Generate all three element sets over the nodes of "g".
        size_t num_pairs = 0;
        for (Node i = 0; i < num_nodes; ++i) {
            for (Node j = i + 1; j < num_nodes; ++j) {
                pair.push_back(make_tuple(i, j));
                pair_to_int.insert({{make_tuple(i, j), num_pairs}});
                num_pairs++;
            }
        }

        Labels coloring;
        coloring.reserve(num_pairs);
        Labels coloring_temp;
        coloring_temp.reserve(num_pairs);

        Labels labels;
        if (use_labels) {
            labels = g.get_labels();
        }

        ColorCounter color_map;
        // Assign isomorphism type to each 2-element set.
        for (Pair t: pair) {
            Node i = get<0>(t);
            Node j = get<1>(t);

            Label new_color;
            if (use_labels) {
                Label c_i = labels[i];
                Label c_j = labels[j];

                if (c_i < c_j) {
                    new_color = AuxiliaryMethods::pairing(AuxiliaryMethods::pairing(c_i, c_j), g.has_edge(i, j));
                } else {
                    new_color = AuxiliaryMethods::pairing(AuxiliaryMethods::pairing(c_j, c_i), g.has_edge(i, j));
                }
            } else {
                new_color = g.has_edge(i, j);
            }

            coloring[pair_to_int.find(t)->second] = new_color;

            ColorCounter::iterator it(color_map.find(new_color));
            if (it == color_map.end()) {
                color_map.insert({{new_color, 1}});
                m_label_to_index.insert({{new_color, m_num_labels}});
                m_num_labels++;
            } else {
                it->second++;
            }
        }

        uint h = 1;
        while (h <= num_iterations) {
            for (Pair t: pair) {
                Node i = get<0>(t);
                Node j = get<1>(t);


                Labels colors;
                // Get colors of neighbors.
                // Exchange node 0.
                Nodes neighbors_j = g.get_neighbours(j);
                for (Node &c: neighbors_j) {
                    unordered_map<Pair, uint>::const_iterator it = pair_to_int.find(make_tuple(c, j));

                    if ((it != pair_to_int.end()) and (c != i)) {
                        colors.push_back(coloring[it->second]);
                    }
                }

                // Exchange node 1.
                Nodes neighbors_i = g.get_neighbours(i);
                for (Node &c: neighbors_i) {
                    unordered_map<Pair, uint>::const_iterator it = pair_to_int.find(make_tuple(i, c));
                    if ((it != pair_to_int.end()) and (c != j)) {
                        colors.push_back(coloring[it->second]);
                    }
                }

                sort(colors.begin(), colors.end());
                Label new_color = coloring[pair_to_int.find(make_tuple(i, j))->second];

                for (Label const &c: colors) {
                    new_color = AuxiliaryMethods::pairing(new_color, c);
                }

                coloring_temp[pair_to_int.find(t)->second] = new_color;

                // Keep track how often "new_color" occurs.
                ColorCounter::iterator it(color_map.find(new_color));
                if (it == color_map.end()) {
                    color_map.insert({{new_color, 1}});
                    m_label_to_index.insert({{new_color, m_num_labels}});
                    m_num_labels++;
                } else {
                    it->second++;
                }
            }

            coloring = coloring_temp;
            h++;
        }

        return color_map;
    }

    WeisfeilerLehmanTwoLocal::~WeisfeilerLehmanTwoLocal() {}
}
