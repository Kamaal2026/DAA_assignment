import { useState, useEffect } from 'react';
import { motion } from 'framer-motion';

interface DatasetGeneratorProps {
  onDatasetChange?: (nodes: number, edges: [number, number][]) => void;
}

export default function DatasetGenerator({ onDatasetChange }: DatasetGeneratorProps) {
  const [nodes, setNodes] = useState<number>(4);
  const [edges, setEdges] = useState<[number, number][]>([[1, 2], [2, 3], [3, 4], [4, 1]]);
  const [newEdge, setNewEdge] = useState<[number, number]>([1, 2]);
  const [dataset, setDataset] = useState<string>("");
  const [copied, setCopied] = useState<boolean>(false);

  useEffect(() => {
    generateDataset();
    onDatasetChange?.(nodes, edges);
  }, [nodes, edges, onDatasetChange]);

  const generateDataset = () => {
    const data = `${nodes} ${edges.length}\n${edges.map(edge => `${edge[0]} ${edge[1]}`).join('\n')}`;
    setDataset(data);
  };

  const addEdge = () => {
    if (newEdge[0] < 1 || newEdge[0] > nodes || newEdge[1] < 1 || newEdge[1] > nodes) {
      alert(`Node values must be between 1 and ${nodes}`);
      return;
    }
    if (edges.some(edge => 
      (edge[0] === newEdge[0] && edge[1] === newEdge[1]) || 
      (edge[0] === newEdge[1] && edge[1] === newEdge[0])
    )) {
      alert("Edge already exists!");
      return;
    }
    setEdges([...edges, [newEdge[0], newEdge[1]]]);
  };

  const removeEdge = (index: number) => {
    const newEdges = [...edges];
    newEdges.splice(index, 1);
    setEdges(newEdges);
  };

  const copyToClipboard = () => {
    navigator.clipboard.writeText(dataset);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      className="bg-slate-800/50 p-6 rounded-xl border border-slate-700 shadow-lg backdrop-blur-sm"
    >
      <h3 className="text-lg font-semibold mb-4 text-white">Dataset Generator</h3>
      <div className="grid grid-cols-1 md:grid-cols-2 gap-5">
        <div>
          <div className="mb-4">
            <label className="block mb-2 text-slate-300 text-sm font-medium">Number of Nodes</label>
            <input
              type="number"
              value={nodes}
              onChange={(e) => setNodes(Math.max(1, parseInt(e.target.value || "1")))}
              className="w-full bg-slate-700 rounded-md p-2 text-sm text-white border border-slate-600 focus:border-indigo-500 focus:ring focus:ring-indigo-500/20 transition-all"
              min="1"
            />
          </div>
          <div className="mt-4">
            <label className="block mb-2 text-slate-300 text-sm font-medium">Current Edges</label>
            <div className="max-h-40 overflow-y-auto bg-slate-900/50 p-2 rounded-md border border-slate-700">
              {edges.map((edge, index) => (
                <div key={index} className="flex justify-between items-center mb-2 last:mb-0">
                  <span className="text-slate-300 text-sm">{`${edge[0]}-${edge[1]}`}</span>
                  <button
                    onClick={() => removeEdge(index)}
                    className="text-red-400 hover:text-red-300 bg-red-900/20 hover:bg-red-900/30 px-2 py-1 rounded-md text-xs"
                  >
                    Remove
                  </button>
                </div>
              ))}
            </div>
          </div>
        </div>
        <div>
          <div className="mb-4">
            <label className="block mb-2 text-slate-300 text-sm font-medium">Add New Edge</label>
            <div className="flex gap-2 mb-4">
              <input
                type="number"
                value={newEdge[0]}
                onChange={(e) => setNewEdge([parseInt(e.target.value || "1"), newEdge[1]])}
                className="w-20 bg-slate-700 rounded-md p-2 text-sm text-white border border-slate-600 focus:border-indigo-500 focus:ring focus:ring-indigo-500/20"
                min="1"
                max={nodes}
              />
              <span className="text-slate-400 self-center">-</span>
              <input
                type="number"
                value={newEdge[1]}
                onChange={(e) => setNewEdge([newEdge[0], parseInt(e.target.value || "1")])}
                className="w-20 bg-slate-700 rounded-md p-2 text-sm text-white border border-slate-600 focus:border-indigo-500 focus:ring focus:ring-indigo-500/20"
                min="1"
                max={nodes}
              />
              <button
                onClick={addEdge}
                className="bg-indigo-600/80 hover:bg-indigo-700 px-3 py-1.5 rounded-md text-white text-sm font-medium flex items-center gap-1"
              >
                <svg
                  xmlns="http://www.w3.org/2000/svg"
                  className="h-4 w-4"
                  viewBox="0 0 24 24"
                  stroke="currentColor"
                  fill="none"
                  strokeWidth="2"
                >
                  <path strokeLinecap="round" strokeLinejoin="round" d="M12 4v16m8-8H4" />
                </svg>
                Add
              </button>
            </div>
          </div>
          <div className="mt-4">
            <label className="block mb-2 text-slate-300 text-sm font-medium">Generated Dataset</label>
            <div className="mt-2 p-3 bg-slate-900/50 rounded-md border border-slate-700">
              <pre className="text-xs whitespace-pre-wrap text-slate-300">{dataset}</pre>
            </div>
            <button
              onClick={copyToClipboard}
              className={`mt-2 w-full px-3 py-1.5 rounded-md text-sm font-medium flex items-center justify-center gap-1.5 ${
                copied ? 'bg-green-600/90' : 'bg-indigo-600/80 hover:bg-indigo-700/90'
              }`}
            >
            </button>
          </div>
        </div>
      </div>
    </motion.div>
  );
}
