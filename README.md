# hremap

## 概要

変換キーとの同時押しを，矢印等のキーにマップします。

## キーマップ

- --henkan-map

| KEY                | MAP                      |
|--------------------|--------------------------|
| HENKAN + SPC       | ESC                      |
| HENKAN + h/j/k/l   | Left / Down / Up / Right |
| HENKAN + i/o       | PgUp / PgDn              |
| HENKAN + ,/.       | Home / End               |
| HENKAN + m         | Delete                   |
| HENKAN + u         | App (Compose)            |
| HENKAN + 1,2,...,0 | F1,F2,...,F10            |
| HENKAN             | HENKAN                   |

HENKAN と同時押しの場合，他の modifier (Ctrl, Alt, Shift) はそのまま通す。

- --muhenkan-map

| KEY                | MAP                      |
|--------------------|--------------------------|
| HENKAN + ????      | ALT + ????               |
| MUHENKAN           | MUHENKAN                 |

MUHENKAN のみの場合は，MUHENKAN を入力される
MUHENKAN と他のキーの同時押しの場合は，LALT + ???? が入力される

- -e|--enable-ctrl-map オプション

| KEY      | MAP       |
|----------|-----------|
| CTRL + h | Backspace |
| CTRL + m | Enter     |
| PAUSE    | Ctrl + h  |

- -j|-jp-to-us オプション

| KEY              | MAP                 |
|------------------|---------------------|
| \ _              | RightShift          |
| RightShift       | RightCtrl           |
| KatakanaHiragana | RightMeta(RightWin) |
| CapsLock         | RightCtrl           |
| LeftCtrl         | Katakana            |
| ScrollLock	   | CapsLock            |

- -h|-hhk-jp-to-us オプション

| KEY              | MAP                 |
|------------------|---------------------|
| ` ~              | Muhenkan            |
| ESC              | ` ~                 |
| \ _              | RightShift          |
| RightShift       | RightCtrl           |
| KatakanaHiragana | RightMeta(RightWin) |
| LeftCtrl         | Katakana            |
| CapsLock         | RightCtrl           |
| ScrollLock	   | CapsLock            |

- -f|--enable-function-map オプション

| KEY              | MAP                                      |
|------------------|------------------------------------------|
| F1,F2,F3,F4      | RightCtrl,Alt+Shift+Ctrl+Win+1,2,3,4     |
| F5,F6,F7,F8      | Alt+Shift+Ctrl+Win+h,j,k,l               |
| F9,F10,F11,F12   | Alt+Shift+Ctrl+Win+y,u,i.o               |

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
