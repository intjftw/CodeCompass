#ifndef CC_SERVICE_GITHUB_GITHUBSSERVICE_H
#define CC_SERVICE_GITHUB_GITHUBSSERVICE_H

#include <memory>
#include <vector>

#include <boost/program_options/variables_map.hpp>

#include <odb/database.hxx>
#include <util/odbtransaction.h>
#include <webserver/servercontext.h>

#include <model/person.h>
#include <model/person-odb.hxx>
#include <model/pull.h>
#include <model/pull-odb.hxx>
#include <model/issue.h>
#include <model/issue-odb.hxx>

#include "GitHubService.h"

namespace cc
{
namespace service
{
namespace github
{

class GitHubServiceHandler : virtual public GitHubServiceIf
{
public:
  GitHubServiceHandler(
    std::shared_ptr<odb::database> db_,
    std::shared_ptr<std::string> datadir_,
    const cc::webserver::ServerContext& context_);

  void getContributorList(
    std::vector<Person>& return_) override;
  /*void getPullList(
    std::vector<model::Pull>& return_);
  void getIssueList(
    std::vector<model::Issue>& return_);*/

  void getGitHubString(std::string& str_);

private:
  std::shared_ptr<odb::database> _db;
  util::OdbTransaction _transaction;

  const boost::program_options::variables_map& _config;
};

} // github
} // service
} // cc

#endif // CC_SERVICE_GITHUB_GITHUBSSERVICE_H
