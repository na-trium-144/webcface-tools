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
