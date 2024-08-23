#include <webcface/webcface.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <optional>
#include <array>
#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>
#include "../common/common.h"
#include "../common/cv2imgframe.h"

int main(int argc, char **argv) {
    CLI::App app{TOOLS_VERSION_DISP("WebCFace CV-Capture")};

    std::string wcli_host = "127.0.0.1", wcli_name = "webcface-cv-capture";
    int wcli_port = WEBCFACE_DEFAULT_PORT;
    app.add_option("-a,--address", wcli_host,
                   "Server address (default: 127.0.0.1)");
    app.add_option("-p,--port", wcli_port,
                   "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")");
    app.add_option("-m,--member", wcli_name, "Client member name");

    std::optional<int> fps;
    std::optional<std::array<int, 2>> size;
    app.add_option("-s,--size", size, "Width and height of image");
    app.add_option("-r,--frame-rate", fps, "Frame rate of image");
    std::string fourcc = "";
    app.add_option("-f,--fourcc", fourcc, "Fourcc code for VideoCapture");
    int index = 0;
    app.add_option("index", index,
                   "Device index for cv::VideoCapture (default: 0)");
    std::string field = "image";
    app.add_option("field", field, "Field name to send (default: image)");

    CLI11_PARSE(app, argc, argv);

    WebCFace::Client wcli(wcli_name, wcli_host, wcli_port);

    cv::VideoCapture cap(index);
    if (size) {
        cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, size->at(0));
        cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, size->at(1));
    }
    if (fps) {
        cap.set(cv::VideoCaptureProperties::CAP_PROP_FPS, *fps);
    }
    if (fourcc.size() >= 4) {
        cap.set(cv::VideoCaptureProperties::CAP_PROP_FOURCC,
                cv::VideoWriter::fourcc(fourcc[0], fourcc[1], fourcc[2],
                                        fourcc[3]));
    }

    if (cap.isOpened()) {
        wcli.start();
        while (true) {
            cv::Mat image;
            cap >> image;
            if (image.empty()) {
                break;
            }
            wcli.image(field).set(cv2ImgFrame(image));
            wcli.sync();
        }
        if (!wcli.connected()) {
            while (!wcli.connected()) {
                std::this_thread::yield();
            }
            wcli.sync();
        }
    } else {
        spdlog::error("Failed to open cv::VideoCapture({})", index);
    }
}
