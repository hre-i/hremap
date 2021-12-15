# hremap

## 概要

変換キーとの同時押しを，矢印等のキーにマップします。

## キーマップ

1. SIMPLE キーマップ変換
2. COMPLEX キーマップ変換

### SIMPLE キーマップ変換

#### --jp-to-us

| KEY              | MAP                 | 備考                     |
|------------------|---------------------|--------------------------|
| ScrollLock	   | CapsLock            | CapsLock の退避          |
| \ _              | RightShift          |                          |
| YEN bar          | ESC                 |                          |
| RightShift       | RightCtrl           |                          |
| CapsLock         | RightCtrl           |                          |
| KatakanaHiragana | RightMeta(RightWin) |                          |
| LeftCtrl         | Katakana            | katakana-map 用          |

#### -hhk-jp-to-us

| KEY              | MAP                 |
|------------------|---------------------|
| ` ~              | ESC                 |
| ESC              | ` ~                 |

## COMPLEX キーマップ

### --henkan-map

| KEY                | MAP                      |
|--------------------|--------------------------|
| HENKAN + h/j/k/l   | Left / Down / Up / Right |
| HENKAN + i/o       | PgUp / PgDn              |
| HENKAN + ,/.       | Home / End               |
| HENKAN + m         | Delete                   |
| HENKAN + u         | App (Compose)            |
| HENKAN + 1,2,...,0 | F1,F2,...,F10            |
| HENKAN + w         | Win+2                    |
| HENKAN + e         | Win+4                    |
| HENKAN + r         | Win+6                    |
| HENKAN + t         | Win+8                    |
| HENKAN + a         | Win+1                    |
| HENKAN + s         | Win+3                    |
| HENKAN + d         | Win+5                    |
| HENKAN + f         | Win+7                    |
| HENKAN + g         | Win+9                    |

HENKAN と同時押しの場合，他の modifier (Ctrl, Alt, Shift) はそのまま通す。

### --henkan-only-to-henkan

「--henkan-map」 と同時に指定した場合，変換キーのみ押してすぐに放した場合は，
変換キーを入力する。

### --muhenkan-map

| KEY                | MAP                      |
|--------------------|--------------------------|
| HENKAN + ????      | ALT + ????               |
| MUHENKAN           | MUHENKAN                 |

MUHENKAN のみの場合は，MUHENKAN を入力される
MUHENKAN と他のキーの同時押しの場合は，LALT + ???? が入力される

### -e|--ctrl-map オプション

| KEY      | MAP       |
|----------|-----------|
| CTRL + h | Backspace |
| CTRL + m | Enter     |
| PAUSE    | Ctrl + h  |

### --katakana-map 

「かたかな」を，CTRL+SHIFT+WIN にマッピング

## インストール方法

```
$ make
$ sudo make install
```

systemd から起動する場合は，

```
$ sudo make setup
```

## 使い方

hremap の実行には，以下の権限が必要です：

  - /dev/input/uinput, /dev/input/event... の読み書き権限

起動方法は systemd/hremap 参照。

## 謝辞

[x11keymacs](http://yashiromann.sakura.ne.jp/x11keymacs/) のコードを使用させていただいてます。
