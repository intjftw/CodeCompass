#include <githubparser/githubparser.h>

#include <boost/filesystem.hpp>

#include <util/logutil.h>

#include <memory>

namespace cc
{
namespace parser
{

GithubParser::GithubParser(ParserContext& ctx_): AbstractParser(ctx_)
{
}

bool GithubParser::accept(const std::string& path_)
{
  std::string ext = boost::filesystem::extension(path_);
  return ext == ".github";
}

bool GithubParser::parse()
{        
  for(std::string path : _ctx.options["input"].as<std::vector<std::string>>())
  {
    if(accept(path))
    {
      LOG(info) << "GithubParser parse path: " << path;
    }
  }
  return true;
}

GithubParser::~GithubParser()
{
}

/* These two methods are used by the plugin manager to allow dynamic loading
   of CodeCompass Parser plugins. Clang (>= version 6.0) gives a warning that
   these C-linkage specified methods return types that are not proper from a
   C code.

   These codes are NOT to be called from any C code. The C linkage is used to
   turn off the name mangling so that the dynamic loader can easily find the
   symbol table needed to set the plugin up.
*/
// When writing a plugin, please do NOT copy this notice to your code.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern "C"
{
  boost::program_options::options_description getOptions()
  {
    boost::program_options::options_description description("Github Plugin");

    description.add_options()
        ("github-arg", po::value<std::string>()->default_value("Github arg"),
          "This argument will be used by the github parser.");

    return description;
  }

  std::shared_ptr<GithubParser> make(ParserContext& ctx_)
  {
    return std::make_shared<GithubParser>(ctx_);
  }
}
#pragma clang diagnostic pop

} // parser
} // cc
