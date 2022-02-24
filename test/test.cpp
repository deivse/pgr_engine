#include <spdlog/spdlog.h>

#include <npgr.h>
#include <shader.h>
#include <layers.h>




int main() {
    try {
        npgr::app_t app(100, 100, "tesst");
        app.main_loop();
    } catch (std::runtime_error& e) {
        spdlog::error(e.what());
        return 1;
    }
    return 0;
}
