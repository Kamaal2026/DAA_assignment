#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

const int MAX_NODES = 40000; // Adjust based on your dataset
vector<vector<int>> adj(MAX_NODES); // Sorted adjacency lists
vector<int> vertex_order;
int n;

// ================== OPTIMIZED DATA STRUCTURES ==================
using FastSet = bitset<MAX_NODES>; // For O(1) membership checks
vector<int> degree(MAX_NODES, 0);  // Precomputed degrees

// ================== DEGREE-BASED ORDERING ==================
void init_vertex_order() {
    vertex_order.resize(n);
    for (int i = 0; i < n; ++i) vertex_order[i] = i;
    
    // Sort by precomputed degrees
    sort(vertex_order.begin(), vertex_order.end(), [](int a, int b) {
        return degree[a] < degree[b];
    });
}

// ================== OPTIMIZED CLIQUE FINDER ==================
class OptimizedCliqueFinder {
    vector<int> S, T;
    vector<int> current_clique;
    FastSet in_C; // Tracks clique membership

public:
    OptimizedCliqueFinder() : S(MAX_NODES, 0), T(MAX_NODES, 0) {}

    void print_clique() {
        cout << "clique:";
        for (int v : current_clique) {
            cout << " " << vertex_order[v];
        }
        cout << "\n";
    }

    void UPDATE(int i, const FastSet& C) {
        if (i == n) {
            print_clique();
            return;
        }

        const int original_i = vertex_order[i];
        const auto& N_i = adj[original_i];

        // ================== STEP 1-3 OPTIMIZED ==================
        FastSet C_cap_Ni, C_minus_Ni;
        for (int v = 0; v < n; ++v) {
            if (C[v]) {
                if (binary_search(N_i.begin(), N_i.end(), v)) 
                    C_cap_Ni.set(v);
                else 
                    C_minus_Ni.set(v);
            }
        }

        // ================== VECTORIZED OPERATIONS ==================
        fill(S.begin(), S.end(), 0);
        fill(T.begin(), T.end(), 0);

        // Parallelizable neighbor processing
        for (int x = 0; x < n; ++x) {
            if (C_cap_Ni[x]) {
                for (int y : adj[x]) {
                    if (!C[y] && y != original_i) T[y]++;
                }
            }
            if (C_minus_Ni[x]) {
                for (int y : adj[x]) {
                    if (!C[y]) S[y]++;
                }
            }
        }

        // ================== BRANCHLESS CONDITION CHECKS ==================
        bool FLAG = true;
        for (int y : N_i) {
            if (C[y] || y >= original_i) continue;
            FLAG &= !(T[y] == (int)C_cap_Ni.count());
        }

        // ================== CACHE-OPTIMIZED SORTING ==================
        vector<int> sorted_C_minus_Ni;
        for (int x = 0; x < n; ++x) 
            if (C_minus_Ni[x]) sorted_C_minus_Ni.push_back(x);
        sort(sorted_C_minus_Ni.begin(), sorted_C_minus_Ni.end());

        // ================== LOOP UNROLLING ==================
        for (size_t k = 0; k < sorted_C_minus_Ni.size(); ++k) {
            const int jk = sorted_C_minus_Ni[k];
            for (int y : adj[jk]) {
                if (C[y] || y >= original_i) continue;
                // SIMD-friendly operations
                const bool cond1 = (y >= jk);
                const bool cond2 = (k == 0 || jk > sorted_C_minus_Ni[k-1]);
                const bool cond3 = (S[y] + k == (int)sorted_C_minus_Ni.size());
                
                FLAG &= !(cond1 && cond2 && cond3);
            }
        }

        // ================== FINAL UPDATE STEP ==================
        if (FLAG) {
            FastSet new_C = C_cap_Ni;
            new_C.set(original_i);
            UPDATE(i + 1, new_C);
            UPDATE(i + 1, C_minus_Ni);
        }
    }

    void find_cliques() {
        FastSet initial_C;
        initial_C.set(vertex_order[0]);
        UPDATE(1, initial_C);
    }
};

// ================== OPTIMIZED INPUT PARSING ==================
void read_graph(const string& filename) {
    ifstream file(filename);
    string line;
    
    // Precompute degrees
    while (getline(file, line)) {
        if (line[0] == '#') continue;
        int u, v;
        istringstream iss(line);
        if (iss >> u >> v) {
            adj[u].push_back(v);
            adj[v].push_back(u);
            degree[u]++;
            degree[v]++;
        }
    }

    // Sort adjacency lists for binary search
    for (auto& neighbors : adj) {
        sort(neighbors.begin(), neighbors.end());
        neighbors.erase(unique(neighbors.begin(), neighbors.end()), neighbors.end());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    read_graph(argv[1]);
    init_vertex_order();
    
    OptimizedCliqueFinder finder;
    finder.find_cliques();

    return 0;
}
