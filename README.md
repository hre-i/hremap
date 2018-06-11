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

- -j|-jp-to-us オプション

| KEY              | MAP                 |
|------------------|---------------------|
| \ _              | RightShift          |
| RightShift       | RightCtrl           |
| KatakanaHiragana | RightMeta(RightWin) |
| Muhenkan         | Esc                 |
| Esc              | ` ~                 |
| LeftCtrl         | Muhenkan            |

- -h|-hhk-jp-to-us オプション

| KEY              | MAP                 |
|------------------|---------------------|
| \ _              | RightShift          |
| RightShift       | RightCtrl           |
| KatakanaHiragana | RightMeta(RightWin) |
| Muhenkan         | Esc                 |
| ` ~              | Muhenkan            |

- -f|--enable-function-map オプション

| KEY              | MAP                                      |
|------------------|------------------------------------------|
| F1,F2,F3,F4      | RightCtrl,Alt+Shift+Ctrl+Win+1,2,3,4     |
| F5,F6,F7,F8      | Alt+Shift+Ctrl+Win+h,j,k,l               |
| F9,F10,F11,F12   | Alt+Shift+Ctrl+Win+y,u,i.o               |

- -m|--enable-muhankan-map オプション

| KEY              | MAP                                      |
|------------------|------------------------------------------|
| Muhenkan+1,2,3,4 | RightCtrl,Alt+Shift+Ctrl+Win+1,2,3,4     |
| Muhenkan+h,j,k,l | Alt+Shift+Ctrl+Win+h,j,k,l               |
| Muhenkan+q,w,a,s | Alt+Shift+Ctrl+Win+q,w,a,s               |
| Muhenkan+y,u,i,o | Alt+Shift+Ctrl+Win+y,u,i.o               |
| Muhenkan+z,x,c,v | Alt+Shift+Ctrl+Win+z,x,c,v               |

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
