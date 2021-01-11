# Pseudo3D

C++11 と DX ライブラリをを使用した 疑似 3D の基本的なプログラムです。  

# Requirement

* Visual Studio 2019
* DX ライブラリ

# Note

__dxlib__ ディレクトリを作成して  
そこに DX ライブラリの __プロジェクトに追加すべきファイル_VC用__ の中身をコピーして下さい。  
(Visual Studio のプロジェクト設定がされています)

_USE_RASTERIZE をプリプロセッサに設定すると  
描画の処理は DX ライブラリの DrawModiGraph を使用しなくなり  
libpng と png++ を使用したソフトウェア処理に切り替わります。