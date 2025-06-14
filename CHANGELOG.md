## [2.3.1] - 2025-06-09
### Changed
* launcherが実行したコマンドのログ出力をパースする際 \n だけでなく \r やカーソル移動を伴うANSIエスケープシーケンスも改行と同様に扱うようにした

## [2.3.0] - 2025-02-09
### Added
* webcface-joystick (#43)
### Changed
* subprojectにsdl2とfmt追加
* default_optionsのbuildtype=releaseを消した
### Fixed
* MSVC,MinGWでのコンパイルエラーを修正

## [2.2.0] - 2025-01-17
### Added
* webcface-notepad (#56)
* webcface-tui でView要素のwidth,heightプロパティに対応 (#58)

## [2.1.3] - 2024-11-30
### Fixed
* webcface-launcher.service から呼び出すlauncherコマンドを絶対パスに変更 (#55)

## [2.1.2] - 2024-11-18
### Changed
* Launcherでstderrのログをwarn表示に変更、コマンドの開始時と終了時にdebugログを追加 (#54)

## [2.1.1] - 2024-10-30
### Added
* Launcherにkill機能(killボタン、`コマンド名/kill` 関数)追加
### Fixed
* コマンド名/stop などの関数を複数回実行した場合、そのすべての呼び出しがコマンドの終了時に正常終了するようにした

## [2.1.0] - 2024-10-03
### Changed
* Launcher機能追加 (#50)
	* `コマンド名.exit_status`, `コマンド名.running` というValueデータでコマンドの状態を取得できるようになった
	* `コマンド名/stop` 関数はコマンドが停止するまで待機するようになった
	* `コマンド名/run` 関数追加 (startをして、コマンドが停止するまで待機する)
	* webcface-sendを使わなくても実行したコマンドのログをLogデータとして送信するようにした
		* tomlのstdout_captureオプションを今までのような文字列ではなくtrueまたはfalseで指定するようにした

## [2.0.1] - 2024-08-27
### Fixed
* cli11のパスを通していてもnot foundになっていたのを修正 (#48)

## [2.0.0] - 2024-08-27
### Changed
* Mesonに移行、WebCFace ver2に合わせて修正 (#45)
* webcface-cv-capture 削除
### Added
* webcface-tui, webcface-ls (#46)

## [1.4.5] - 2024-07-03
### Added
* launcherのstopボタンの動作を変更するオプションついか
* toolsをビルドしたときのバージョン番号にgit describeの出力を追加

## [1.4.4] - 2024-04-26
### Added
* Ubuntu24.04のビルドを追加
### Changed
* CMakeListsをターゲットごとに分割 (#42)

## [1.4.3] - 2024-04-08
### Changed
* launcherのstart,stopボタンで呼び出される関数の非表示設定を解除、名前を`command名/start`などに変更 (#39)
* CMakeListsでwebcfaceのバージョンを1.3以上と指定

## [1.4.2] - 2024-02-24
### Fixed
* (unixのみ) launcherを強制終了したときlauncher内で実行中のコマンドにもシグナルを送るようにした (#33)

## [1.4.1] - 2024-02-22
### Fixed
* Releaseのdebパッケージでserviceがインストールされてなかったのを修正

## [1.4.0] - 2024-02-22
### Added
* launcherでコマンドの環境変数を設定する機能 (#29)
### Changed
* tomlファイルをパースする時のエラーチェックを厳しくした(文字列ではなく数値やboolが入っているなど)
* stdout_captureが有効のとき標準出力にも出力するようにした

## [1.3.1] - 2024-02-09
### Added
* ReleaseするUbuntuのdebパッケージで lib/systemd/system/webcface-launcher.service をインストールするようにした (#25)
* launcherでstdoutとstderrをキャプチャしないようにする(ver1.2以前の仕様にする)設定項目を追加 (#23)
* launcher, sendで入力がutf8かどうか指定するオプション追加(windowsのみ) (#23)
	* ver1.3.0まではすべてデフォルトでutf8とみなしていたが、1.3.1ではデフォルトでACPとみなしutf8へ変換するよう仕様変更
### Fixed
* Linux,Macでlauncherの引数に絶対パスを渡せないバグを修正 (#25)
* RPATHが設定されておらずライブラリを読み込めない問題を修正 (#24)

## [1.3.0] - 2024-01-19
### Added
* webcface-cv-capture (#19)
### Changed
* launcherのstartとstopのボタンを別々にした & エラー時ログ表示追加 (#18)


## [1.2.1] - 2024-01-15
### Fixed
* windowsでのビルドエラーを修正 (#17)
### Changed
* ciでwindowsのビルドをテストするようにした

## [1.2.0] - 2024-01-14
### Added
* launcherに標準入力から設定データを渡せる -s オプション追加 (#16)
### Changed
* launcherでコマンド実行時、終了時、エラー時などにログを出すようにした

## [1.1.7] - 2023-12-12
### Changed
* 依存ライブラリをsubmoduleではなくFetchContentで取得するように変更 (#15)
* ciでMacOSでのビルドもテスト

## [1.1.6] - 2023-12-08
### Fixed
* msvcでビルドするときutf-8フラグを追加するようにした

## [1.1.5] - 2023-12-08
### Added
* LICENSE (#8)
* LICENSEを/usr/shareにインストールするようにした (#11)
* バージョン番号にサフィックスをつけられるオプション追加
* Ubuntu20.04でのReleaseを追加

### Fixed
* subdirectoryなど、インストールしてないwebcfaceを使用してもcmakeが通るようにした (#7)

## [1.1.4] - 2023-11-30
### Fixed
* webcface-sendでvalueに数値以外の文字列を渡すとそれ以上値を受け取らなくなるのを修正
### Added
* webcface-sendで入力したものを標準出力に流すかどうかを切り替えられるオプション追加 (#6)

## [1.1.3] - 2023-11-07
### Changed
* webcface 1.1.4での変更に合わせて、ciでのテストとReleaseビルドをubuntu-22.04に戻した

## [1.1.2] - 2023-10-26
### Changed
* ciでのテストとReleaseビルドをubuntu-22.04から20.04に変更

## [1.1.1] - 2023-10-20
### Changed
* コマンドラインパーサーをcli11に変更 (#2)
	* オプションは変えていません

## [1.1.0] - 2023-10-17
### Added
* webcface-send (#1)
### Changed
* launcherにコマンドライン引数を追加 (#1)

## [1.0.0] - 2023-10-03
### Added
* めっちゃ適当な実装のlauncher
