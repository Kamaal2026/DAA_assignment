#include <bits/stdc++.h>
using namespace std;
using namespace chrono;  // Include chrono for precise timing

#define ll long long
#define pb push_back

vector<vector<ll>> adj;  // Adjacency list using vectors
vector<ll> dsu_rank, parent;

// DSU Functions
void initialize(ll n) {
    parent.assign(n, 0);
    dsu_rank.assign(n, 0);
    for (ll i = 0; i < n; i++) parent[i] = i;
}

ll find_par(ll u) {
    if (parent[u] == u) return u;
    return parent[u] = find_par(parent[u]);
}

void unite(ll u, ll v) {
    u = find_par(u);
    v = find_par(v);
    if (u == v) return;
    if (dsu_rank[u] > dsu_rank[v]) swap(u, v);
    parent[u] = v;
    if (dsu_rank[u] == dsu_rank[v]) dsu_rank[v]++;
}

// Bron-Kerbosch Algorithm (Vector Version)
void BronKerbosch(vector<ll> R, vector<ll> P, vector<ll> X) {
    if (P.empty() && X.empty()) {
        cout << "Clique: { ";
        for (ll v : R) cout << v + 1 << " ";
        cout << "}\n";
        return;
    }

    // Choose pivot u with max neighbors in P
    ll pivot = -1, max_neighbors = -1;
    for (ll u : P) {
        if ((ll)adj[u].size() > max_neighbors) {
            pivot = u;
            max_neighbors = adj[u].size();
        }
    }

    // Candidates in P not adjacent to pivot
    vector<ll> candidates;
    for (ll v : P) {
        if (pivot == -1 || find(adj[pivot].begin(), adj[pivot].end(), v) == adj[pivot].end()) {
            candidates.pb(v);
        }
    }

    for (ll v : candidates) {
        vector<ll> newR = R, newP, newX;
        newR.pb(v);

        for (ll u : P) if (find(adj[v].begin(), adj[v].end(), u) != adj[v].end()) newP.pb(u);
        for (ll u : X) if (find(adj[v].begin(), adj[v].end(), u) != adj[v].end()) newX.pb(u);

        BronKerbosch(newR, newP, newX);

        // Move v from P to X
        P.erase(find(P.begin(), P.end(), v));
        X.pb(v);
    }
}

// Graph input and component-wise clique enumeration
void solve() {
    ll n, m;
    cin >> n >> m;
    adj.assign(n, vector<ll>());
    initialize(n);

    for (ll i = 0; i < m; i++) {
        ll x, y;
        cin >> x >> y;
        x--; y--;
        adj[x].pb(y);
        adj[y].pb(x);
        unite(x, y);
    }

    // Finding connected components
    unordered_map<ll, vector<ll>> components;
    for (ll i = 0; i < n; i++) {
        components[find_par(i)].pb(i);
    }

    // Start profiling with high precision
    auto start_time = high_resolution_clock::now();

    // Running Bron-Kerbosch on each connected component
    for (const auto& comp_pair : components) {
        const vector<ll>& component = comp_pair.second;
        BronKerbosch({}, component, {});
    }

    // End profiling
    auto end_time = high_resolution_clock::now();

    // Compute durations in nanoseconds and microseconds
    auto duration_micro = duration<double, micro>(end_time - start_time);
    auto duration_nano = duration<double, nano>(end_time - start_time);

    cout << fixed << setprecision(6);
    cout << "Execution Time: " << duration_micro.count() << " microseconds ("
         << duration_nano.count() << " nanoseconds)\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}
