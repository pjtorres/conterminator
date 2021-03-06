#include <cassert>

#include "Debug.h"
#include "Util.h"
#include "Parameters.h"
#include "FileUtil.h"
#include "CommandCaller.h"

#include "update_clustering.sh.h"

int clusterupdate(int argc, const char **argv, const Command& command) {
    Parameters& par = Parameters::getInstance();
    par.parseParameters(argc, argv, command, true, 0, 0);

    CommandCaller cmd;
    cmd.addVariable("REMOVE_TMP", par.removeTmpFiles ? "TRUE" : NULL);
    cmd.addVariable("RECOVER_DELETED", par.recoverDeleted ? "TRUE" : NULL);

    cmd.addVariable("RUNNER", par.runner.c_str());
    cmd.addVariable("DIFF_PAR", par.createParameterString(par.diff).c_str());
    cmd.addVariable("VERBOSITY", par.createParameterString(par.onlyverbosity).c_str());

    int maxAccept = par.maxAccept;
    par.maxAccept = 1;
    cmd.addVariable("SEARCH_PAR", par.createParameterString(par.clusterUpdateSearch).c_str());
    par.maxAccept = maxAccept;

    cmd.addVariable("CLUST_PAR", par.createParameterString(par.clusterworkflow).c_str());
    
    std::string tmpDir = par.db6;
    std::string hash = SSTR(par.hashParameter(par.filenames, par.clusterUpdate));
    if (par.reuseLatest) {
        hash = FileUtil::getHashFromSymLink(tmpDir + "/latest");
    }
    tmpDir = FileUtil::createTemporaryDirectory(tmpDir, hash);
    par.filenames.pop_back();
    par.filenames.push_back(tmpDir);

    std::string program = tmpDir + "/update_clustering.sh";
    FileUtil::writeFile(program, update_clustering_sh, update_clustering_sh_len);
	cmd.execProgram(program.c_str(), par.filenames);

    // Should never get here
    assert(false);
    return 0;
}
