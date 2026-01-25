#ifndef ROLLBACKEXECUTOR_H
#define ROLLBACKEXECUTOR_H

#include "CommandProcessor.h"
#include <string>
#include <atomic>

class RollbackExecutor {
public:
    RollbackExecutor(CommandProcessor &c,
                     const std::string &path,
                     std::atomic<bool> &cancelFlag);
    
    void executeRollback();

private:
    CommandProcessor &cmd;
    std::string logPath;
    std::atomic<bool> &cancel;
};

#endif
