#include <bits/stdc++.h>
#include <chrono>
using namespace std;

typedef long long ll;

// ---------------------------------------------------------------------
// Optional BFS and DSU code from your template (not used for Bron–Kerbosch)

vector<ll> dsu_rank_ds, parentDSU;
void initializeDSU(ll n) {
    parentDSU.resize(n);
    dsu_rank_ds.resize(n, 0);
    for(ll i = 0; i < n; i++) {
        parentDSU[i] = i;
    }
}
ll find_par(ll u) {
    if(parentDSU[u] == u) return u;
    return parentDSU[u] = find_par(parentDSU[u]);
}
void unite(ll u, ll v) {
    u = find_par(u);
    v = find_par(v);
    if(u == v) return;
    if(dsu_rank_ds[u] > dsu_rank_ds[v]) {
        parentDSU[v] = u;
    } else if(dsu_rank_ds[u] < dsu_rank_ds[v]) {
        parentDSU[u] = v;
    } else {
        parentDSU[u] = v;
        dsu_rank_ds[v]++;
    }
}
bool same(ll u, ll v) {
    return find_par(u) == find_par(v);
}
void bfs(ll src, vector<vector<ll>>& adj, vector<ll>& vis) {
    vis[src] = 1;
    queue<pair<ll, ll>> q;
    q.push({src, 0});
    while(!q.empty()) {
        ll node = q.front().first;
        ll level = q.front().second; // level is unused, but included for completeness
        q.pop();
        for(ll neighbor : adj[node]) {
            if(!vis[neighbor]) {
                vis[neighbor] = 1;
                q.push({neighbor, level + 1});
            }
        }
    }
}
// ---------------------------------------------------------------------

// Global adjacency list
static vector<vector<ll>> adj;
// Stores all maximal cliques found by Bron–Kerbosch
static vector<vector<ll>> allCliques;

/*
 * getDegeneracyOrdering:
 *   Computes a degeneracy ordering of 'graph' in O(n + m) time using a standard
 *   "peeling" procedure:
 *      - Maintain buckets of vertices grouped by their current degree
 *      - Repeatedly remove one vertex of minimum degree, update neighbors
 */
vector<ll> getDegeneracyOrdering(const vector<vector<ll>>& graph) {
    ll n = (ll)graph.size();
    vector<ll> deg(n, 0);
    ll maxDeg = 0;

    // Compute degrees, track max degree
    for(ll i = 0; i < n; i++) {
        deg[i] = (ll)graph[i].size();
        maxDeg = max(maxDeg, deg[i]);
    }

    // Buckets D[k] for vertices of degree k
    vector<vector<ll>> D(maxDeg + 1);
    for(ll v = 0; v < n; v++) {
        D[deg[v]].push_back(v);
    }

    vector<bool> removed(n, false);
    vector<ll> ordering;
    ordering.reserve(n);

    ll currentDeg = 0;
    // Peel off all vertices
    for(ll i = 0; i < n; i++) {
        // find non-empty bucket
        while(currentDeg <= maxDeg && D[currentDeg].empty()) {
            currentDeg++;
        }
        // take one vertex from D[currentDeg]
        ll v = D[currentDeg].back();
        D[currentDeg].pop_back();
        removed[v] = true;
        ordering.push_back(v);

        // update neighbors
        for(ll nbr : graph[v]) {
            if(!removed[nbr]) {
                ll oldDeg = deg[nbr];
                ll newDeg = oldDeg - 1;
                deg[nbr] = newDeg;
                
                // Remove from old bucket and add to new bucket
                auto it = find(D[oldDeg].begin(), D[oldDeg].end(), nbr);
                if(it != D[oldDeg].end()) {
                    D[oldDeg].erase(it);
                }
                D[newDeg].push_back(nbr);
            }
        }
    }
    return ordering;
}

/*
 * bronKerboschPivot:
 *   Standard Bron–Kerbosch with pivoting (Tomita style).
 *   Finds and stores all maximal cliques containing R and considering P ∪ X.
 */
void bronKerboschPivot(vector<ll>& R, vector<ll>& P, vector<ll>& X) {
    // If P and X are both empty, we've found a maximal clique
    if(P.empty() && X.empty()) {
        allCliques.push_back(R);
        return;
    }

    // Pick a pivot 'u' from P or X
    ll u = -1;
    if(!P.empty()) {
        u = P[0];
    } else if(!X.empty()) {
        u = X[0];
    }

    // Initialize locally to avoid issues with recursion
    vector<bool> isNeighbor(adj.size(), false);
    
    // Mark neighbors of u
    for(ll nbr : adj[u]) {
        isNeighbor[nbr] = true;
    }

    // We'll only recurse on vertices in (P \ neighbors(u))
    vector<ll> expandList;
    expandList.reserve(P.size());
    for(ll v : P) {
        if(!isNeighbor[v]) {
            expandList.push_back(v);
        }
    }

    for(ll v : expandList) {
        // Include v in clique
        R.push_back(v);

        // Build P' = P ∩ Γ(v), X' = X ∩ Γ(v)
        vector<ll> newP, newX;
        newP.reserve(P.size());
        newX.reserve(X.size());

        // Initialize locally to avoid issues with recursion
        vector<bool> isNeighborV(adj.size(), false);
        
        // Mark neighbors of v
        for(ll nbr : adj[v]) {
            isNeighborV[nbr] = true;
        }

        // P'
        for(ll w : P) {
            if(w != v && isNeighborV[w]) {
                newP.push_back(w);
            }
        }
        // X'
        for(ll w : X) {
            if(isNeighborV[w]) {
                newX.push_back(w);
            }
        }

        // Recurse
        bronKerboschPivot(R, newP, newX);

        // Backtrack: remove v from R
        R.pop_back();

        // Move v from P to X
        auto it = find(P.begin(), P.end(), v);
        if(it != P.end()) {
            P.erase(it);
        }
        X.push_back(v);
    }
}

/*
 * bronKerboschDegeneracy:
 *   Implements the Eppstein–Löffler–Strash strategy:
 *   For each vertex v in degOrder:
 *     - Partition neighbors into P (later in degOrder) and X (earlier)
 *     - R = {v}
 *     - bronKerboschPivot(R, P, X)
 */
void bronKerboschDegeneracy(const vector<ll>& degOrder) {
    ll n = (ll)adj.size();
    // Precompute positions in degOrder
    vector<ll> pos(n);
    for(ll i = 0; i < n; i++) {
        pos[degOrder[i]] = i;
    }

    // For each vertex v in degOrder
    for(ll i = 0; i < n; i++) {
        ll v = degOrder[i];

        vector<ll> P, X;
        for(ll nbr : adj[v]) {
            // if nbr comes after v in degOrder => belongs to P
            if(pos[nbr] > i) {
                P.push_back(nbr);
            }
            // if nbr comes before v => belongs to X
            else if(pos[nbr] < i) {
                X.push_back(nbr);
            }
        }

        // R starts as {v}
        vector<ll> R;
        R.push_back(v);

        bronKerboschPivot(R, P, X);
    }
}

// ---------------------------------------------------------------------
// Main with more accurate time measurements for each step
// ---------------------------------------------------------------------
int main() {
    using namespace std::chrono;

    // Start overall timer right away
    auto T0 = high_resolution_clock::now();

    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // ------------------ Read input ------------------
    ll n, m;
    cin >> n >> m;
    adj.clear();
    adj.resize(n);
    for(ll i = 0; i < m; i++) {
        ll x, y;
        cin >> x >> y;
        x--;  // switch to 0-based
        y--;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
    auto T1 = high_resolution_clock::now();

    // ------------------ Degeneracy ordering ------------------
    vector<ll> degOrder = getDegeneracyOrdering(adj);
    auto T2 = high_resolution_clock::now();

    // ------------------ Bron–Kerbosch ------------------
    allCliques.clear();
    bronKerboschDegeneracy(degOrder);
    auto T3 = high_resolution_clock::now();

    // ------------------ Print results ------------------
    // Format: clique: {v1, v2, ...} with 1-based indices
    for(const auto &clique : allCliques) {
        cout << "clique: {";
        for(size_t i = 0; i < clique.size(); i++) {
            cout << (clique[i] + 1);
            if(i + 1 < clique.size()) {
                cout << ", ";
            }
        }
        cout << "}\n";
    }
    auto T4 = high_resolution_clock::now();

    // ------------------ Timing summary ------------------
    // Convert all intervals to microseconds
    ll inputMicros    = duration_cast<microseconds>(T1 - T0).count();
    ll degeneracyMicros = duration_cast<microseconds>(T2 - T1).count();
    ll bkMicros       = duration_cast<microseconds>(T3 - T2).count();
    ll printMicros    = duration_cast<microseconds>(T4 - T3).count();
    ll totalMicros    = duration_cast<microseconds>(T4 - T0).count();

    // Print them
    cout << "\n--- Profiling Information (microseconds) ---\n";
    cout << "1) Reading input:   " << inputMicros << " µs\n";
    cout << "2) Degeneracy:      " << degeneracyMicros << " µs\n";
    cout << "3) Bron-Kerbosch:   " << bkMicros << " µs\n";
    cout << "4) Printing cliques:" << printMicros << " µs\n";
    cout << "Total (T4 - T0):     " << totalMicros << " µs\n";

    // Optional: show how these add up
    ll sumParts = degeneracyMicros + bkMicros + printMicros;
    cout << "Sum of sub-steps:    " << sumParts << " µs\n";

    return 0;
}
