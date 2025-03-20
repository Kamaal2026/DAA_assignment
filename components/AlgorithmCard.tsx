import { motion } from 'framer-motion';

interface Algorithm {
  name: string;
  description: string;
  complexity: string;
  path: string;
}

interface AlgorithmCardProps {
  algorithm: Algorithm;
  delay: number;
  isActive?: boolean;
}

export default function AlgorithmCard({ algorithm, delay, isActive = false }: AlgorithmCardProps) {
  return (
    <motion.div
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ delay }}
      className={`${
        isActive ? 'bg-indigo-800/30 border-indigo-600' : 'bg-slate-800/50 border-slate-700'
      } p-5 rounded-xl hover:bg-slate-800/40 transition-all cursor-pointer border backdrop-blur-sm`}
    >
      <h3 className="text-lg font-semibold mb-2 text-indigo-100">{algorithm.name}</h3>
      <p className="text-slate-300 text-sm mb-4 leading-relaxed">{algorithm.description}</p>
      <div className="flex justify-between items-center">
        <span className="text-xs bg-indigo-600/30 px-2.5 py-1 rounded-full font-medium">
          {algorithm.complexity}
        </span>
        <a
          href={algorithm.path}
          download
          className="text-indigo-400 hover:text-indigo-300 hover:bg-indigo-900/30 transition-colors px-2 py-1 rounded-md flex items-center text-sm gap-1.5 border border-indigo-600/50"
          onClick={(e) => e.stopPropagation()}
        >
          Source Code
        </a>
      </div>
    </motion.div>
  );
}
