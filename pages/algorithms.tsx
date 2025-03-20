import Layout from '../components/Layout';
import { useRouter } from 'next/router';
import fs from 'fs';
import path from 'path';
import CodeBlock from '../components/CodeBlock';
import { GetStaticProps } from 'next';

interface Algorithm {
  name: string;
  content: string;
  slug: string;
}

interface AlgorithmsProps {
  algorithms: Algorithm[];
}

export const getStaticProps: GetStaticProps = async () => {
  const algorithmsPath = path.join(process.cwd(), 'public/algorithms');
  const files = fs.readdirSync(algorithmsPath);
  
  const algorithms = files.map(filename => {
    const content = fs.readFileSync(path.join(algorithmsPath, filename), 'utf8');
    return {
      name: filename.replace('.cpp', ''),
      content,
      slug: filename.replace('.cpp', '')
    };
  });

  return { props: { algorithms } };
};

export default function Algorithms({ algorithms }: AlgorithmsProps) {
  const router = useRouter();
  const { slug } = router.query;

  return (
    <Layout>
      <div className="grid grid-cols-1 lg:grid-cols-4 gap-8">
        <div className="lg:col-span-1 space-y-2">
          <h1 className="text-2xl font-bold mb-4">Available Algorithms</h1>
          {algorithms.map(algorithm => (
            <button
              key={algorithm.slug}
              onClick={() => router.push(`/algorithms?slug=${algorithm.slug}`)}
              className={`w-full p-3 rounded text-left ${
                slug === algorithm.slug ? 'bg-indigo-600' : 'bg-slate-700 hover:bg-slate-600'
              }`}
            >
              {algorithm.name}
            </button>
          ))}
        </div>
        <div className="lg:col-span-3">
          {slug ? (
            <>
              <h2 className="text-2xl font-bold mb-4">
                {algorithms.find(a => a.slug === slug)?.name || 'Algorithm'}
              </h2>
              <CodeBlock
                language="cpp"
                value={algorithms.find(a => a.slug === slug)?.content || ''}
              />
            </>
          ) : (
            <div className="bg-slate-800 p-6 rounded-xl">
              <h2 className="text-xl font-bold mb-4">Select an algorithm</h2>
              <p className="text-slate-300">
                Please select an algorithm from the sidebar to view its implementation.
              </p>
            </div>
          )}
        </div>
      </div>
    </Layout>
  );
}
