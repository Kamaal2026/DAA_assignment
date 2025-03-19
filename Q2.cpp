#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <iterator>

using namespace std;

void bronkkerbosch_pivot(const unordered_set<int>& R, unordered_set<int> P, unordered_set<int> X, 
                         vector<vector<int>>& cliques, 
                         const vector<unordered_set<int>>& adj_list) {
    if (P.empty() && X.empty()) {
        vector<int> clique(R.begin(), R.end());
        sort(clique.begin(), clique.end());
        cliques.push_back(clique);
        return;
    }

    // Select pivot u from P union X
    unordered_set<int> U;
    U.insert(P.begin(), P.end());
    U.insert(X.begin(), X.end());
    if (U.empty()) return;
    int u = *U.begin();

    // Iterate over P \ N(u)
    unordered_set<int> P_diff_Nu;
    for (int v : P) {
        if (adj_list.at(u).count(v) == 0) {
            P_diff_Nu.insert(v);
        }
    }

    for (int v : P_diff_Nu) {
        unordered_set<int> P_new;
        for (int w : adj_list.at(v)) {
            if (P.count(w)) P_new.insert(w);
        }
        unordered_set<int> X_new;
        for (int w : adj_list.at(v)) {
            if (X.count(w)) X_new.insert(w);
        }
        unordered_set<int> R_new = R;
        R_new.insert(v);
        bronkkerbosch_pivot(R_new, P_new, X_new, cliques, adj_list);
        P.erase(v);
        X.insert(v);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input-file>" << endl;
        return 1;
    }

    vector<pair<int, int>> edges;
    int max_node = -1;

    // Read input and build edges
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    string line;
    while (getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;
        istringstream iss(line);
        int u, v;
        iss >> u >> v;
        edges.emplace_back(u, v);
        max_node = max(max_node, max(u, v));
    }

    // Build adjacency list
    int n = max_node + 1;
    vector<unordered_set<int>> adj_list(n);
    for (const auto& e : edges) {
        int u = e.first, v = e.second;
        adj_list[u].insert(v);
        adj_list[v].insert(u);
    }

    // Compute degeneracy ordering using bucket method
    vector<int> current_degree(n);
    for (int u = 0; u < n; ++u) {
        current_degree[u] = adj_list[u].size();
    }

    map<int, unordered_set<int>> buckets;
    for (int u = 0; u < n; ++u) {
        buckets[current_degree[u]].insert(u);
    }

    vector<int> order;
    vector<bool> processed(n, false);

    for (int i = 0; i < n; ++i) {
        auto it = buckets.begin();
        while (it != buckets.end() && it->second.empty()) {
            ++it;
        }
        if (it == buckets.end()) break;

        int d = it->first;
        int u = *it->second.begin();
        it->second.erase(u);
        if (it->second.empty()) {
            buckets.erase(d);
        }

        order.push_back(u);
        processed[u] = true;

        for (int v : adj_list[u]) {
            if (!processed[v]) {
                int old_d = current_degree[v];
                buckets[old_d].erase(v);
                if (buckets[old_d].empty()) {
                    buckets.erase(old_d);
                }
                current_degree[v]--;
                buckets[current_degree[v]].insert(v);
            }
        }
    }

    // Create order position mapping
    vector<int> order_pos(n);
    for (int i = 0; i < order.size(); ++i) {
        order_pos[order[i]] = i;
    }

    // Collect all maximal cliques
    vector<vector<int>> cliques;

    for (int i = 0; i < order.size(); ++i) {
        int u = order[i];
        unordered_set<int> P, X;
        for (int v : adj_list[u]) {
            if (order_pos[v] > i) {
                P.insert(v);
            } else if (order_pos[v] < i) {
                X.insert(v);
            }
        }
        bronkkerbosch_pivot({u}, P, X, cliques, adj_list);
    }

    // Output cliques
    for (const auto& clique : cliques) {
        for (auto it = clique.begin(); it != clique.end(); ++it) {
            if (it != clique.begin()) cout << " ";
            cout << *it;
        }
        cout << endl;
    }

    return 0;
}
