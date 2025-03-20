import { useState } from 'react';
import Head from 'next/head';
import Link from 'next/link';
import { motion } from 'framer-motion';

export default function Layout({ children, title = "Maximal Clique Algorithms" }) {
  const [isMenuOpen, setIsMenuOpen] = useState(false);
  
  return (
    <div className="min-h-screen bg-slate-900 text-white">
      <Head>
        <title>{title} | DAA Assignment</title>
        <meta name="description" content="Design and Analysis of Algorithms Assignment" />
        <link rel="icon" href="/favicon.ico" />
      </Head>
      
      <header className="bg-slate-800 border-b border-slate-700">
        <div className="container mx-auto px-4 py-4">
          <div className="flex justify-between items-center">
            
            {/* Desktop Navigation */}
            <nav className="hidden md:flex space-x-6">
              <Link href="/">
                <span className="hover:text-indigo-400 transition-colors">Home </span>
              </Link>
              <Link href="/algorithms">
                <span className="hover:text-indigo-400 transition-colors">Algorithms </span>
              </Link>
              <Link href="/docs/about">
                <span className="hover:text-indigo-400 transition-colors">Documentation</span>
              </Link>
            </nav>
            
          </div>
          
          {/* Mobile Navigation */}
          {isMenuOpen && (
            <motion.div
              initial={{ opacity: 0, height: 0 }}
              animate={{ opacity: 1, height: 'auto' }}
              exit={{ opacity: 0, height: 0 }}
              className="md:hidden mt-4 border-t border-slate-700 pt-4"
            >
              <nav className="flex flex-col space-y-3">
                <Link href="/">
                  <span className="px-2 py-1 rounded hover:bg-slate-700 block transition-colors">Home</span>
                </Link>
                <Link href="/algorithms">
                  <span className="px-2 py-1 rounded hover:bg-slate-700 block transition-colors">Algorithms</span>
                </Link>
                <Link href="/docs/about">
                  <span className="px-2 py-1 rounded hover:bg-slate-700 block transition-colors">Documentation</span>
                </Link>
              </nav>
            </motion.div>
          )}
        </div>
      </header>
      
      <main className="container mx-auto px-4 py-8">
        {children}
      </main>
      
      <footer className="bg-slate-800 border-t border-slate-700 py-6">
        <div className="container mx-auto px-4 text-center">
          <p className="text-slate-400">Design and Analysis of Algorithms Assignment &copy; {new Date().getFullYear()}</p>
        </div>
      </footer>
    </div>
  );
}
