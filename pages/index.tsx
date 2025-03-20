import { useState } from 'react';
import Layout from '../components/Layout';
import AlgorithmCard from '../components/AlgorithmCard';
import DatasetGenerator from '../components/DatasetGenerator';
import GraphVisualizer from '../components/GraphVisualizer';
import { motion } from 'framer-motion';

const algorithms = [
  {
    name: "Bron–Kerbosch",
    description: "Recursive backtracking algorithm for finding all maximal cliques in an undirected graph",
    complexity: "O(3^(n/3))",
    path: "/algorithms/bronk_kerbosch.cpp"
  },
  {
    name: "Degeneracy Ordering",
    description: "Optimized version using degeneracy ordering for faster execution on sparse graphs",
    complexity: "O(dn3^(d/3))",
    path: "/algorithms/bronk_kerbosch_degeneracy.cpp"
  },
  {
    name: "Tomita's Algorithm",
    description: "Efficient algorithm with pivot selection strategy to reduce the search space",
    complexity: "O(3^(n/3))",
    path: "/algorithms/tomita.cpp"
  }
];

export default function Home() {
  const [nodes, setNodes] = useState<number>(4);
  const [edges, setEdges] = useState<[number, number][]>([[1, 2], [2, 3], [3, 4], [4, 1]]);
  const [activeAlgorithm, setActiveAlgorithm] = useState<string>("Bron–Kerbosch");

  // Handler for when dataset is updated
  const handleDatasetChange = (newNodes: number, newEdges: [number, number][]) => {
    setNodes(newNodes);
    setEdges(newEdges);
  };

  return (
    <Layout>
      <motion.div
        initial={{ y: 20, opacity: 0 }}
        animate={{ y: 0, opacity: 1 }}
        className="space-y-12"
      >
        <div className="text-center">
          <h1 className="text-4xl font-bold mb-4">Maximal Clique Algorithms</h1>
          <p className="text-slate-300 max-w-2xl mx-auto">
            Interactive platform for comparing and analyzing maximal clique finding algorithms
            with real-time visualization and performance metrics.
          </p>
        </div>

        {/* Dataset Generator Section */}
        <div id="dataset-section">
          <h2 className="text-2xl font-bold mb-6">Generate Test Dataset</h2>
          <DatasetGenerator 
            onDatasetChange={handleDatasetChange}
          />
        </div>

        {/* Graph Visualization Section */}
        <div id="visualization-section">
          <h2 className="text-2xl font-bold mb-6">Interactive Visualization</h2>
          <GraphVisualizer 
            nodes={nodes} 
            edges={edges} 
            algorithm={activeAlgorithm}
          />
        </div>

        {/* Algorithms Section */}
        <div id="algorithms-section">
          <h2 className="text-2xl font-bold mb-6">Available Algorithms</h2>
          <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
            {algorithms.map((algorithm, index) => (
              <div 
                key={algorithm.name}
                onClick={() => setActiveAlgorithm(algorithm.name)}
              >
                <AlgorithmCard
                  algorithm={algorithm}
                  delay={index * 0.1}
                  isActive={activeAlgorithm === algorithm.name}
                />
              </div>
            ))}
          </div>
        </div>

        {/* Additional Information */}
        <div className="bg-slate-800 p-6 rounded-xl">
          <h2 className="text-2xl font-bold mb-4">About Maximal Cliques</h2>
          <p className="text-slate-300 mb-4">
            A clique in an undirected graph is a subset of vertices such that every two distinct vertices 
            in the clique are adjacent. A maximal clique is a clique that cannot be extended by including 
            any additional adjacent vertex.
          </p>
          <p className="text-slate-300">
            Finding all maximal cliques is important in various applications including social network analysis, 
            bioinformatics, and pattern recognition. The algorithms implemented here demonstrate different approaches 
            to solving this NP-hard problem with various optimizations.
          </p>
        </div>
      </motion.div>
    </Layout>
  );
}
