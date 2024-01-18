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
