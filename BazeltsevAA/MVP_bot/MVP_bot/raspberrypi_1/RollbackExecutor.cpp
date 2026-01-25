#include "RollbackExecutor.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <map>


struct LoggedMove {
    int direction; // 0-6: направление движения
    int speed;     // 10-100: скорость
    std::chrono::system_clock::time_point start;
    std::chrono::milliseconds duration;
};


static std::chrono::system_clock::time_point parseTime(const std::string &line) {
    int d, m, y, hh, mm, ss, ms;

    if (sscanf(
            line.c_str(),
            "[%d.%d.%d][%d:%d:%d.%d]",
            &d, &m, &y,
            &hh, &mm, &ss, &ms
        ) != 7) {
        return {};
    }

    std::tm tm{};
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = y - 1900;
    tm.tm_hour = hh;
    tm.tm_min = mm;
    tm.tm_sec = ss;

    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return tp + std::chrono::milliseconds(ms);
}

// Обратные команды
static char directionToInverseCommand(int direction) {
    static const std::map<int, char> dirToInvCmd = {
        {0, 'n'}, // STOP - STOP
        {1, 's'}, // FORWARD - BACKWARD
        {2, 'w'}, // BACKWARD - FORWARD
        {3, 'a'}, // RIGHT - LEFT
        {4, 'd'}, // LEFT - RIGHT
        {5, 'q'}, // TURN_RIGHT - TURN_LEFT
        {6, 'e'}  // TURN_LEFT - TURN_RIGHT
    };
    
    auto it = dirToInvCmd.find(direction);
    return (it != dirToInvCmd.end()) ? it->second : 'n';
}

RollbackExecutor::RollbackExecutor(CommandProcessor &c,
                                   const std::string &path,
                                   std::atomic<bool> &cancelFlag)
    : cmd(c), logPath(path), cancel(cancelFlag) {
}

void RollbackExecutor::executeRollback() {
    std::ifstream log(logPath);
    if (!log.is_open()) {
        std::cerr << "[RB] Failed to open log\n";
        return;
    }

    std::cout << "[RB] Parsing log...\n";

    std::vector<std::string> sessionLines;
    std::string line;

    while (std::getline(log, line)) {
        if (line.find("SYS:OPERATOR_CONNECTED") != std::string::npos) {
            sessionLines.clear(); 
        }
        sessionLines.push_back(line);
    }

    log.close();

    if (sessionLines.empty()) {
        std::cout << "[RB] Empty session\n";
        return;
    }

    // Восстановление движения из логов
    std::vector<LoggedMove> moves;

    bool hasActiveMove = false;
    int activeDirection = 0;
    std::chrono::system_clock::time_point activeStart;
    int currentSpeed = 50; // начальная скорость по умолчанию
    int activeSpeed = 50;

    for (const auto &l: sessionLines) {
        if (l.find("SPD:") != std::string::npos) {
            int from, to;
            if (sscanf(l.c_str(), "%*[^S]SPD:%d->%d", &from, &to) == 2) {
                currentSpeed = to;
                std::cout << "[RB] Speed changed to: " << currentSpeed << std::endl;
            }
            continue;
        }

        if (l.find("DIR:") != std::string::npos) {
            int from, to;
            if (sscanf(l.c_str(), "%*[^D]DIR:%d->%d", &from, &to) == 2) {
                auto ts = parseTime(l);
                
                if (hasActiveMove && activeDirection != 0) {
                    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
                        ts - activeStart
                    );

                    if (dur.count() > 0) {
                        moves.push_back({
                            activeDirection,
                            activeSpeed,
                            activeStart,
                            dur
                        });
                        std::cout << "[RB] Added move: dir=" << activeDirection 
                                  << " speed=" << activeSpeed 
                                  << " duration=" << dur.count() << "ms" << std::endl;
                    }
                }
                
                // Начало нового дыижения
                if (to != 0) {
                    activeDirection = to;
                    activeStart = ts;
                    activeSpeed = currentSpeed;
                    hasActiveMove = true;
                    std::cout << "[RB] Start move: dir=" << to << std::endl;
                } else {
                    hasActiveMove = false;
                }
            }
            continue;
        }

        if (l.find("SYS:") != std::string::npos) {
            continue;
        }
    }

    if (hasActiveMove && activeDirection != 0) {
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - activeStart
        );
        
        if (dur.count() > 0) {
            moves.push_back({
                activeDirection,
                activeSpeed,
                activeStart,
                dur
            });
            std::cout << "[RB] Added final move: dir=" << activeDirection 
                      << " duration=" << dur.count() << "ms" << std::endl;
        }
    }

    if (moves.empty()) {
        std::cout << "[RB] No movements to rollback\n";
        return;
    }

    std::cout << "[RB] Movements to rollback: " << moves.size() << std::endl;
    for (size_t i = 0; i < moves.size(); ++i) {
        char invCmd = directionToInverseCommand(moves[i].direction);
        std::cout << "  [" << i << "] dir=" << moves[i].direction 
                  << " inverse_cmd=" << invCmd
                  << " speed=" << moves[i].speed 
                  << " duration=" << moves[i].duration.count() << "ms" << std::endl;
    }

    cmd.setRollbackMode(true);

    int lastAppliedSpeed = -1;
    int executedCount = 0;

    // Движения в обратном порядке
    for (auto it = moves.rbegin(); it != moves.rend(); ++it) {
        if (cancel.load()) {
            std::cout << "[RB] Rollback cancelled\n";
            break;
        }

        char invCmd = directionToInverseCommand(it->direction);
        
        // Проверка поддержки обратной команды
        char checkInv;
        if (!cmd.inverseCommand(directionToInverseCommand(it->direction), checkInv)) {
            std::cerr << "[RB] No inverse for direction: " << it->direction 
                      << " (cmd would be: " << invCmd << ")" << std::endl;
            continue;
        }

        if (it->speed != lastAppliedSpeed) {
            std::cout << "[RB] Setting speed: " << it->speed << std::endl;
            cmd.setSpeedDirect(it->speed);
            lastAppliedSpeed = it->speed;
            
            usleep(100 * 1000); // 100 мс
        }

        std::cout << "[RB] Executing #" << (++executedCount) << ": " 
                  << "dir=" << it->direction << " -> inv_cmd=" << invCmd
                  << " for " << it->duration.count() << " ms"
                  << " at speed " << it->speed << std::endl;

        // Обратная команда
        cmd.processCommand(invCmd);

        int remaining = it->duration.count();
        while (remaining > 0 && !cancel.load()) {
            int chunk = (remaining > 100) ? 100 : remaining;
            usleep(chunk * 1000);
            remaining -= chunk;
        }
        
        if (cancel.load()) {
            std::cout << "[RB] Rollback cancelled during movement\n";
            cmd.processCommand('n');
            break;
        }
        
        // Останавка
        cmd.processCommand('n');
        
        if (!cancel.load()) {
            usleep(200 * 1000); // 200 мс
        }
    }

    cmd.setRollbackMode(false);
    std::cout << "[RB] Rollback finished. Executed " << executedCount << " movements.\n";
}
