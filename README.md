# webcface-tools
[![release](https://img.shields.io/github/v/release/na-trium-144/webcface-tools)](https://github.com/na-trium-144/webcface-tools/releases)

[WebCFace](https://github.com/na-trium-144/webcface)のクライアントとなるプログラム群です。  
それぞれ`-h`オプションでコマンドライン引数や使い方を確認できます。

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

Ubuntu,MacOSでビルドできることを確認済み ~~Windowsは知りません~~

# webcface-send

コマンドラインからwebcfaceに値を送信します。
```sh
webcface-send hoge
```
のように実行し、標準入力に値を入れると送信されます。

オプションで送信するデータの種類をvalue(デフォルト)、text、logから選べます。
別コマンドの出力をパイプで渡して、webcfaceで見るという使い方ができます

# webcface-launcher

設定ファイルにしたがってコマンドの実行、停止ができるviewを提供します。
tomlファイルに設定を記述し、
```sh
webcface-launcher ./webcface-launcher.toml
```
のように指定して起動します。

serverとlauncherだけを常時起動しておき、webcfaceでリモートに操作するという使い方ができます

tomlファイルの例
```toml
[init]
address = "127.0.0.1"
port = 7530
name = "my launcher"
 
[[command]]
name = "sleep"
exec = "sleep 1"
 
[[command]]
name = "main"
workdir = "/path/to/somewhere"
exec = "./main"
```
### init
* addressとport
	* サーバーのアドレスとポートです。デフォルトは127.0.0.1, 7530で、省略できます。
	* また、コマンドライン引数でも設定でき、その場合そちらが優先されます
* name
	* webcfaceでのこのlauncherの名前です。デフォルトは`webcface-launcher`で、省略できます。
	* コマンドライン引数でも設定できます。

### command
* name
	* webcfaceに表示される名前です
* workdir
	* コマンドを実行するディレクトリです。
	* 省略時カレントディレクトリになります
* exec
	* 実行するコマンドです
* stdout_capture
	* `"never"`, `"onerror"`(デフォルト), `"always"` が指定可能です
	* alwaysではコマンド終了時、onerrorではエラーで終了時にコマンドの標準出力とエラー出力の内容をlauncherのボタンの下に表示します
* stdout_utf8 (windowsのみ)
	* falseの場合(デフォルト)、stdout_captureで取得したデータはANSIエンコーディングとみなし、UTF-8に変換してからWebCFaceに送られます。
	* trueの場合、stdout_captureで取得したデータをUTF-8とみなし、そのままWebCFaceに送ります。

# webcface-cv-capture

OpenCVのVideoCaptureで画像を取得し、WebCFaceに送信します。

```sh
webcface-cv-capture 0
```
でVideoCapture(0)を取得します。
その他オプションで画像のサイズ、FPS、フォーマットを指定できます。
