#include <webcface/webcface.h>
#include <tclap/CmdLine.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd( // 全角24文字でtclapに勝手に改行されちゃう
            "WebCFaceサーバーにデータを送信できます。\n"
            "標準入力からデータを読み込み、改行するごとに\n"
            "送信されます。\n"
            "logデータの場合、levelはすべてinfoになります。",
            ' ', TOOLS_VERSION);
        TCLAP::ValueArg<std::string> hostArg(
            "a", "address", "Server address (default: 127.0.0.1)", false,
            "127.0.0.1", "address");
        cmd.add(hostArg);
        TCLAP::ValueArg<int> portArg(
            "p", "port", "Server port (default: " WEBCFACE_DEFAULT_PORT_S ")",
            false, WEBCFACE_DEFAULT_PORT, "number");
        cmd.add(portArg);
        TCLAP::ValueArg<std::string> nameArg("m", "member_name",
                                             "Client member name", false,
                                             "webcface-send", "string");
        cmd.add(nameArg);

        std::vector<std::string> dataTypes = {"value", "text", "log"};
        TCLAP::ValuesConstraint<std::string> dataTypesConstraint(dataTypes);
        TCLAP::ValueArg<std::string> dataTypeArg(
            "t", "type", "Message type (default: value)", false, "value",
            &dataTypesConstraint);
        cmd.add(dataTypeArg);

        TCLAP::UnlabeledValueArg<std::string> fieldArg(
            "field", "Field name to send", false, "data", "string");
        cmd.add(fieldArg);

        cmd.parse(argc, argv);

        WebCFace::Client wcli(nameArg.getValue(), hostArg.getValue(),
                              portArg.getValue());

        while (!std::cin.eof()) {
            if (dataTypeArg.getValue() == "value") {
                static double val;
                std::cin >> val;
                wcli.value(fieldArg.getValue()).set(val);
            } else if (dataTypeArg.getValue() == "text") {
                static std::string val;
                std::getline(std::cin, val);
                wcli.text(fieldArg.getValue()).set(val);
            } else if (dataTypeArg.getValue() == "log") {
                static std::string val;
                std::getline(std::cin, val);
                wcli.logger()->info(val);
            }
            wcli.sync();
        }
        if (!wcli.connected()) {
            while (!wcli.connected()) {
                std::this_thread::yield();
            }
            wcli.sync();
        }

        return 0;
    } catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId()
                  << std::endl;
        return 1;
    }

    // todo: オプションで変えられるようにする
}