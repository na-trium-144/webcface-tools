# webcface-tools
[![release](https://img.shields.io/github/v/release/na-trium-144/webcface-tools)](https://github.com/na-trium-144/webcface-tools/releases)

[WebCFace](https://github.com/na-trium-144/webcface)のクライアントとなるプログラム群です。

WebCFaceのREADMEにしたがって、WebCFaceといっしょにインストールしてください。  
ソースからビルドする場合は
```sh
git submodule update --init
mkdir build
cd build
cmake ..
make
sudo make install
```
* `-DWEBCFACE_TOOLS_INSTALL_SERVICE`、`-DWEBCFACE_TOOLS_LAUNCHER_CONFIG_PATH`オプションでsystemdのserviceを lib/systemd/system にインストールできます

Ubuntu,MacOS,Windowsでビルドできるはずです

使い方はWebCFaceのドキュメントを参照してください
