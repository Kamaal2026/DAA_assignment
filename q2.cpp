#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <string>
#include <stack>
#include <sstream>
#include <iomanip>
#include <numeric>

using namespace std;
using namespace chrono;

vector<vector<int>> adj;
vector<int> pos;
vector<int> clique_sizes;
int largest_clique_size = 0;

struct State {
    vector<int> P;
    vector<int> R;
    vector<int> X;
    State(vector<int> P_, vector<int> R_, vector<int> X_)
        : P(move(P_)), R(move(R_)), X(move(X_)) {}
};

void intersect(const vector<int>& a, const vector<int>& b, vector<int>& res) {
    res.clear();
    auto it = a.begin();
    auto jt = b.begin();
    while (it != a.end() && jt != b.end()) {
        if (*it < *jt) ++it;
        else if (*it > *jt) ++jt;
        else {
            res.push_back(*it);
            ++it;
            ++jt;
        }
    }
}

void set_difference(const vector<int>& a, const vector<int>& b, vector<int>& res) {
    res.clear();
    auto it = a.begin();
    auto jt = b.begin();
    while (it != a.end() && jt != b.end()) {
        if (*it < *jt) {
            res.push_back(*it);
            ++it;
        } else if (*it > *jt) {
            ++jt;
        } else {
            ++it;
            ++jt;
        }
    }
    while (it != a.end()) {
        res.push_back(*it);
        ++it;
    }
}

void bronKerboschPivotIterative(vector<int>& P, vector<int>& R, vector<int>& X) {
    stack<State> st;
    st.emplace(P, R, X);

    while (!st.empty()) {
        auto current = move(st.top());
        st.pop();

        if (current.P.empty() && current.X.empty()) {
            const int clique_size = current.R.size();
            if (clique_size >= 2) {
                largest_clique_size = max(largest_clique_size, clique_size);
                if (clique_sizes.size() <= clique_size) {
                    clique_sizes.resize(clique_size + 1);
                }
                clique_sizes[clique_size]++;
            }
            continue;
        }

        vector<int> PuX;
        PuX.reserve(current.P.size() + current.X.size());
        merge(current.P.begin(), current.P.end(),
              current.X.begin(), current.X.end(),
              back_inserter(PuX));
        auto last = unique(PuX.begin(), PuX.end());
        PuX.erase(last, PuX.end());

        int u = PuX.empty() ? -1 : PuX.front();
        vector<int> P_diff;

        if (u != -1) set_difference(current.P, adj[u], P_diff);
        else P_diff = current.P;

        for (int v : P_diff) {
            vector<int> newP, newX, newR;
            intersect(current.P, adj[v], newP);
            intersect(current.X, adj[v], newX);
            newR = current.R;
            newR.push_back(v);
            st.emplace(move(newP), move(newR), move(newX));

            auto it_p = lower_bound(current.P.begin(), current.P.end(), v);
            if (it_p != current.P.end() && *it_p == v) {
                current.P.erase(it_p);
            }
            current.X.insert(upper_bound(current.X.begin(), current.X.end(), v), v);
        }
    }
}

void readGraph(const string& filename, int& n) {
    ifstream file(filename);
    string line;
    int max_node = -1;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        istringstream iss(line);
        int a, b;
        if (!(iss >> a >> b)) continue;
        max_node = max(max_node, max(a, b));
    }

    n = max_node + 1;
    adj.resize(n);
    file.clear();
    file.seekg(0);

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        istringstream iss(line);
        int a, b;
        if (!(iss >> a >> b)) continue;
        if (a != b) {
            adj[a].push_back(b);
            adj[b].push_back(a);
        }
    }

    for (auto& list : adj) {
        sort(list.begin(), list.end());
        auto last = unique(list.begin(), list.end());
        list.erase(last, list.end());
    }
}

vector<int> coreDecomposition(int n) {
    vector<int> degree(n);
    for (int i = 0; i < n; ++i) degree[i] = adj[i].size();

    int max_degree = *max_element(degree.begin(), degree.end());
    vector<int> bin(max_degree + 2, 0);
    for (int d : degree) bin[d + 1]++;
    for (int d = 0; d <= max_degree; ++d) bin[d + 1] += bin[d];

    vector<int> vert(n), pos_arr(n);
    for (int i = 0; i < n; ++i) {
        pos_arr[i] = bin[degree[i]]++;
        vert[pos_arr[i]] = i;
    }

    for (int d = max_degree; d >= 1; --d) bin[d] = bin[d - 1];
    bin[0] = 0;

    vector<int> order;
    for (int i = 0; i < n; ++i) {
        int v = vert[i];
        order.push_back(v);
        for (int u : adj[v]) {
            if (degree[u] > degree[v]) {
                int du = degree[u];
                int pu = pos_arr[u];
                int pw = bin[du];
                int w = vert[pw];
                if (u != w) {
                    pos_arr[u] = pw;
                    pos_arr[w] = pu;
                    vert[pu] = w;
                    vert[pw] = u;
                }
                bin[du]++;
                degree[u]--;
            }
        }
    }
    return order;
}

void saveResults(const time_point<high_resolution_clock>& start,
                const time_point<high_resolution_clock>& read_time,
                const time_point<high_resolution_clock>& core_time,
                const time_point<high_resolution_clock>& pos_time,
                const time_point<high_resolution_clock>& bk_time) {
    ofstream clique_file("clique_sizes.txt");
    for (size_t i = 2; i < clique_sizes.size(); ++i) {
        if (clique_sizes[i] > 0) {
            clique_file << i << " " << clique_sizes[i] << "\n";
        }
    }
    clique_file.close();

    ofstream profile_file("profiling.txt");
    profile_file << "==== PROFILING ====\n";
    profile_file << "Read time: " 
                << duration_cast<milliseconds>(read_time - start).count() 
                << " ms\n";
    profile_file << "Core decomposition: " 
                << duration_cast<milliseconds>(core_time - read_time).count() 
                << " ms\n";
    profile_file << "Position computation: " 
                << duration_cast<milliseconds>(pos_time - core_time).count() 
                << " ms\n";
    profile_file << "Bron-Kerbosch time: " 
                << duration_cast<milliseconds>(bk_time - pos_time).count() 
                << " ms\n";
    profile_file.close();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    auto start = high_resolution_clock::now();
    int n = 0;
    readGraph(argv[1], n);
    auto read_time = high_resolution_clock::now();
    cout << "Dataset Read Sucessfully\n";

    vector<int> order = coreDecomposition(n);
    auto core_time = high_resolution_clock::now();

    pos.resize(n);
    for (int i = 0; i < n; ++i) pos[order[i]] = i;
    auto pos_time = high_resolution_clock::now();

    for (int i = 0; i < n; ++i) {
        int v_i = order[i];
        vector<int> P, X;
        for (int w : adj[v_i]) {
            if (pos[w] > i) P.push_back(w);
            else X.push_back(w);
        }
        sort(P.begin(), P.end());
        sort(X.begin(), X.end());
        vector<int> R = {v_i};
        bronKerboschPivotIterative(P, R, X);
    }
    auto bk_time = high_resolution_clock::now();

    int total_cliques = accumulate(clique_sizes.begin(), clique_sizes.end(), 0);
    cout << "==== RESULTS ====\n"
         << "Largest Clique Size: " << largest_clique_size << "\n"
         << "Total Maximal Cliques: " << total_cliques << "\n"
         << "Full execution time: " 
         << duration_cast<milliseconds>(bk_time - start).count() 
         << " ms\n"<<" saved to profiling.txt and clique_sizes.txt"<<endl;

    saveResults(start, read_time, core_time, pos_time, bk_time);

    return 0;
}