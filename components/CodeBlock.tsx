import { Prism as SyntaxHighlighter } from 'react-syntax-highlighter';
import { atomDark } from 'react-syntax-highlighter/dist/cjs/styles/prism';

interface CodeBlockProps {
  language: string;
  value: string;
}

export default function CodeBlock({ language, value }: CodeBlockProps) {
  return (
    <SyntaxHighlighter
      language={language}
      style={atomDark}
      customStyle={{
        borderRadius: '0.5rem',
        padding: '1.5rem',
        margin: '1rem 0'
      }}
    >
      {value}
    </SyntaxHighlighter>
  );
}
