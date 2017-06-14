# Crop-texture-generator

開発：Shimizu Satoru

====

## Overview
OpenCVを利用した画像切り抜きテクスチャ生成ツール

## Description
Unityで作成したアプリケーションのテクスチャを紙をスキャンしたデータから自動生成するため作成
Imageフォルダ内のすべてのjpg画像を画像に含まれるマーカに従い，クロップし,Maskingフォルダ内masking.jpgを使用（png出力時は透過）し1024px×1024pxの画像として連番でTextureファイルに保存する
左上のマーカをmarker_1.jpg，右下のマーカをmarker_2.jpgとしMarkerフォルダに入れる
サンプルの用紙はpsd_pdf_jpg内に含まれている
\x64\Release\ConsoleApplication1.exeを起動することで動作


## Requirement
OpenCV3.1.0をインストールし，Pathを通しておく必要あり（今後不要になるよう静的リンクの予定）作者はC:\devの下にインストールしている

VisualStudio2015で作成
C++コンソールアプリケーション
プロパティより文字セットを"マルチ バイト文字セットを使用する"に変更する必要あり
