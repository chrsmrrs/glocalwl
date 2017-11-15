/**********************************************************************
 * Copyright (C) 2017 Christopher Morris <christopher.morris@udo.edu>
 *
 * This file is part of globalwl.
 *
 * globalwl can not be copied and/or distributed without the express
 * permission of Christopher Morris.
 *********************************************************************/

#include "AuxiliaryMethods.h"
#include "WeisfeilerLehmanThreeLocal.h"

namespace WeisfeilerLehmanThreeLocal {
    WeisfeilerLehmanThreeLocal::WeisfeilerLehmanThreeLocal(const GraphDatabase &graph_database) : m_graph_database(
            graph_database), m_label_to_index(), m_num_labels(0), color_to_int() {}

    GramMatrix WeisfeilerLehmanThreeLocal::compute_gram_matrix(const uint num_iterations,
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
    WeisfeilerLehmanThreeLocal::compute_colors(const Graph &g, const uint num_iterations, const bool use_labels,
                                               const bool use_iso_type) {
        size_t num_nodes = g.get_num_nodes();

        unordered_map<tuple<uint, uint, uint>, uint> triple_to_int;
        vector<Triple> triples;

        // Generate all three element sets over the nodes of "g".
        size_t num_triples = 0;

        for (Node i = 0; i < num_nodes; ++i) {
            for (Node j = i + 1; j < num_nodes; ++j) {
                for (Node k = j + 1; k < num_nodes; ++k) {
                    triples.push_back(make_tuple(i, j, k));
                    triple_to_int.insert({{make_tuple(i, j, k), num_triples}});
                    num_triples++;
                }
            }
        }

        Labels coloring;
        coloring.reserve(num_triples);
        Labels coloring_temp;
        coloring_temp.reserve(num_triples);

        Labels labels;
        if (use_labels) {
            labels = g.get_labels();
        }

        ColorCounter color_map;
        // Assign isomorphism type to each 3-element set.
        for (Triple t: triples) {
            Node i = get<0>(t);
            Node j = get<1>(t);
            Node k = get<2>(t);

            Label new_color;
            if (use_labels) {
                Label c_i = labels[i];
                Label c_j = labels[j];
                Label c_k = labels[k];

                if (use_iso_type) {

                    Labels labels(
                            {{AuxiliaryMethods::pairing(g.has_edge(i, j) + g.has_edge(i, k) + 1, c_i + 1),
                                     AuxiliaryMethods::pairing(g.has_edge(j, i) + g.has_edge(j, k) + 1, c_j + 1),
                                     AuxiliaryMethods::pairing(g.has_edge(k, i) + g.has_edge(k, j) + 1, c_k + 1)
                             }
                            });

                    sort(labels.begin(), labels.end());

                    new_color = g.has_edge(i, j) + g.has_edge(i, k) + g.has_edge(j, k);
                    for (Label d: labels) {
                        new_color = AuxiliaryMethods::pairing(new_color, d);
                    }
                } else {

                    //FIX: CHeck this.
                    Labels labels({{
                                           // FIX: Is ordering relevant here?
                                           AuxiliaryMethods::pairing(AuxiliaryMethods::pairing(c_i, c_j),
                                                                     g.has_edge(i, j)),
                                           AuxiliaryMethods::pairing(AuxiliaryMethods::pairing(c_i, c_k),
                                                                     g.has_edge(i, k)),
                                           AuxiliaryMethods::pairing(AuxiliaryMethods::pairing(c_j, c_k),
                                                                     g.has_edge(j, k))
                                   }
                                  });
                    sort(labels.begin(), labels.end());

                    new_color = 1;
                    for (Label d: labels) {
                        new_color = AuxiliaryMethods::pairing(new_color, d);
                    }
                }
            } else {
                new_color = g.has_edge(i, j) + g.has_edge(i, k) + g.has_edge(j, k);
            }

            coloring[triple_to_int.find(t)->second] = new_color;

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
            for (Triple t: triples) {
                Node i = get<0>(t);
                Node j = get<1>(t);
                Node k = get<2>(t);


                // TODO: Thi
                Labels colors;
                // Get colors of neighbors.
                // Exchange node 0.
                Nodes neighbors_j = g.get_neighbours(j);
                for (Node &c: neighbors_j) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(c, j, k));

                    // FIX: Do I really need this triple_to_int here???
                    if ((it != triple_to_int.end()) and (c != i)) {
                        colors.push_back(coloring[it->second]);
                    }
                }

                Nodes neighbors_k = g.get_neighbours(k);
                for (Node &c: neighbors_k) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(c, j, k));
                    if (!g.has_edge(c, j)) {
                        if ((it != triple_to_int.end()) and (c != i)) {
                            colors.push_back(coloring[it->second]);
                        }
                    }
                }

                Nodes neighbors_i = g.get_neighbours(i);
                for (Node &c: neighbors_i) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(c, j, k));
                    if (!g.has_edge(c, j) and !g.has_edge(c, k)) {  // FIX: Is this really necessary?
                        if ((it != triple_to_int.end()) and (c != i)) {
                            colors.push_back(coloring[it->second]);
                        }
                    }
                }

                // Exchange node 1.
                neighbors_i = g.get_neighbours(i);
                for (Node &c: neighbors_i) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(i, c, k));
                    if ((it != triple_to_int.end()) and (c != j)) {
                        colors.push_back(coloring[it->second]);
                    }
                }

                neighbors_k = g.get_neighbours(k);
                for (Node &c: neighbors_k) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(i, c, k));
                    if (!g.has_edge(c, i)) {
                        if ((it != triple_to_int.end()) and (c != j)) {
                            colors.push_back(coloring[it->second]);
                        }
                    }
                }

                neighbors_j = g.get_neighbours(j);
                for (Node &c: neighbors_j) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(i, c, k));
                    if (!g.has_edge(c, i) and !g.has_edge(c, k)) {
                        if ((it != triple_to_int.end()) and (c != j)) {
                            colors.push_back(coloring[it->second]);
                        }
                    }
                }

                // Exchange node 2.
                neighbors_i = g.get_neighbours(i);
                for (Node &c: neighbors_i) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(i, j, c));
                    if ((it != triple_to_int.end()) and (c != k)) {
                        colors.push_back(coloring[it->second]);
                    }
                }

                neighbors_j = g.get_neighbours(j);
                for (Node &c: neighbors_j) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(i, j, c));
                    if (!g.has_edge(c, i)) {
                        if ((it != triple_to_int.end()) and (c != k)) {
                            colors.push_back(coloring[it->second]);
                        }
                    }
                }

                neighbors_k = g.get_neighbours(k);
                for (Node &c: neighbors_k) {
                    unordered_map<Triple, uint>::const_iterator it = triple_to_int.find(
                            make_tuple(i, j, c));
                    if (!g.has_edge(c, i) and !g.has_edge(c, j)) {
                        if ((it != triple_to_int.end()) and (c != k)) {
                            colors.push_back(coloring[it->second]);
                        }
                    }
                }

                sort(colors.begin(), colors.end());
                Label new_color = coloring[triple_to_int.find(make_tuple(i, j, k))->second];

                for (Label const &c: colors) {
                    new_color = AuxiliaryMethods::pairing(new_color, c);
                }

                coloring_temp[triple_to_int.find(t)->second] = new_color;

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


    WeisfeilerLehmanThreeLocal::~WeisfeilerLehmanThreeLocal() {}

}