#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <unordered_set>

using namespace std;

vector<unordered_set<int>> adj; // Adjacency list (0-based)
vector<int> vertex_order;        // Vertex ordering based on degree (1-based in pseudocode)
int n;                           // Number of vertices

// Comparator to sort vertices by ascending degree
bool compare_degree(int a, int b) {
    return adj[a].size() < adj[b].size();
}

// Initialize vertex ordering by degree
void init_vertex_order() {
    vertex_order.resize(n);
    for (int i = 0; i < n; ++i) vertex_order[i] = i;
    sort(vertex_order.begin(), vertex_order.end(), compare_degree);
}

class CliqueFinder {
    vector<int> S, T;
    vector<int> current_clique;

public:
    CliqueFinder() : S(n, 0), T(n, 0) {}

    void print_clique() {
        cout << "clique:";
        for (int v : current_clique) {
            cout << " " << vertex_order[v]; // Map back to original vertex numbering
        }
        cout << endl;
    }

    void UPDATE(int i, unordered_set<int>& C) {
        if (i == n) {
            print_clique();
            return;
        }

        int original_i = vertex_order[i]; // Current vertex in original graph
        unordered_set<int> N_i = adj[original_i]; // Neighbors of vertex i

        // Step 1: Check if C - N(i) is not empty
        unordered_set<int> C_minus_Ni;
        for (int v : C) {
            if (!N_i.count(v)) C_minus_Ni.insert(v);
        }

        if (!C_minus_Ni.empty()) {
            UPDATE(i + 1, C);
        }

        // Compute T[y] = |N(y) ∩ C ∩ N(i)|
        for (int y = 0; y < n; ++y) {
            if (C.count(y) || y == original_i) continue;
            int count = 0;
            for (int c : C) {
                if (N_i.count(c) && adj[y].count(c)) ++count;
            }
            T[y] = count;
        }

        // Step 2: Update T[y] for x ∈ C ∩ N(i)
        unordered_set<int> C_cap_Ni;
        for (int x : C) {
            if (N_i.count(x)) C_cap_Ni.insert(x);
        }

        for (int x : C_cap_Ni) {
            for (int y : adj[x]) {
                if (C.count(y) || y == original_i) continue;
                T[y]++;
            }
        }

        // Step 3: Update S[y] for x ∈ C - N(i)
        for (int x : C_minus_Ni) {
            for (int y : adj[x]) {
                if (C.count(y)) continue;
                S[y]++;
            }
        }

        bool FLAG = true;

        // Step 4: Check maximality condition
        for (int y : N_i) {
            if (C.count(y) || y >= original_i) continue;
            if (T[y] == C_cap_Ni.size()) {
                FLAG = false;
                break;
            }
        }

        // Step 5: Sort C - N(i) in ascending order
        vector<int> sorted_C_minus_Ni(C_minus_Ni.begin(), C_minus_Ni.end());
        sort(sorted_C_minus_Ni.begin(), sorted_C_minus_Ni.end());

        // Step 6: Adjust S[y] and check lexicographic conditions
        for (int k = 0; k < sorted_C_minus_Ni.size(); ++k) {
            int jk = sorted_C_minus_Ni[k];
            for (int y : adj[jk]) {
                if (C.count(y) || y >= original_i) continue;
                if (T[y] == C_cap_Ni.size()) {
                    if (y >= jk) {
                        S[y]--;
                    } else {
                        if (k == 0 || jk > sorted_C_minus_Ni[k-1]) {
                            if (S[y] + k == sorted_C_minus_Ni.size() && y >= (k == 0 ? 0 : sorted_C_minus_Ni[k-1])) {
                                FLAG = false;
                            }
                        }
                    }
                }
            }
        }

        // Step 7: Additional checks for S[y]
        if (!C_cap_Ni.empty()) {
            for (int x : C_cap_Ni) {
                for (int y : adj[x]) {
                    if (C.count(y) || y == original_i || y >= original_i) continue;
                    if (T[y] == C_cap_Ni.size() && S[y] == 0) {
                        if (vertex_order[y] < y) { // j_y < y condition
                            FLAG = false;
                        }
                    }
                }
            }
        }

        // Steps 8-9: Reset T and S
        for (int x : C_cap_Ni) {
            for (int y : adj[x]) {
                T[y] = 0;
            }
        }

        for (int x : C_minus_Ni) {
            for (int y : adj[x]) {
                S[y] = 0;
            }
        }

        // Step 10: Proceed if FLAG is true
        if (FLAG) {
            unordered_set<int> SAVE = C_minus_Ni;
            unordered_set<int> new_C = C_cap_Ni;
            new_C.insert(original_i);
            UPDATE(i + 1, new_C);
            C = SAVE; // Restore C
        }
    }

    void find_cliques() {
        init_vertex_order();
        unordered_set<int> C;
        C.insert(vertex_order[0]); // Start with vertex 1 in original ordering
        UPDATE(1, C);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream file(argv[1]);
    string line;
    getline(file, line); // Skip header

    // Read edges and build adjacency list
    unordered_set<int> all_nodes;
    vector<pair<int, int>> edges;
    int u, v;
    while (file >> u >> v) {
        edges.emplace_back(u, v);
        all_nodes.insert(u);
        all_nodes.insert(v);
    }

    n = all_nodes.size();
    adj.resize(n);

    // Build adjacency list (assuming nodes are 0-based)
    for (auto& [u, v] : edges) {
        adj[u].insert(v);
        adj[v].insert(u);
    }

    CliqueFinder finder;
    finder.find_cliques();

    return 0;
}
