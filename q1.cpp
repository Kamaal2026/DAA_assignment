#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

vector<int> set_intersection(const vector<int>& a, const vector<int>& b) {
    vector<int> result;
    auto it_a = a.begin();
    auto it_b = b.begin();
    while (it_a != a.end() && it_b != b.end()) {
        if (*it_a < *it_b) {
            ++it_a;
        } else if (*it_b < *it_a) {
            ++it_b;
        } else {
            result.push_back(*it_a);
            ++it_a;
            ++it_b;
        }
    }
    return result;
}

vector<int> set_difference(const vector<int>& a, const vector<int>& b) {
    vector<int> result;
    auto it_a = a.begin();
    auto it_b = b.begin();
    while (it_a != a.end() && it_b != b.end()) {
        if (*it_a < *it_b) {
            result.push_back(*it_a);
            ++it_a;
        } else if (*it_b < *it_a) {
            ++it_b;
        } else {
            ++it_a;
            ++it_b;
        }
    }
    while (it_a != a.end()) {
        result.push_back(*it_a);
        ++it_a;
    }
    return result;
}

int select_pivot(const vector<int>& SUBG, const vector<int>& CAND, const map<int, vector<int>>& adj) {
    int max_size = -1;
    int best_u = -1;
    for (int u : SUBG) {
        vector<int> neighbors;
        if (adj.find(u) != adj.end()) {
            neighbors = adj.at(u);
        }
        vector<int> intersection = set_intersection(CAND, neighbors);
        int current_size = intersection.size();
        if (current_size > max_size || best_u == -1) {
            max_size = current_size;
            best_u = u;
        }
    }
    return best_u;
}

void EXPAND(vector<int> SUBG, vector<int> CAND, vector<int>& Q, vector<vector<int>>& cliques, const map<int, vector<int>>& adj) {
    if (SUBG.empty()) {
        cliques.push_back(Q);
        return;
    }

    int u = select_pivot(SUBG, CAND, adj);

    vector<int> u_neighbors;
    if (adj.find(u) != adj.end()) {
        u_neighbors = adj.at(u);
    }

    while (true) {
        vector<int> current_EXT = set_difference(CAND, u_neighbors);
        if (current_EXT.empty()) {
            break;
        }

        int q = current_EXT.front();

        Q.push_back(q);

        vector<int> q_neighbors;
        if (adj.find(q) != adj.end()) {
            q_neighbors = adj.at(q);
        }

        vector<int> SUBG_q = set_intersection(SUBG, q_neighbors);
        vector<int> CAND_q = set_intersection(CAND, q_neighbors);

        EXPAND(SUBG_q, CAND_q, Q, cliques, adj);

        Q.pop_back();

        CAND = set_difference(CAND, {q});
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error opening file: " << argv[1] << endl;
        return 1;
    }

    cout << "Reading dataset..." << endl;

    set<int> nodes;
    map<int, vector<int>> adj;

    int a, b;
    while (infile >> a >> b) {
        nodes.insert(a);
        nodes.insert(b);
        adj[a].push_back(b);
        adj[b].push_back(a);
    }

    for (auto& kv : adj) {
        sort(kv.second.begin(), kv.second.end());
        auto last = unique(kv.second.begin(), kv.second.end());
        kv.second.erase(last, kv.second.end());
    }

    vector<int> V(nodes.begin(), nodes.end());
    sort(V.begin(), V.end());

    cout << "Graph Loaded Successfully!" << endl;

    vector<vector<int>> cliques;
    vector<int> Q;

    cout << "Finding Maximal cliques....." << endl;

    auto start = high_resolution_clock::now();

    EXPAND(V, V, Q, cliques, adj);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    int largest_size = 0;
    map<int, int> size_distribution;

    for (const auto& clique : cliques) {
        int size = clique.size();
        largest_size = max(largest_size, size);
        size_distribution[size]++;
    }

    cout << "\n==== Results ====" << endl;
    cout << "Largest Clique Size: " << largest_size << endl;
    cout << "Total Number of Maximal Cliques: " << cliques.size() << endl;
    cout << "Execution time (ms): " << duration.count() << endl;

    cout << "\nDistribution of different size cliques:" << endl;
    for (const auto& kv : size_distribution) {
        cout << "Size " << kv.first << ": " << kv.second << endl;
    }

    // Save clique size distribution to file
    ofstream outfile("clique_sizes.txt");
    if (outfile) {
        for (const auto& kv : size_distribution) {
            outfile << kv.first << " " << kv.second << "\n";
        }
        cout << "\nClique size distribution saved to 'clique_sizes.txt'" << endl;
    } else {
        cerr << "Error: Unable to save clique size distribution to file." << endl;
    }

    return 0;
}