---
title: "第一回 簡易Forthインタプリタを作ろう"
layout: page
---


# 第一回 簡易Forthインタプリタを作ろう

やる事

- ハッシュの実装
- 線形リストの実装
- 簡単なインタープリタの実装
   - ワード定義、if、whileくらいまで
- 3000行くらい。

Forthは触るとすぐわかるけど説明するとややこしい言語仕様なので、作りながら言語自体の説明はしていきます。


## 01 intのパーサーを作ろう

とりあえずintのパーサーを作る所から始めてみましょう。
初回なので進め方の説明もします。

### githubでの作業の進め方

1. githubのアカウントを作る
2. https://github.com/karino2/c-lesson/ をforkする
3. 自分のc-lessonをローカルにcloneする
4. ローカルで、karino-orignというブランチを作って、remoteにhttps://github.com/karino2/c-lesson.git を指定する
5. 問題は毎回ローカルでブランチを作り、ブランチをgithubにはプッシュする（マスターはいじらない）

こんな感じで作業をしていきましょう。

今回の問題は、01_int_parserというブランチでやるとします。

### int_parserの問題をやってみよう

sources/forth_modoki/01_int_parser/int_parser.c を修正しassertを通るようにしてcommitしてpushしてみて下さい。