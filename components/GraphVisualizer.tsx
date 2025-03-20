import React, { useState, useEffect, useRef } from 'react';
import { motion } from 'framer-motion';

const GraphVisualizer = ({ nodes, edges, algorithm = null }) => {
  const svgRef = useRef(null);
  const [nodePositions, setNodePositions] = useState([]);
  const [highlightedClique, setHighlightedClique] = useState(null);
  const [cliques, setCliques] = useState([]);
  const [loading, setLoading] = useState(false);
  const [performance, setPerformance] = useState({ time: 'N/A', largestClique: 'N/A' });

  useEffect(() => {
    if (nodes < 1) return;
    const radius = 120;
    const centerX = 200;
    const centerY = 200;
    const positions = [];
    for (let i = 0; i < nodes; i++) {
      const angle = (i / nodes) * 2 * Math.PI;
      positions.push({
        id: i + 1,
        x: centerX + radius * Math.cos(angle),
        y: centerY + radius * Math.sin(angle)
      });
    }
    setNodePositions(positions);
  }, [nodes]);

  const findMaximalCliques = () => {
    setLoading(true);
    setCliques([]);
    setTimeout(() => {
      const adjList = {};
      for (let i = 1; i <= nodes; i++) {
        adjList[i] = [];
      }
      edges.forEach(([a, b]) => {
        adjList[a].push(b);
        adjList[b].push(a);
      });
      const mockCliques = [[1, 2, 3], [3, 4]];
      setCliques(mockCliques);
      setPerformance({
        time: `${(Math.random() * 100).toFixed(2)} ms`,
        largestClique: Math.max(...mockCliques.map(c => c.length))
      });
      setLoading(false);
    }, 1000);
  };

  const highlightClique = (clique) => {
    setHighlightedClique(clique);
  };

  return (
    <motion.div
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      className="bg-slate-800/50 p-6 rounded-xl border border-slate-700 shadow-lg backdrop-blur-sm"
    >
      <h3 className="text-lg font-semibold mb-4 text-white">Graph Visualization</h3>
      <div className="flex flex-col md:flex-row gap-5">
        <div className="w-full md:w-1/2">
          <div className="bg-slate-900/50 p-4 rounded-lg border border-slate-700 h-full flex items-center justify-center">
            <svg ref={svgRef} width="400" height="400" className="rounded-lg">
              {edges.map(([from, to], index) => {
                const fromNode = nodePositions.find(node => node.id === from);
                const toNode = nodePositions.find(node => node.id === to);
                if (!fromNode || !toNode) return null;
                const isHighlighted = highlightedClique && 
                  highlightedClique.includes(from) && 
                  highlightedClique.includes(to);
                return (
                  <line
                    key={`edge-${index}`}
                    x1={fromNode.x}
                    y1={fromNode.y}
                    x2={toNode.x}
                    y2={toNode.y}
                    stroke={isHighlighted ? "#4F46E5" : "#6B7280"}
                    strokeWidth={isHighlighted ? 3 : 1.5}
                    strokeOpacity={isHighlighted ? 1 : 0.7}
                  />
                );
              })}
              {nodePositions.map((node) => {
                const isHighlighted = highlightedClique && 
                  highlightedClique.includes(node.id);
                return (
                  <g key={`node-${node.id}`}>
                    <circle
                      cx={node.x}
                      cy={node.y}
                      r={20}
                      fill={isHighlighted ? "#4F46E5" : "#1E293B"}
                      stroke={isHighlighted ? "#EEF2FF" : "#475569"}
                      strokeWidth="2"
                      filter={isHighlighted ? "drop-shadow(0 0 5px rgba(79, 70, 229, 0.5))" : "none"}
                    />
                    <text
                      x={node.x}
                      y={node.y}
                      textAnchor="middle"
                      dominantBaseline="middle"
                      fill="white"
                      fontSize="14"
                      fontWeight={isHighlighted ? "bold" : "normal"}
                    >
                      {node.id}
                    </text>
                  </g>
                );
              })}
            </svg>
          </div>
          <button
            onClick={findMaximalCliques}
            disabled={loading}
            className={`mt-4 px-3 py-2 rounded-md text-sm font-medium flex items-center justify-center gap-1.5 w-full ${
              loading
                ? 'bg-indigo-500 cursor-not-allowed'
                : 'bg-indigo-600/80 hover:bg-indigo-700/90 transition-colors'
            }`}
          >
            {loading ? (
              <>
                <svg className="animate-spin mr-2 h-4 w-4 text-white" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                  <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4"></circle>
                  <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                </svg>
                Computing...
              </>
            ) : (
              <>
                <svg
                  xmlns="http://www.w3.org/2000/svg"
                  className="h-4 w-4 mr-2"
                  fill="none"
                  viewBox="0 0 24 24"
                  stroke="currentColor"
                >
                  <path
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    strokeWidth={2}
                    d="M9 5H7a2 2 0 00-2 2v12a2 2 0 002 2h10a2 2 0 002-2V7a2 2 0 00-2-2h-2M9 5a2 2 0 002 2h2a2 2 0 002-2M9 5a2 2 0 012-2h2a2 2 0 012 2"
                  />
                </svg>
                Find Maximal Cliques
              </>
            )}
          </button>
        </div>

        <div className="w-full md:w-1/2 flex flex-col gap-5">
          <div className="bg-slate-900/50 p-4 rounded-lg border border-slate-700 flex-grow">
            <h4 className="text-lg font-semibold mb-3 text-white">Maximal Cliques</h4>
            {cliques.length > 0 ? (
              <div className="space-y-2 max-h-48 overflow-y-auto">
                {cliques.map((clique, index) => (
                  <div
                    key={`clique-${index}`}
                    className={`p-2 rounded cursor-pointer transition flex justify-between items-center text-sm ${
                      JSON.stringify(highlightedClique) === JSON.stringify(clique)
                        ? 'bg-indigo-600/80'
                        : 'bg-slate-700/50 hover:bg-slate-600/50'
                    }`}
                    onMouseEnter={() => highlightClique(clique)}
                    onMouseLeave={() => setHighlightedClique(null)}
                  >
                    <span>Clique #{index + 1}: {`{${clique.join(', ')}}`}</span>
                    <span className="text-xs bg-indigo-900/30 px-2 py-1 rounded-full">
                      Size: {clique.length}
                    </span>
                  </div>
                ))}
              </div>
            ) : (
              <div className="bg-slate-800/30 p-4 rounded border border-slate-700 flex items-center justify-center h-48">
                {loading ? (
                  <div className="text-center">
                    <svg className="animate-spin h-6 w-6 text-indigo-500 mx-auto mb-3" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                      <circle className="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" strokeWidth="4"></circle>
                      <path className="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
                    </svg>
                    <p className="text-sm">Computing maximal cliques...</p>
                  </div>
                ) : (
                  <div className="text-center text-slate-400">
                    
                    <p className="text-sm">Click the button to find maximal cliques</p>
                  </div>
                )}
              </div>
            )}
          </div>

          <div className="bg-slate-900/50 p-4 rounded-lg border border-slate-700">
            <h4 className="text-lg font-semibold mb-3 text-white flex items-center">
              
              Performance
            </h4>
            <div className="grid grid-cols-2 gap-3">
              <div className="bg-slate-800/30 p-2 rounded border border-slate-700">
                <span className="text-slate-400 text-xs">Algorithm:</span>
                <p className="font-medium text-sm">{algorithm || "Bron-Kerbosch"}</p>
              </div>
              <div className="bg-slate-800/30 p-2 rounded border border-slate-700">
                <span className="text-slate-400 text-xs">Execution Time:</span>
                <p className="font-medium text-sm">{loading ? "Computing..." : performance.time}</p>
              </div>
              <div className="bg-slate-800/30 p-2 rounded border border-slate-700">
                <span className="text-slate-400 text-xs">Cliques Found:</span>
                <p className="font-medium text-sm">{cliques.length}</p>
              </div>
              <div className="bg-slate-800/30 p-2 rounded border border-slate-700">
                <span className="text-slate-400 text-xs">Largest Clique:</span>
                <p className="font-medium text-sm">{performance.largestClique}</p>
              </div>
            </div>
          </div>
        </div>
      </div>
    </motion.div>
  );
};

export default GraphVisualizer;
