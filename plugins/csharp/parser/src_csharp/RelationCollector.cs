using System;
using System.Collections.Generic;
using System.Linq;
using static System.Console;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.FindSymbols;
using CSharpParser.model;
using Microsoft.CodeAnalysis;
using System.Threading.Tasks;

namespace CSharpParser
{
    partial class RelationCollector : CSharpSyntaxWalker
    {
        private readonly CsharpDbContext DbContext;
        private readonly SemanticModel Model;
        private readonly SyntaxTree Tree;

        public RelationCollector(CsharpDbContext context, SemanticModel model, SyntaxTree tree)
        {
            this.DbContext = context;
            this.Model = model;
            this.Tree = tree;  
            //this.solution = Solution ???          
        }    

        public override void VisitVariableDeclarator(VariableDeclaratorSyntax node)
        {
            if (node.Initializer != null)
            {
                var symbol = Model.GetDeclaredSymbol(node);
                var references = symbol.FindReferences(/*Model.Compilation*/);
                //var solution = _semanticModel.Compilation.SyntaxTrees.First().Options.SyntaxTree.GetRoot().SyntaxTree.Options
                //        .Project.Solution;
                //var references = await SymbolFinder.FindReferencesAsync(symbol, solution);

                foreach (var reference in references)
                {
                    var referenceNode = reference.Definition.DeclaringSyntaxReferences[0].GetSyntax();
                    if (referenceNode != node)
                    {
                        var referenceSymbol = Model.GetSymbolInfo(referenceNode).Symbol;

                        var declaringNode = node.Parent.Parent; // The parent of the variable declarator is the variable declaration syntax, whose parent is the member declaration syntax.
                        var declaringFile = declaringNode.SyntaxTree.FilePath;

                        var referenceFile = referenceNode.SyntaxTree.FilePath;

                        WriteLine($"Value declaringFile: {declaringFile}; referenceFile: {referenceFile}");

                        CsharpEdge csharpEdge = new CsharpEdge();
                        csharpEdge.From = fnvHash(declaringFile);
                        csharpEdge.To = fnvHash(referenceFile);
                        csharpEdge.Type = EdgeType.USE;
                        csharpEdge.Id = createIdentifier(csharpEdge);
                        DbContext.CsharpEdges.Add(csharpEdge);
                    }
                }
            }

            base.VisitVariableDeclarator(node);
        }
        

        /*
        public override void VisitVariableDeclarator(VariableDeclaratorSyntax node)
        {
            var symbol = Model.GetDeclaredSymbol(node);

            var solution = 

            ProcessReferencesAsync(node, symbol, solution).Wait();

            base.VisitVariableDeclarator(node);
        }

        private async Task ProcessReferencesAsync(VariableDeclaratorSyntax node, ISymbol symbol, Solution solution)
        {
            var references = await SymbolFinder.FindReferencesAsync(symbol, solution);

            foreach (var reference in references)
            {
                var referenceNode = reference.Definition.DeclaringSyntaxReferences[0].GetSyntax();
                if (referenceNode != node)
                {
                    var referenceSymbol = Model.GetSymbolInfo(referenceNode).Symbol;

                    var declaringNode = node.Parent.Parent; // The parent of the variable declarator is the variable declaration syntax, whose parent is the member declaration syntax.
                    var declaringFile = declaringNode.SyntaxTree.FilePath;

                    var referenceFile = referenceNode.SyntaxTree.FilePath;

                    WriteLine($"Value declaringFile: {declaringFile}; referenceFile: {referenceFile}");

                    CsharpEdge csharpEdge = new CsharpEdge();
                    csharpEdge.From = fnvHash(declaringFile);
                    csharpEdge.To = fnvHash(referenceFile);
                    csharpEdge.Type = EdgeType.USE;
                    csharpEdge.Id = createIdentifier(csharpEdge);
                    DbContext.CsharpEdges.Add(csharpEdge);
                }
            }
        }
        */


        public override void VisitInvocationExpression(InvocationExpressionSyntax node)
        {
            var symbol = Model.GetSymbolInfo(node).Symbol;

            if (symbol != null && symbol.Kind == SymbolKind.Method)
            {
                var methodSymbol = (IMethodSymbol)symbol;

                var containingType = methodSymbol.ContainingType;
                if (containingType != null && containingType.Name != "<global namespace>")
                {
                    var declaringFile = node.SyntaxTree.FilePath;

                    string referenceFile = null;
                    if (containingType.Locations.Length > 0)
                    {
                        referenceFile = containingType.Locations[0].SourceTree.FilePath;
                    }
                    WriteLine($"Method declaringFile: {declaringFile}; referenceFile: {referenceFile}");

                    if (referenceFile != null)
                    {
                        CsharpEdge csharpEdge = new CsharpEdge();
                        csharpEdge.From = fnvHash(declaringFile);
                        csharpEdge.To = fnvHash(referenceFile);
                        csharpEdge.Type = EdgeType.USE;
                        csharpEdge.Id = createIdentifier(csharpEdge);
                        DbContext.CsharpEdges.Add(csharpEdge);
                    }
                }
            }

            base.VisitInvocationExpression(node);
        }

        private ulong createIdentifier(CsharpEdge edge_)
        {
            return fnvHash($"{edge_.From}{edge_.To}{typeToString(edge_.Type)}");
        }

        private ulong fnvHash(string data_)
        {
            ulong hash = 14695981039346656037;

            int len = data_.Length;
            for (int i = 0; i < len; ++i)
            {
                hash ^= data_[i];
                hash *= 1099511628211;
            }

            return hash;
        }

        private string typeToString(EdgeType type_)
        {
            switch (type_)
            {
                case EdgeType.PROVIDE: return "Provide";
                case EdgeType.IMPLEMENT: return "Implement";
                case EdgeType.USE: return "Use";
                case EdgeType.DEPEND: return "Depend";
            }

            return string.Empty;
        }     
    }
}