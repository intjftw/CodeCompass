#ifndef CC_SERVICE_CSHARP_CSHARPSERVICE_H
#define CC_SERVICE_CSHARP_CSHARPSERVICE_H

#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <thrift/transport/TFDTransport.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/protocol/TBinaryProtocol.h>

#include <odb/database.hxx>
#include <util/odbtransaction.h>
#include <webserver/servercontext.h>

#include <LanguageService.h>
#include <CsharpService.h>

#include <util/odbtransaction.h>
#include <webserver/servercontext.h>

namespace cc
{
namespace service
{
namespace csharp
{

namespace language = cc::service::language;

using TransportException = apache::thrift::transport::TTransportException;

class CSharpQueryHandler : public CsharpServiceIf
{
public:
  CSharpQueryHandler() {}

  void getClientInterface(int timeoutInMs_)
  {
    using Transport = apache::thrift::transport::TTransport;
    using BufferedTransport = apache::thrift::transport::TBufferedTransport;
    using Socket = apache::thrift::transport::TSocket;
    using Protocol = apache::thrift::protocol::TBinaryProtocol;
    namespace ch = std::chrono;

    std::string host = "localhost";
    std::shared_ptr<Transport> socket(new Socket(host, _thriftServerPort));
    std::shared_ptr<Transport> transport(new BufferedTransport(socket));
    std::shared_ptr<Protocol> protocol(new Protocol(transport));

    _service.reset(new CsharpServiceClient(protocol));

    // Redirect Thrift output into std::stringstream
    apache::thrift::GlobalOutput.setOutputFunction(
      [](const char* x)
      {
        _thriftStream << x;
      });

    ch::steady_clock::time_point begin = ch::steady_clock::now();

    while (!transport->isOpen())
    {
      try
      {
        transport->open();
      }
      catch (TransportException& ex)
      {
        ch::steady_clock::time_point current = ch::steady_clock::now();
        float elapsed_time = ch::duration_cast<ch::milliseconds>(current - begin).count();

        if (elapsed_time > timeoutInMs_)
        {
          LOG(debug) << "Connection timeout, could not reach CSharp server on"
                     << host << ":" << _thriftServerPort;
          apache::thrift::GlobalOutput.setOutputFunction(
                  apache::thrift::TOutput::errorTimeWrapper);
          throw ex;
        }
      }
    }

    apache::thrift::GlobalOutput.setOutputFunction(
            apache::thrift::TOutput::errorTimeWrapper);

    LOG(info) << "C# server started!";
  }


  void getAstNodeInfo(
          language::AstNodeInfo& return_,
          const core::AstNodeId& astNodeId_) override
  {
    _service -> getAstNodeInfo(return_, astNodeId_);
  }

  void getAstNodeInfoByPosition(
          language::AstNodeInfo& return_,
          const std::string& path_,
          const core::Position& fpos_) override
  {
    _service -> getAstNodeInfoByPosition(return_, path_, fpos_);
  }

  void getFileRange(
          core::FileRange& return_,
          const core::AstNodeId& astNodeId_) override
  {
    _service -> getFileRange(return_, astNodeId_);
  }

  void getProperties(
          std::map<std::string, std::string>& return_,
          const core::AstNodeId& astNodeId_) override
  {
    _service -> getProperties(return_, astNodeId_);
  }

  void getDocumentation(
          std::string& return_,
          const core::AstNodeId& astNodeId_) override
  {
    _service -> getDocumentation(return_, astNodeId_);
  }

  void getReferenceTypes(
          std::map<std::string, std::int32_t>& return_,
          const core::AstNodeId& astNodeId_) override
  {
    _service -> getReferenceTypes(return_, astNodeId_);
  }

  std::int32_t getReferenceCount(
          const core::AstNodeId& astNodeId_,
          const std::int32_t referenceId_) override
  {
    return _service -> getReferenceCount(astNodeId_, referenceId_);
  }

  void getReferences(
          std::vector<language::AstNodeInfo>& return_,
          const core::AstNodeId& astNodeId_,
          const std::int32_t referenceId_,
          const std::vector<std::string>& tags_) override
  {
    _service -> getReferences(return_, astNodeId_, referenceId_, tags_);
  }

  void getFileReferenceTypes(
          std::map<std::string, std::int32_t>& return_) override
  {
    _service -> getFileReferenceTypes(return_);
  }

  std::int32_t getFileReferenceCount(
          const core::FileId& fileId_,
          const std::int32_t referenceId_) override
  {
    return _service -> getFileReferenceCount(fileId_, referenceId_);
  }

  void getFileReferences(
          std::vector<language::AstNodeInfo>& return_,
          const core::FileId& fileId_,
          const std::int32_t referenceId_) override
  {
    _service -> getFileReferences(return_, fileId_, referenceId_);
  }

  void getDiagramTypes(
          std::map<std::string, std::int32_t>& return_,
          const core::AstNodeId& astNodeId_) override
  {
    _service -> getDiagramTypes(return_, astNodeId_);
  }

  void getDiagram(
          std::string& return_,
          const core::AstNodeId& astNodeId_,
          const std::int32_t diagramId_) override
  {
    _service -> getDiagram(return_, astNodeId_, diagramId_);
  }

  void getSyntaxHighlight(
          std::vector<language::SyntaxHighlight>& return_,
          const core::FileRange& range_,
          const std::vector<std::string>& content_) override
  {
    _service -> getSyntaxHighlight(return_, range_, content_);
  }

  void setThriftServerPort(int port)
  {
    _thriftServerPort = port;
  }

  void getFileDiagramTypes(
          std::map<std::string, std::int32_t>& return_,
          const core::FileId& fileId_) override
  {
    _service -> getFileDiagramTypes(return_, fileId_);
  }

  void getFileDiagram(
          std::string& return_,
          const core::FileId& fileId_,
          const std::int32_t diagramId_) override
  {
    _service -> getFileDiagram(return_, fileId_, diagramId_);
  }

private:
    std::unique_ptr<CsharpServiceIf> _service;
    static std::stringstream _thriftStream;
    static int _thriftServerPort;
};
}

namespace language
{

using TransportException = apache::thrift::transport::TTransportException;

class CsharpServiceHandler : virtual public LanguageServiceIf
{
  friend class Diagram;

public:
  CsharpServiceHandler(
    std::shared_ptr<odb::database> db_,
    std::shared_ptr<std::string> datadir_,
    const cc::webserver::ServerContext& context_);  

  std::string getDbString();
  
  void getFileTypes(std::vector<std::string>& return_) override;

  void getAstNodeInfo(
    AstNodeInfo& return_,
    const core::AstNodeId& astNodeId_) override;

  void getAstNodeInfoByPosition(
    AstNodeInfo& return_,
    const core::FilePosition& fpos_) override;

  void getSourceText(
    std::string& return_,
    const core::AstNodeId& astNodeId_) override;

  void getDocumentation(
    std::string& return_,
    const core::AstNodeId& astNodeId_) override;

  void getProperties(
    std::map<std::string, std::string>& return_,
    const core::AstNodeId& astNodeId_) override;

  void getDiagramTypes(
    std::map<std::string, std::int32_t>& return_,
    const core::AstNodeId& astNodeId_) override;

  void getDiagram(
    std::string& return_,
    const core::AstNodeId& astNodeId_,
    const std::int32_t diagramId_) override;

  void getDiagramLegend(
    std::string& return_,
    const std::int32_t diagramId_) override;

  void getFileDiagramTypes(
    std::map<std::string, std::int32_t>& return_,
    const core::FileId& fileId_) override;

  void getFileDiagram(
    std::string& return_,
    const core::FileId& fileId_,
    const int32_t diagramId_) override;

  void getFileDiagramLegend(
    std::string& return_,
    const std::int32_t diagramId_) override;

  void getReferenceTypes(
    std::map<std::string, std::int32_t>& return_,
    const core::AstNodeId& astNodeId) override;

  void getReferences(
    std::vector<AstNodeInfo>& return_,
    const core::AstNodeId& astNodeId_,
    const std::int32_t referenceId_,
    const std::vector<std::string>& tags_) override;

  std::int32_t getReferenceCount(
    const core::AstNodeId& astNodeId_,
    const std::int32_t referenceId_) override;

  void getReferencesInFile(
    std::vector<AstNodeInfo>& return_,
    const core::AstNodeId& astNodeId_,
    const std::int32_t referenceId_,
    const core::FileId& fileId_,
    const std::vector<std::string>& tags_) override;

  void getReferencesPage(
    std::vector<AstNodeInfo>& return_,
    const core::AstNodeId& astNodeId_,
    const std::int32_t referenceId_,
    const std::int32_t pageSize_,
    const std::int32_t pageNo_) override;

  void getFileReferenceTypes(
    std::map<std::string, std::int32_t>& return_,
    const core::FileId& fileId_) override;

  void getFileReferences(
    std::vector<AstNodeInfo>& return_,
    const core::FileId& fileId_,
    const std::int32_t referenceId_) override;

  std::int32_t getFileReferenceCount(
    const core::FileId& fileId_,
    const std::int32_t referenceId_) override;

  void getSyntaxHighlight(
    std::vector<SyntaxHighlight>& return_,
    const core::FileRange& range_) override;

private:
  std::shared_ptr<odb::database> _db;
  util::OdbTransaction _transaction;

  std::shared_ptr<std::string> _datadir;
  const cc::webserver::ServerContext& _context;
  boost::process::child c;

  cc::service::csharp::CSharpQueryHandler _csharpQueryHandler;
  static int _thriftServerPort;
};

} // language
} // service
} // cc

#endif // CC_SERVICE_CSHARP_CSHARPSERVICE_H
