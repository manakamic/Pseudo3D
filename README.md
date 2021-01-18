# Pseudo3D

C++11 と DX ライブラリをを使用した 疑似 3D の基本的なプログラムです。  
vertex / camera / vector / matrix / 等の class を用意して  
ビュー変換 / パースペクティブ変換などの 3D 処理を自前で行います。  

# Requirement

* Visual Studio 2019  
* DX ライブラリ  
* libpng(+ zlib)  
  (static library 追加済)
* png++  
  (source file 追加済)

# Note

__dxlib__ ディレクトリを作成して  
そこに DX ライブラリの __プロジェクトに追加すべきファイル_VC用__ の中身をコピーして下さい。  
(Visual Studio のプロジェクト設定がされています)  



**_USE_RASTERIZE** をプリプロセッサに設定すると  
描画の処理は DX ライブラリの描画関数 (DrawModiGraph) を使用しなくなり  
libpng と png++ を使用したソフトウェア処理に切り替わります。  

* ピクセルのラスタライズ
* パースペクティブ コレクト テクスチャ マッピング
* デプスバッファ
* 擬似ダブル バッファリング