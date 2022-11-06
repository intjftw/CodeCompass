#ifndef CC_PARSER_TEMPLATEANALYZER_H
#define CC_PARSER_TEMPLATEANALYZER_H

#include "yaml-cpp/yaml.h"

#include "model/file.h"

#include <model/helmtemplate.h>
#include <model/microservice.h>
#include <model/microservice-odb.hxx>
#include <model/microserviceedge.h>
#include <model/microserviceedge-odb.hxx>

#include <parser/parsercontext.h>

namespace cc
{
namespace parser
{

class TemplateAnalyzer
{
public:
  TemplateAnalyzer(
    ParserContext& ctx_,
    std::map<std::string, YAML::Node>& fileAstCache_);

  ~TemplateAnalyzer();

  void init();

private:
  bool visitKeyValuePairs(
    std::string path_,
    YAML::Node& currentFile_,
    model::Microservice& service_);

  void processServiceDeps(
    const std::string& path_,
    YAML::Node& currentFile_,
    model::Microservice& service_);
  void processMountDeps(
    const std::string& path_,
    YAML::Node& currentFile_,
    model::Microservice& service_);
  void processCertificateDeps(
    const std::string& path_,
    YAML::Node& currentFile_);
  //void processCertificateDeps(YAML::Node& currentFile_);

  void addEdge(
    const model::MicroserviceId& from_,
    const model::MicroserviceId& to_,
    std::string type_);

  void fillDependencyPairsMap();
  YAML::Node findKey(
    const std::string& key_,
    YAML::Node& currentFile_);

  std::map<std::string, model::HelmTemplate::DependencyType> _dependencyPairs;

  static std::unordered_set<model::MicroserviceEdgeId> _edgeCache;
  std::vector<model::MicroserviceEdgePtr> _newEdges;
  std::vector<model::HelmTemplate> _newTemplates;

  static std::vector<model::Microservice> _microserviceCache;
  model::Microservice _currentService;

  static std::mutex _edgeCacheMutex;

  ParserContext& _ctx;
  std::map<std::string, YAML::Node>& _fileAstCache;
};
}
}


#endif // CC_PARSER_TEMPLATEANALYZER_H
