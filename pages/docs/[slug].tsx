import Layout from '../../components/Layout';
import fs from 'fs';
import path from 'path';
import matter from 'gray-matter';
import ReactMarkdown from 'react-markdown';
import CodeBlock from '../../components/CodeBlock';
import { GetStaticPaths, GetStaticProps } from 'next';

interface DocPageProps {
  frontmatter: {
    title: string;
  };
  content: string;
}

export const getStaticPaths: GetStaticPaths = async () => {
  const docsPath = path.join(process.cwd(), 'pages/docs');
  const files = fs.readdirSync(docsPath);
  const paths = files
    .filter(filename => filename.endsWith('.md'))
    .map(filename => ({
      params: { slug: filename.replace('.md', '') }
    }));

  return { paths, fallback: false };
};

export const getStaticProps: GetStaticProps<DocPageProps> = async ({ params }) => {
  try {
    const slug = params?.slug;
    if (!slug || typeof slug !== 'string') {
      return { notFound: true };
    }

    const filePath = path.join(process.cwd(), 'pages/docs', `${slug}.md`);
    const fileContent = fs.readFileSync(filePath, 'utf8');
    const { data: frontmatter, content } = matter(fileContent);

    if (!frontmatter?.title) {
      return { notFound: true };
    }

    return {
      props: {
        frontmatter: {
          title: frontmatter.title.toString()
        },
        content
      }
    };
  } catch (error) {
    return { notFound: true };
  }
};

export default function DocPage({ frontmatter, content }: DocPageProps) {
  return (
    <Layout>
      <article className="prose prose-invert max-w-none">
        <h1 className="text-4xl font-bold mb-8">{frontmatter.title}</h1>
        <ReactMarkdown
          components={{
            code({ inline, className, children, ...props }) {
              const match = /language-(\w+)/.exec(className || '');
              return !inline && match ? (
                <CodeBlock
                  language={match[1]}
                  value={String(children).replace(/\n$/, '')}
                  {...props}
                />
              ) : (
                <code className={className} {...props}>
                  {children}
                </code>
              );
            }
          }}
        >
          {content}
        </ReactMarkdown>
      </article>
    </Layout>
  );
}
