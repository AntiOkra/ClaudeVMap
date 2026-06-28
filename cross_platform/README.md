# VectorMapping – クロスプラットフォーム コア

元の `VectorMapping`(Windows / MFC の GUI アプリ)の中核ロジックを
**MFC 非依存の移植版**として切り出したものです。Linux / macOS / Windows の
いずれでも CMake でビルド・テストでき、CI や自動テストに載せられます。

元の MFC プロジェクト(`../VectorMapping/`)はそのまま残してあり、
Windows / Visual Studio で引き続きビルドできます。本ディレクトリは
「同じアルゴリズムの検証可能なリファレンス実装 + コマンドライン版」です。

## 何をするツールか

CFD/圧力解析の結果(Nastran 面メッシュ上の法線・接線圧力)を、
構造解析用の ADX メッシュ(3D 二次四面体)の表面ノードへ
**荷重としてマッピング(移送)** します。

パイプライン:

1. Nastran メッシュ読込(`GRID*` / `CTRIA3` / `CQUAD4` / `CBEAM`)
2. 法線・接線圧力ベクトル読込
3. 荷重計算 `Force = (法線圧力 + 接線圧力) × ノード面積 × 1000` [N]
4. ADX メッシュ読込 → 表面トポロジ構築 → 選択要素セットの表面ノード抽出
5. 空間グリッドによる最近傍探索で、各 Nastran ノードの荷重を
   探索距離内の最も近い ADX 表面ノードへ加算(軸別の倍率指定可)
6. ADX 荷重ファイル出力

## ビルドと実行

```sh
cd cross_platform
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure   # or: ./build/vmcore_tests
```

CLI:

```sh
./build/vmap \
  --nastran mesh.nas --normal normal.txt --tangent tangent.txt \
  --adx mesh.adx --out force.txt \
  --set Body_1_e1 --distance 30 --ratio 1 --process AdvcStep-99999
```

`--set` を省略すると全要素セットが対象になります。`--ratio-xyz x y z` で
軸別倍率を指定できます。

## 元プログラムに対して直した点 / 改善した点

### 実バグの修正(元の MFC ソースにも反映済み)

- **荷重出力フィルタの不具合**(`SurfaceNode.cpp` `ExportAdxForce`)
  出力対象の判定が `x != 0 || x != 0 || x != 0` と **3 つとも X 成分**を
  見ており、**X がゼロで Y/Z が非ゼロのノードが出力から漏れて**いました。
  `x || y || z` に修正。テスト `TestExportFilterBugfix` で検証。

- **自己代入**(`AdxNode.cpp` `Copy`)
  `m_ForceVector = m_ForceVector;` を `= n.m_ForceVector;` に修正。

- **未知ノード参照の暗黙吸収**(`CNastran.cpp` `Indexing`)
  要素が参照するノード ID が存在しない場合、`std::map::operator[]` が
  **黙って index 0 を挿入**し、誤ったノードに面積が積算され得ました。
  `find` で検証してエラーを返すよう変更。あわせて `CBEAM` のノード数を
  正しく 2 に。テスト `TestDanglingNodeRef` で検証。

### 堅牢性 / 性能の改善

- **最近傍探索の探索範囲**(`SurfaceNode.cpp` `NearestNode`、移植版 `AreaMap`)
  従来は固定で前後 ±1 セルのみを探索していました。グリッドピッチ(50)より
  探索距離が大きいと範囲内の最近傍を取りこぼす潜在的問題があります
  (現状 GUI は探索距離を最大 30 に制限しているため顕在化はしていません)。
  探索距離からセル範囲を算出するように変更し、ピッチに依存せず正しく
  探索します。テスト `TestAreaMapAdaptiveRange` で検証。

  > 注: 元のグリッド探索ロジック自体は、セル単位で真の最近傍を返す設計のため
  > 全探索版(`NearestNodeSimple`)と結果は等価でした。これは計算結果バグ
  > ではなく、上記は探索範囲に関する堅牢性改善です。

- **空間グリッドのメモリ効率**(移植版 `AreaMap`)
  元の密な 3 次元配列(`ixno × iyno × izno` 個のセルを常に確保)を、
  **占有セルのみ保持する疎なハッシュグリッド**に変更。薄く広い表面でも
  メモリを浪費しません。`TestAreaMapNearestMatchesBrute` で全探索と一致を確認。

### テスト

`tests/test_core.cpp` に幾何計算・荷重計算・最近傍探索・出力フィルタ・
ADX 読込〜マッピングのエンドツーエンドまで含む 35 件のチェックがあります。

## ディレクトリ構成

```
cross_platform/
├── include/vm/   ヘッダ(Vec3, StringUtil, AreaMap, Nastran, Adx, Mapper)
├── src/          実装 + CLI(cli_main.cpp)
├── tests/        単体・結合テスト
└── CMakeLists.txt
```
