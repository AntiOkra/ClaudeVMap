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

1. Nastran メッシュ読込(`GRID*` ロング形式 / 小フィールド `GRID` /
   フリーフォーマット(カンマ区切り)/ `CTRIA3` / `CQUAD4` / `CBEAM`)
2. 法線・接線圧力ベクトル読込(ヘッダ行数は自動判別)
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

### 荷重移送モード(保存性)

```sh
# 距離重み付きで近傍 k 点に分配し、範囲外も全体最近傍へ回し、合計を保存
./build/vmap ... --mode weighted --k 4 --idw-power 2 --no-loss --conserve
```

| オプション | 効果 |
|---|---|
| `--mode nearest` | 従来通り、最近傍 1 点へ全荷重(既定) |
| `--mode weighted` | 探索距離内の **k 近傍へ逆距離重み付き分配**(源ごとに保存) |
| `--mode projection` | 最近傍の**表面三角形へ投影し、面積座標(重心座標)で 3 頂点へ分配**。空間的に正確で、源ごとに厳密保存(CAE 標準の consistent load transfer) |
| `--mode sampled` | **源要素を細分積分してサンプル化し、各サンプルを面投影**。粗→細・細→粗の**両方向**に強く、保存的。`--samples` で細分数を指定 |
| `--samples <n>` | sampled の各源要素あたりのエッジ細分数(既定 3 → 三角形あたり n² サンプル) |
| `--k <n>` | weighted の近傍数(既定 4) |
| `--idw-power <p>` | 逆距離の指数(既定 2) |
| `--no-loss` | 範囲内に対象が無い荷重を**全体最近傍へフォールバック**(消失ゼロ) |
| `--conserve` | 移送後に**合計荷重を元の合計へ再正規化**(成分別、グローバル保存を保証) |
| `--conserve-moment` | 自己釣合いの補正を加えて**合計モーメント Σr×F も保存**(力は不変) |
| `--normal-filter <deg>` | projection/sampled で**法線が源方向と <deg> 以内の面のみ採用**(薄板の表裏誤マッピング防止) |
| `--normal-flip` | `--normal-filter` が使う源法線の向きを反転 |

実行後に「Applied / Mapped / Lost」荷重と、取りこぼした場合の最大未マッチ距離を
表示するので、探索距離の調整に使えます。

## 元プログラムに対して直した点 / 改善した点

### 実バグの修正(元の MFC ソースにも反映済み)

- **荷重出力フィルタの不具合**(`SurfaceNode.cpp` `ExportAdxForce`)
  出力対象の判定が `x != 0 || x != 0 || x != 0` と **3 つとも X 成分**を
  見ており、**X がゼロで Y/Z が非ゼロのノードが出力から漏れて**いました。
  `x || y || z` に修正。テスト `TestExportFilterBugfix` で検証。

- **自己代入**(`AdxNode.cpp` `Copy`)
  `m_ForceVector = m_ForceVector;` を `= n.m_ForceVector;` に修正。

- **所有権の明確化**(`SurfaceNode.h` / `.cpp`、`Adx.cpp`)
  `CSurfaceNode::m_vNode` を生ポインタ + 手動 `new`/`delete` から
  `std::vector<std::unique_ptr<CAdxNode>>` に変更(例外安全・リーク防止)。
  ※ MFC 依存のため本環境ではコンパイル検証不可。Windows/VS でのビルド確認を推奨。

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

### 荷重保存(load conservation)の改善

従来は「各 Nastran ノードの荷重を最近傍 ADX ノード **1 点**へ全部加算」する
方式で、(a) 探索距離外の荷重は**消失**して**合計が保存されない**、
(b) メッシュ密度差で**空間的に偏る**、という弱点がありました。移植コアでは
上記の `weighted` / `projection` / `--no-loss` / `--conserve` を追加し、
保存的な荷重移送を選べるようにしています(既定は後方互換の単一最近傍)。

特に `projection`(面投影 + 重心座標補間)は、源ノードを最近傍の表面三角形へ
投影し、面積座標で 3 頂点へ分配します。重みの和が 1 のため**源ごとに荷重を
厳密保存**し、ノード集中や飛び地への漏れが起きにくい、CAE 標準の方式です。

#### メッシュ密度比への注意(粗→細・細→粗)

`projection` は **源ノードごと**に最近傍フェイスへ分配する source-driven 方式です。
**源(Nastran)が粗く・対象(ADX)が細かい**場合、各源ノードの大きな荷重が
細かい 1 枚の三角形に集中し、間のノードがゼロになる**スパイク**が生じます
(合計は保存されますが分布が不正確)。

`sampled` はこれを解決します。**各源要素を細分積分してサンプル化**し、各サンプル
(圧力 × 部分面積)を面投影で分配するため、粗い源でも多数のサンプルが細かい面に
**滑らかに**載ります。**粗→細・細→粗のどちらでも保存的**で、密度比に依存しません。
`--samples` を上げるほど分布が滑らかになります(コストはサンプル数に比例)。

| 状況 | 推奨モード |
|---|---|
| 源が細・対象が粗 | `projection` または `sampled` |
| 源が粗・対象が細 | **`sampled`**(`projection` はスパイク化) |
| とにかく合計を厳守 | 任意のモード + `--conserve` |

`TestWeightedDistribution` / `TestConservation` / `TestFallbackNoLoss` /
`TestFaceProjection` / `TestFaceProjectionNearVertex` /
`TestSampledCoarseToFine`(粗→細でスパイクが出ず分散することを検証)で確認。

### モーメント保存・法線フィルタ・入力堅牢性

- **モーメント保存(`--conserve-moment`)**: 合計力に加え、自己釣合いの補正場
  `δf_i = b × (r_i − o)` を加えて**合計モーメントも一致**させます(o は対象重心)。
  力の合計は変えません。退化(共線配置)には正則化付き 3×3 解で対応。
  `TestMomentConservation` で検証。
- **法線整合フィルタ(`--normal-filter`)**: 薄板で表裏が近接する場合に、
  源の法線方向と整合する面のみを採用し、**反対面への誤マッピングを防止**。
  `TestNormalFilter` で検証。
- **入力堅牢性**: `GRID*`/小フィールド/フリーフォーマットの読込、圧力ファイルの
  **ヘッダ自動判別**(マジックナンバー9の排除)、重複ノードID検出、不正行のスキップ
  集計。`TestInputRobustness` で検証。

### テスト

`tests/test_core.cpp` に幾何計算・荷重計算・最近傍探索・出力フィルタ・
荷重保存・面投影補間・密度比(粗⇄細)・モーメント保存・法線フィルタ・
入力堅牢性・ADX 読込〜マッピングのエンドツーエンドまで含む 73 件のチェックが
あります。

## CI

`.github/workflows/ci.yml` で、push 毎に移植コアを **Linux(gcc / clang)+
Windows(MSVC)** でビルドし `ctest` を実行します(MFC GUI は Windows 専用の
ため CI 対象外)。

## ディレクトリ構成

```
cross_platform/
├── include/vm/   ヘッダ(Vec3, Geometry, StringUtil, AreaMap, Nastran, Adx, Mapper)
├── src/          実装 + CLI(cli_main.cpp)
├── tests/        単体・結合テスト
└── CMakeLists.txt
```
