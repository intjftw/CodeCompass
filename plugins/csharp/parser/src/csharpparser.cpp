#include <csharpparser/csharpparser.h>

#include <boost/filesystem.hpp>

#include <model/buildaction.h>
#include <model/buildaction-odb.hxx>
#include <model/buildsourcetarget.h>
#include <model/buildsourcetarget-odb.hxx>
#include <model/file.h>
#include <model/file-odb.hxx>

#include <parser/sourcemanager.h>

#include <util/logutil.h>
#include <util/odbtransaction.h>
#include <util/threadpool.h>

#include <memory>

namespace cc
{
namespace parser
{

CsharpParser::CsharpParser(ParserContext& ctx_): AbstractParser(ctx_)
{
  _threadNum = _ctx.options["jobs"].as<int>();
}

bool CsharpParser::acceptProjectBuildPath(const std::vector<std::string>& path_)
{
  return path_.size() >= 2 && fs::is_directory(path_[0]) && fs::is_directory(path_[1]);
}

bool CsharpParser::parse()
{        
  bool success = true;

  std::vector<std::string> paths = _ctx.options["input"].as<std::vector<std::string>>();
  
    if (acceptProjectBuildPath(paths))
    {
      LOG(debug) << "C# parser parse path: " << paths[0];
      LOG(debug) << "Parsed csharp project build path: " << paths[1];
      success = success && parseProjectBuildPath(paths);
    }
    else
    {
      LOG(error) << "Build path must be a directory!";
      success = false;
    }
  
  return success;
}

bool CsharpParser::parseProjectBuildPath(const std::vector<std::string>& paths_)
{
  namespace ch = std::chrono;
  fs::path csharp_path = fs::system_complete("../lib/csharp/");

  std::future<std::string> log;

  std::string command("./CSharpParser ");
  command.append("'");
  command.append(_ctx.options["database"].as<std::string>());
  command.append("' '");
  command.append(paths_[0]);
  command.append("' '");
  command.append(paths_[1]);
  command.append("' '");
  command.append(csharp_path.string());
  command.append("' ");
  command.append(std::to_string(_ctx.options["jobs"].as<int>()));
  LOG(debug) << "CSharpParser command: " << command;

  ch::steady_clock::time_point begin = ch::steady_clock::now();
  
  int result = bp::system(command, bp::start_dir(csharp_path), bp::std_out > log);

  ch::steady_clock::time_point current = ch::steady_clock::now();
  float elapsed_time = ch::duration_cast<ch::milliseconds>(current - begin).count();
  LOG(debug) << "CSharp Parse time: " << elapsed_time << " ms";

  std::string line;
  std::stringstream log_str(log.get());
  LOG(warning) << log_str.str();
  int countFull = 0, countPart = 0;
  
  while(std::getline(log_str, line, '\n'))
  {
    if (line[0] == '+' || line[0] == '-')
    {
      addSource(line.substr(1), line[0] == '-');
      if (line[0] == '+')
      {
        countFull++;
      }
      else
      {
        countPart++;
      }
    }
  }

  ch::steady_clock::time_point after = ch::steady_clock::now();
  elapsed_time =
    ch::duration_cast<ch::milliseconds>(after - current).count();

  LOG(debug) << "C# source manage time: " << elapsed_time << " ms";
  LOG(info) << "Number of files fully parsed: " << countFull << 
    ", partially parsed: " << countPart << ", total: " <<  countFull+countPart;

  return result == 0;
}

void CsharpParser::addSource(const std::string& filepath_, bool error_)
{
  util::OdbTransaction transaction(_ctx.db);

  model::BuildActionPtr buildAction(new model::BuildAction);
  buildAction->command = " ";
  buildAction->type = model::BuildAction::Compile;

  model::BuildSource buildSource;
  buildSource.file = _ctx.srcMgr.getFile(filepath_);
  buildSource.file->parseStatus = error_
    ? model::File::PSPartiallyParsed
    : model::File::PSFullyParsed;
  buildSource.file->type = "CS";
  buildSource.action = buildAction;

  _ctx.srcMgr.updateFile(*buildSource.file);
  _ctx.srcMgr.persistFiles();

  transaction([&, this] { 
    _ctx.db->persist(buildAction);
    _ctx.db->persist(buildSource);
  });
}


CsharpParser::~CsharpParser()
{
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern "C"
{
  boost::program_options::options_description getOptions()
  {
    boost::program_options::options_description description("C# Plugin");

    description.add_options()
        ("dummy-arg", po::value<std::string>()->default_value("Dummy arg"),
          "This argument will be used by the dummy parser.");

    return description;
  }

  std::shared_ptr<CsharpParser> make(ParserContext& ctx_)
  {
    return std::make_shared<CsharpParser>(ctx_);
  }
}
#pragma clang diagnostic pop

} // parser
} // cc
