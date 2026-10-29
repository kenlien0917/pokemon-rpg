# Pokémon RPG — Qt / C++

Computer Science II **Group 9** 課程專案的整理與修正版。使用 Qt Widgets / Graphics View 實作地圖探索、道具、初始寶可夢、隨機遭遇、回合制戰鬥、捕捉與進化。

## 本次修正

- 戰鬥以明確回合狀態控制，防止連點造成重複攻擊或道具消耗；離開或重開戰鬥會使舊延遲回呼失效。
- 統一戰鬥選單清理，避免重新進入戰鬥後使用已刪除的按鈕；野生寶可夢以明確所有權管理，捕捉成功才移交隊伍。
- 升級保留原物件位址，修正外部參照失效；開戰挑選尚有 HP 的成員，強制換人不再讓對手多攻擊一次，HP 顯示範圍也會隨成員更新。
- 修正草系進階技能誤用火系判斷、HP 上下界、背包標籤重建與跨遊戲實例共用回呼。
- 合併重複的建構子、技能資料、對話框與場景切換流程，移除未使用的 MainWindow scaffold。
- 非使用中的地圖停止更新；進出場景重置移動方向。草叢遭遇改在角色實際移動後檢查，開背包或對話時不會原地刷遭遇。
- 保留地圖原有的角色中心碰撞設計，改查下一步位置；補上實際地圖傳送門與領取初始寶可夢的整合測試。
- 隊伍選單可捲動；從主選單開始會建立新遊戲，清空舊隊伍與道具，重建地圖物件。

詳細原因與測試見 [修正紀錄](docs/CHANGES.md)。

## 建置與測試

需求：**Qt 5.12+（建議使用已驗證的 5.15）、C++11、CMake 3.16+**。尚未驗證 Qt 6。

Ubuntu：

```sh
sudo apt-get install qtbase5-dev cmake g++
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure
./build/PokemonRPG
```

Windows 可在 Qt Creator 開啟 `CMakeLists.txt` 或原本的 `PokemonRPG.pro`，選擇 Qt 5 kit 建置。命令列使用 CMake 時，將 `CMAKE_PREFIX_PATH` 指向該 Qt kit。Qt 資源編譯進程式，不依賴啟動時的工作目錄。

可用 `python3 tests/check_resources.py` 檢查素材與標頭路徑大小寫。GitHub Actions 會執行建置、QtTest 與 ASan / UBSan；offscreen Qt 平台的 leak detection 關閉，因此不宣稱已證明完全無記憶體洩漏。

## 操作

| 操作 | 按鍵／按鈕 |
|---|---|
| 開始新遊戲 | 主選單滑鼠點擊 |
| 移動 | 方向鍵 |
| 箱子、招牌、NPC、初始寶可夢 | A |
| 開關背包 | B |
| 返回主選單 | M；再次開始會重置進度 |
| 戰鬥 | Fight、Bag、Pokemon、Run |

建議先從 Town 右下方入口進入 Lab，領取初始寶可夢，再前往 Town 上方的 GrassLand。勝利會升級，3 級與 5 級進化；Potion 回復 10 HP，Ether 補滿 PP，野生寶可夢 HP 越低越容易捕捉。Pokeball 最多攜帶三個。強制換人不消耗新回合，主動換人則消耗回合。

原版簡化的傷害、捕捉機率及敵人招式規則保留，未新增存檔或完整寶可夢規則。測試以 Linux offscreen Qt 執行，未宣稱完成 Windows 互動遊玩測試。

## 來源

原始資料為使用者提供的 `Project1_Group9.tar`；此 repo 保留 **Group 9 小組作品**的歸屬，不把整個遊戲列為單人獨立創作。課程報告與個人 Qt Creator 設定未上傳。

原素材為課程包中的 Pokémon 圖像與地圖，其角色／品牌屬於各自權利人；原包未提供完整素材來源或授權清單，本次沒有替素材或小組程式另行授權。原檔與修正版雜湊見 [source-manifest.json](source-manifest.json)。
