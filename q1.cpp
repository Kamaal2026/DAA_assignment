#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stack>
#include <numeric>
#include <chrono>
#include <iterator>
#include <thread>

using namespace std;
using namespace chrono;

void say(){
    int i = 0;
    while(1){
        this_thread::sleep_for(chrono::seconds(100));
        cout<<i*100<<" has passed"<<endl;
        i++;
    }
}



vector<vector<int>> adj;

struct State {
    vector<int> P;
    vector<int> R;
    vector<int> X;
    
    State(vector<int> P_, vector<int> R_, vector<int> X_)
        : P(move(P_)), R(move(R_)), X(move(X_)) {}
};

inline void ordered_insert(vector<int>& vec, int val) {
    vec.insert(upper_bound(vec.begin(), vec.end(), val), val);
}

void bron_kerbosch_pivot(vector<int>& P, vector<int> R, vector<int> X, vector<int>& size_counts, int& max_size) {
    stack<State> states;
    states.emplace(P, R, X);

    while (!states.empty()) {
        auto current = move(states.top());
        states.pop();

        if (current.P.empty() && current.X.empty()) {
            const int s = current.R.size();
            if (s >= size_counts.size()) size_counts.resize(s + 1, 0);
            size_counts[s]++;
            if (s > max_size) max_size = s;
            continue;
        }

        vector<int> PuX;
        PuX.reserve(current.P.size() + current.X.size());
        merge(current.P.begin(), current.P.end(),
              current.X.begin(), current.X.end(),
              back_inserter(PuX));

        const int u = PuX.empty() ? -1 : PuX[PuX.size()/2];
        vector<int> P_diff;

        if (u != -1) {
            const auto& nu = adj[u];
            set_difference(current.P.begin(), current.P.end(),
                          nu.begin(), nu.end(),
                          inserter(P_diff, P_diff.end()));
        } else {
            P_diff = current.P;
        }

        for (int v : P_diff) {
            const auto& nv = adj[v];
            
            vector<int> P_new, X_new;
            P_new.reserve(min(current.P.size(), nv.size()));
            X_new.reserve(min(current.X.size(), nv.size()));
            
            set_intersection(current.P.begin(), current.P.end(),
                            nv.begin(), nv.end(),
                            back_inserter(P_new));
            set_intersection(current.X.begin(), current.X.end(),
                            nv.begin(), nv.end(),
                            back_inserter(X_new));
            
            vector<int> R_new = current.R;
            R_new.push_back(v);

            states.emplace(move(P_new), move(R_new), move(X_new));

            current.P.erase(lower_bound(current.P.begin(), current.P.end(), v));
            ordered_insert(current.X, v);
        }
    }
}

void read_graph(const string& filename, int& node_count) {
    ifstream file(filename);
    int max_node = -1;
    vector<pair<int, int>> edges;

    int a, b;
    while (file >> a >> b) {
        max_node = max(max_node, max(a, b));
        edges.emplace_back(min(a, b), max(a, b));
    }

    node_count = max_node + 1;
    adj.resize(node_count);

    sort(edges.begin(), edges.end());
    edges.erase(unique(edges.begin(), edges.end()), edges.end());

    for (auto& [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    #pragma omp parallel for
    for (int i = 0; i < node_count; ++i) {
        sort(adj[i].begin(), adj[i].end());
    }
}

int main(int argc, char* argv[]) {
    thread t(say);
    t.detach();
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    int node_count = 0;
    auto t1 = high_resolution_clock::now();
    read_graph(argv[1], node_count);
    auto t2 = high_resolution_clock::now();

    cout << "Graph loaded in "
         << duration_cast<milliseconds>(t2 - t1).count()
         << "ms\nNodes: " << node_count 
         << "\nEdges: " << accumulate(adj.begin(), adj.end(), 0, 
             [](int sum, auto& v) { return sum + v.size(); }) / 2 << "\n";

vector<int> P(node_count);
    iota(P.begin(), P.end(), 0);
    vector<int> size_counts;
    int max_size = 0;

    t1 = high_resolution_clock::now();
    bron_kerbosch_pivot(P, {}, {}, size_counts, max_size);
    t2 = high_resolution_clock::now();

    cout << "\n==== Results ====\n";
    cout << "Largest clique: " << max_size << "\n";
    cout << "Total cliques: " << accumulate(size_counts.begin(), size_counts.end(), 0) << "\n";
    cout << "Time: " << duration_cast<milliseconds>(t2 - t1).count() << "ms\n";
    
    cout << "\nClique size distribution:\n";
    ofstream outfile("clique_sizes.txt");
    for (int size = max_size; size > 1; --size) {
        if (size < size_counts.size() && size_counts[size] > 0) {
            cout << "Size " << size << ": " << size_counts[size] << "\n";
            if (outfile) outfile << size << " " << size_counts[size] << "\n";
        }
    }

    return 0;
}
