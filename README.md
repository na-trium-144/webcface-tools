# webcface-tools

[WebCFace](https://github.com/na-trium-144/webcface)のクライアントとなるプログラム群です。

それぞれ`-h`オプションでコマンドライン引数や使い方を確認できます。

# webcface-send

コマンドラインからwebcfaceに値を送信します。
```sh
webcface-send hoge
```
のように実行し、標準入力に値を入れると送信されます。

# webcface-launcher

設定ファイルにしたがってコマンドの実行、停止ができる画面を提供します。
tomlファイルに設定を記述し、
```sh
webcface-launcher ./webcface-launcher.toml
```
のように指定して起動します。

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
