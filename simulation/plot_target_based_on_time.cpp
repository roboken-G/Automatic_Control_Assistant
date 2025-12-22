#include <iostream>
#include <fstream>
#include <vector>
#include "target_based_on_time.hpp"

// GTestではないので main 関数が必要
int main() {
    std::string filename = "build/simulation_data.csv";
    std::cout << "[SIMULATION] Generating profile data to " << filename << "..." << std::endl;

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    file << "time,position,velocity,acceleration\n";

    aca::TargetBasedOnTime planner;
    aca::MotionProfile profile;
    
    // --- パラメータ設定 ---
    profile.maxAcceleration = 500.0f; // 500 mm/s^2
    profile.maxVelocity = 1000.0f;    // 1000 mm/s
    float target = 2000.0f;           // 2000 mm 移動
    // ---------------------

    planner.set(target, 0.0f, profile);

    float totalTime = planner.getTimeRequired();
    uint32_t endTimeMs = static_cast<uint32_t>(totalTime * 1000) + 500; // 0.5秒余分に

    // 10ms刻みでシミュレーション
    for (uint32_t t = 0; t <= endTimeMs; t += 10) {
        auto status = planner.update(t);
        file << t / 1000.0f << "," // 秒
             << status.position << ","
             << status.velocity << ","
             << status.acceleration << "\n";
    }
    
    std::cout << "[DONE] Generation complete." << std::endl;
    return 0;
}