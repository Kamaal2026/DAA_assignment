#include <iostream>
#include <vector>
#include <stack>
using namespace std;

class Graph {
private:
    int n; // Number of vertices
    std::vector<std::vector<bool>> adj; // Adjacency matrix
    std::stack<int> current_clique; // Stack to store the current clique vertices

public:
    Graph(int vertices) : n(vertices), adj(vertices, std::vector<bool>(vertices, false)) {}

    void addEdge(int u, int v) {
        adj[u][v] = adj[v][u] = true;
    }

    void EXPAND(std::vector<int>& SUBG, std::vector<int>& CAND) {
        if (SUBG.empty() && CAND.empty()) {
            // Print the maximal clique
            std::cout << endl;
            std::stack<int> temp_stack = current_clique; // Temporary stack to print elements
            std::vector<int> clique_elements;
            while (!temp_stack.empty()) {
                clique_elements.push_back(temp_stack.top());
                temp_stack.pop();
            }
            for (size_t i = 0; i < clique_elements.size(); ++i) {
                std::cout << clique_elements[clique_elements.size() - 1 - i];
                if (i < clique_elements.size() - 1) std::cout << ", ";
            }
            return;
        }

        while (!CAND.empty()) {
            int u = CAND.back();
            CAND.pop_back();
            current_clique.push(u); // Push vertex onto the stack

            std::vector<int> new_SUBG, new_CAND;
            for (int v : SUBG) {
                if (adj[u][v]) new_SUBG.push_back(v);
            }
            for (int v : CAND) {
                if (adj[u][v]) new_CAND.push_back(v);
            }

            EXPAND(new_SUBG, new_CAND);
            
            current_clique.pop();
            cout<<", back";
        }
    }

    void generateMaximalCliques() {
        std::vector<int> SUBG(n), CAND(n);
        for (int i = 0; i < n; ++i) {
            SUBG[i] = CAND[i] = i;
        }
        EXPAND(SUBG, CAND);
    }
};

int main() {
    int n,u,v,e;
    cin>>n>>e;
    Graph g(n);
    for(int i=0;i<e;i++)
     {
      cin>>u>>v;
      g.addEdge(u,v);
     }

    g.generateMaximalCliques();

    return 0;
}
