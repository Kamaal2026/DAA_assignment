#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <algorithm>

using namespace std;

constexpr int MAX_NODES = 40000;
using Bitset = bitset<MAX_NODES>;

vector<vector<int>> adj;
vector<Bitset> adj_bitset(MAX_NODES);
vector<int> Q;  // Global clique storage
int n;          // Number of nodes

void read_graph(const string& filename) {
    ifstream file(filename);
    string line;
    getline(file, line); // Skip header

    adj.resize(MAX_NODES);
    int u, v;
    
    while (file >> u >> v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        adj_bitset[u].set(v);
        adj_bitset[v].set(u);
    }

    // Sort and deduplicate adjacency lists
    for (auto& neighbors : adj) {
        sort(neighbors.begin(), neighbors.end());
        neighbors.erase(unique(neighbors.begin(), neighbors.end()), neighbors.end());
    }

    // Find actual node count
    n = 0;
    for (int i = 0; i < MAX_NODES; ++i)
        if (!adj[i].empty()) n = i + 1;
}

int select_pivot(const Bitset& SUBG, const Bitset& CAND) {
    int best_u = -1;
    size_t max_count = 0;
    
    for (int u = SUBG._Find_first(); u < MAX_NODES; u = SUBG._Find_next(u)) {
        size_t count = (CAND & adj_bitset[u]).count();
        if (count > max_count) {
            max_count = count;
            best_u = u;
        }
    }
    return best_u != -1 ? best_u : SUBG._Find_first();
}

void print_clique() {
    cout << "clique:";
    for (int v : Q) cout << " " << v;
    cout << "\n";
}

void EXPAND(Bitset SUBG, Bitset CAND) {
    if (SUBG.none()) {
        print_clique();
        return;
    }

    int u = select_pivot(SUBG, CAND);
    Bitset EXT = CAND & ~adj_bitset[u];

    while (!EXT.none()) {
        int q = EXT._Find_first();
        Q.push_back(q);

        // Generate new candidate sets
        Bitset SUBG_q = SUBG & adj_bitset[q];
        Bitset CAND_q = CAND & adj_bitset[q];
        
        EXPAND(SUBG_q, CAND_q);
        
        Q.pop_back();
        CAND.reset(q);
        EXT = CAND & ~adj_bitset[u];
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    read_graph(argv[1]);
    Bitset all_nodes;
    for (int i = 0; i < n; ++i) all_nodes.set(i);
    
    EXPAND(all_nodes, all_nodes);
    
    return 0;
}
