手持ちの書籍を管理したい

意気揚々と新しい本を買ってきたかと思ったら、なんと既に購入済みだった…そんなつらい展開をなんとかしたいと思いたち、バーコードリーダーを使って書籍データベースの管理を簡単にするべく、以下略

## 今できること

- バーコードリーダーの出力を受け取る（同期処理でつらい）
- OpenBDからタイトル・著者と表紙画像を取得

## udevルールの作成

- 以下の感じで、必要に応じてudevルールを作成しましょう

```
KERNEL=="ttyACM*", ATTRS{idVendor}=="065a", ATTRS{idProduct}=="a002", MODE="0666"
```
## めもがき

- ISBN・書名・著者・書影しか保存してないけど、他にも保存しても良いのかもしれない
  - 感想とかメモ書きとか？
- ジャンル管理とかしたい

