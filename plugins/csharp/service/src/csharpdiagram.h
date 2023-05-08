#ifndef CC_SERVICE_LANGUAGE_CSHARPDIAGRAM_H
#define CC_SERVICE_LANGUAGE_CSHARPDIAGRAM_H

#include <service/csharpservice.h>
#include <projectservice/projectservice.h>
#include <util/graph.h>

namespace cc
{
namespace service
{
namespace language
{

class CsharpDiagram
{
public:
  CsharpDiagram(
    std::shared_ptr<odb::database> db_,
    std::shared_ptr<std::string> datadir_,
    const cc::webserver::ServerContext& context_);

  void getFunctionCallDiagram(
    util::Graph& graph_,
    const AstNodeInfo& centerNodeInfo_,
    const std::vector<AstNodeInfo>& calleeNodeInfos_,
    const std::vector<AstNodeInfo>& callerNodeInfos_);

  /**
   * This function creates legend for the Function call diagram.
   * @return The generated legend as a string in SVG format.
   */
  std::string getFunctionCallLegend();

  /**
   * This is a classical UML class diagram for the selected class and its
   * direct children and parents. The nodes contain the methods and member
   * variables with their visibility.
   */
  void getDetailedClassDiagram(
    util::Graph& graph_,
  const AstNodeInfo& centerNodeInfo_,
  const std::vector<AstNodeInfo>& propertyNodeInfos_,
  const std::vector<AstNodeInfo>& methodNodeInfos_);

  /**
   * This function creates legend for the Detailed class diagram.
   * @return The generated legend as a string in SVG format.
   */
  std::string getDetailedClassLegend();

private:
  typedef std::vector<std::pair<std::string, std::string>> Decoration;
  typedef std::pair<util::Graph::Node, util::Graph::Node> GraphNodePair;

  /**
   * This function adds a node which represents an AST node. The label of the
   * node is the AST node value. A node associated with the file is added only
   * once. If already added then the previous one is returned.
   */
  util::Graph::Node addNode(
    util::Graph& graph_,
    const AstNodeInfo& nodeInfo_);

  /**
   * This function adds a subgraph which represents a file. The label of the
   * subgraph will be the file path and the subgraph will have a border. A
   * subgraph associated with the file is added only once. If already added then
   * the previous one is returned.
   */
  util::Graph::Subgraph addSubgraph(
    util::Graph& graph_,
    const core::FileId& fileId_);

  /**
   * This function creates node label for UML class diagram for the
   * selected class.
   */
  std::string getDetailedClassNodeLabel(
    const AstNodeInfo& centerNodeInfo_,
    const std::vector<AstNodeInfo>& propertyNodeInfos_,
    const std::vector<AstNodeInfo>& methodNodeInfos_);

  /**
   * This function return string representation visibility of an AST node
   * in HTML format.
   */
  std::string visibilityToHtml(const AstNodeInfo& node_);

  /**
   * This function returns member content styled by their properties.
   * (E.g.: static -> underline, virtual -> italic etc.)
   */
  std::string memberContentToHtml(
    const AstNodeInfo& node_,
    const std::string& content_);

  /**
   * This function decorates a graph node.
   * @param graph_ A graph object.
   * @param elem_ A graph node
   * @param decoration_ A map which describes the style attributes.
   */
  void decorateNode(
    util::Graph& graph_,
    const util::Graph::Node& node_,
    const Decoration& decoration_) const;

  /**
   * This function decorates a graph edge.
   * @param graph_ A graph object.
   * @param elem_ A graph edge
   * @param decoration_ A map which describes the style attributes.
   */
  void decorateEdge(
    util::Graph& graph_,
    const util::Graph::Edge& edge_,
    const Decoration& decoration_) const;

  /**
   * This function decorates a graph subgraph.
   * @param graph_ A graph object.
   * @param elem_ A graph subgraph
   * @param decoration_ A map which describes the style attributes.
   */
  void decorateSubgraph(
    util::Graph& graph_,
    const util::Graph::Subgraph& subgrap_,
    const Decoration& decoration_) const;

  static const Decoration centerNodeDecoration;
  static const Decoration calleeNodeDecoration;
  static const Decoration callerNodeDecoration;
  static const Decoration virtualNodeDecoration;
  static const Decoration calleeEdgeDecoration;
  static const Decoration callerEdgeDecoration;
  static const Decoration classNodeDecoration;

  std::map<core::FileId, util::Graph::Subgraph> _subgraphs;

  std::shared_ptr<odb::database> _db;
  util::OdbTransaction _transaction;
  core::ProjectServiceHandler _projectHandler;
};

}
}
}

#endif
