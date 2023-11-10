#include <webcface/webcface.h>
#include <mutex>
#include <vector>
#include <variant>
#include <memory>

inline std::shared_ptr<WebCFace::Client> wcli;
inline std::vector<std::variant<WebCFace::Value, WebCFace::Text>> data;
inline std::mutex mtx;
void init_display();
void update_display();
void loop_display();
void exit_display();