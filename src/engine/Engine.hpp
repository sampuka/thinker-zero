#ifndef ENGINE_ENGINE_HPP
#define ENGINE_ENGINE_HPP

class Engine
{
public:
    Engine() = default;

    void shutdown();

    void main_loop();

private:
    bool engine_running = true;
};

inline Engine engine;

#endif // ENGINE_ENGINE_HPP
