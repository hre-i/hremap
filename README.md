# hremap

## 概要

変換キーとの同時押しを，矢印等のキーにマップします。

## キーマップ

| KEY                | MAP                      |
|--------------------|--------------------------|
| HENKAN + h/j/k/l   | Left / Down / Up / Right |
| HENKAN + i/o       | PgUp / PgDn              |
| HENKAN + ,/.       | Home / End               |
| HENKAN + m         | Delete                   |
| HENKAN + u         | App (Compose)            |
| HENKAN + 1,2,...,0 | F1,F2,...,F10            |

HENKAN と同時押しの場合，他の modifier (Ctrl, Alt, Shift) はそのまま通す。

- -e|--enable-ctrl-map オプション

| KEY      | MAP       |
|----------|-----------|
| CTRL + h | Backspace |
| PAUSE    | Ctrl + h  |

- -f|--enable-function-map オプション

  F1,F2,F3,F4 を押下した際に，Alt+R が先に送られる。

- -C|--caps-to-ctrl

  CAPS を LEFTCTRL に変換する。


## インストール方法

```
$ make
$ sudo make install
```

systemd から起動する場合は，

```
$ cd systemd
$ sudo sh ./setup.sh
```

## 使い方

hremap の実行には，以下の権限が必要です：

  - /dev/input/uinput, /dev/input/event... の読み書き権限

起動方法は systemd/hremap 参照。

## 謝辞

[x11keymacs](http://yashiromann.sakura.ne.jp/x11keymacs/) のコードを使用させていただいてます。
