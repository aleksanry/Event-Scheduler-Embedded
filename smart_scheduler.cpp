#include <stdint.h>
#include <functional>
#include <vector>
#include <iostream> // For simulation/debugging
#include <chrono>
#include <thread>

// -----------------------------
// Task Class Definition
// -----------------------------
class Task {
public:
    std::function<void()> callback;
    uint32_t interval_ms;
    uint32_t last_run = 0;

    Task(std::function<void()> cb, uint32_t interval)
        : callback(cb), interval_ms(interval) {}
};

// -----------------------------
// Scheduler Class Definition
// -----------------------------
class Scheduler {
private:
    std::vector<Task> tasks;
    uint32_t (*millis)();

public:
    Scheduler(uint32_t (*getMillis)()) : millis(getMillis) {}

    void addTask(std::function<void()> cb, uint32_t interval) {
        tasks.emplace_back(cb, interval);
    }

    void run() {
        uint32_t now = millis();
        for (auto& task : tasks) {
            if (now - task.last_run >= task.interval_ms) {
                task.callback();
                task.last_run = now;
            }
        }
        enterLowPowerMode();
    }

    void enterLowPowerMode() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // Replace with __WFI() or actual sleep call on real hardware
    }
};

// -----------------------------
// Simulation of millis() for PC
// -----------------------------
uint32_t mockMillis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

// -----------------------------
// Example Tasks
// -----------------------------
void setupSensor() {
    std::cout << "Sensor initialized!\n";
}

void readSensor() {
    std::cout << "Sensor reading at " << mockMillis() << " ms\n";
}

// -----------------------------
// Main Function
// -----------------------------
int main() {
    Scheduler scheduler(mockMillis);

    scheduler.addTask(setupSensor, 0);          
    scheduler.addTask(readSensor, 1000);        

    while (true) {
        scheduler.run();
    }

    return 0;
}
