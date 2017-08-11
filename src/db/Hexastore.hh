#ifndef HEXASTORE_HH_
#define HEXASTORE_HH_

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <sqlite3.h>
#include <iostream>
#include "Queries.hh"
#include <boost/format.hpp>

using namespace std;

namespace hexastore
{
/**
 * @brief Holds the results of a query to the Hexastore.
 */
struct hs_result {
  /// Used for the source node.
  string subject;
  /// Used for the edge.
  string predicate;
  /// Used for the destination node.
  string object;
  hs_result() = default;
  hs_result(string s, string p, string o) : subject(s), predicate(p), object(o) {}
  /// Prints an entry of the results to the command-line
  friend ostream& operator<< (ostream& os, const hs_result& hsr) {
    return os << "[ subject: " << hsr.subject << " | predicate: " << hsr.predicate << " | object: " << hsr.object << " ]";
  }
};
/**
 * A Hexastore is an efficient approach to store large graphs.
 * This approach is based on the paper "Database-Backed Program Analysis
 * for Scalable Error Propagation" by Weiss, Rubio-González and Libit.
 *
 * To store a graph into a database, we represent the graph by a set of
 * string 3-tuples by the form:
 *
 *          (source node, edge, destination node)
 *
 * A Hexastore indexes and saves graphs six-fold, according to each of
 * the permutations of source, edge and destination label. For example,
 * this allows to quickly find all edges for a given source node with one
 * look-up. In general, given one or two fixed elements of a (source, edge,
 * destination) tuple, Hexastore can quickly access the related information.
 *
 * @brief Efficient data structure for holding graphs in databases.
 */
class Hexastore {
private:
  sqlite3* hs_internal_db;
  static int callback(void *NotUsed, int argc, char **argv, char **azColName);
  void doPut(string query, array<string, 3> edge);

public:
  /**
   * If the given filename matches an already created Hexastore, no
   * new Hexastore will be created. Instead the already created Hexastore
   * will be used.
   *
   * @brief Constructs a Hexastore under the given filename.
   * @param filename Filename of the Hexastore.
   */
  Hexastore(string filename);

  /**
   * Destructor.
   */
  ~Hexastore();

  /**
   * Adds the given tuple as a new entry to the Hexastore. It is not
   * possible to have duplicate entries in the Hexastore and
   * duplicate put queries will be silently ignored by the Hexastore.
   *
   * @brief Creates a new entry in the Hexastore.
   * @note To mitigate certain compiler warnings, it is advised to use
   *        double braces, e.g.:
   *        hexastore.put({{"subject", "predicate", "object"}});
   * @param edge New entry in the form of a 3-tuple.
   */
  void put(array<string, 3> edge);

  /**
   * A query is always in the form of a 3-tuple (source, edge, destination) where
   * none/one/two or all three elements of the tuple are given, while the not fixed
   * elements are represented by a "?". E.g. to query all destination nodes
   * for a certain source node and edge, the following query is used:
   *
   *    ("node_19", "edge_3", "?")
   *
   * @brief Query information from the Hexastore.
   * @param edge_query Query in the form of a 3-tuple.
   * @param result_size_hint Used for possible optimization.
   * @return An object of hs_result, holding the queried information.
   */
  vector<hs_result> get(array<string, 3> edge_query, size_t result_size_hint=0);
};

} /* end namespace hexastore */

#endif /* HEXASTORE_HH_ */
