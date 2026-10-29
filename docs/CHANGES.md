# Review and repair record — 2026-10-30

## Reproduced defects

The original game was built with GCC 13.3 / Qt 5.15.13 on Ubuntu 24.04 after repairing two build blockers: a wrong-case `SceneManager.h` include and a missing `QEvent` include. The initial behavioral suite reported failures for skill assignment, HP bounds, label recreation, double attack, globally shared overflow callback, and opening with a fainted party member. It also crashed when an old battle callback fired after its scene had been destroyed.

| Area | Cause | Repair |
|---|---|---|
| Skill assignment | Copied fire-family condition for Growth / Razor Leaf | One explicit family table; species-specific advanced moves |
| HP | Setter accepted negative / over-max values | Clamp to `[0, maxHP]` |
| Level-up | Deleted party object while other components retained its address | Assign upgraded data into the existing object |
| Battle actions | Multiple clicks scheduled multiple enemy turns | Explicit phase guard; consume each action once |
| Delayed effects | Unscoped lambdas survived destroyed / exited scenes | QObject-bound callback plus battle-generation check |
| Battle menus | Scene clear deleted graphics objects but cached pointers survived | One cleanup path and menu collection |
| Captures / encounters | Unclear raw-pointer ownership | Manager owns pending encounters; battle owns wild Pokémon; capture transfers ownership to party |
| Switching | Selected index survived encounters; HP-bar range stayed unchanged | Pick a healthy member at entry, reset index, refresh all player display values |
| Forced replacement | Switching after fainting spent another enemy turn | Distinguish forced replacement from voluntary switching |
| Party size | Fixed-position list clipped later captures | Scrollable selector |
| Inventory | Deleted labels before looking them up again | Reuse named labels; remove only stale entries |
| Overflow callback | Single global callback across all managers | Callback stored on each manager and rebound to the active map |
| Exploration | Hidden maps continued ticking; movement could survive a scene change | Explicit scene entry / exit hooks, stop inactive timers and clear direction |
| Encounters | Checked arrow presses even without movement | Check only after a real movement step on the active grass map |
| Dialogs | Two almost identical widget implementations | Shared dialog; NPC-only pagination flag |
| Restart | Menu could resume a permanently fainted party | Starting creates fresh progress and map objects |

## Review correction

An intermediate full-sprite collision change was rejected by independent review because the existing maps use a **35×48 sprite's center** for narrow doors, barriers and interactions. A real-map regression test reproduced the blocked north portal. The final implementation preserves center-based map geometry, queries the future center, and prevents disabled players from advancing. Tests exercise Town's north portal, Lab's exit and starter selection using the actual resources.

## Simplification scope

Consolidated repeated Pokémon initialization and skill rules, scene switching, dialog construction, battle action wiring and menu cleanup. Removed unused MainWindow files. Kept the original maps, art and basic combat formulas. The original enemy unlimited-PP simplification remains; this is not an implementation of the full commercial game rules.

## Verification

- Resource and case-sensitive include check passes.
- 14 QtTest scenarios pass (16 QtTest results including initialization and cleanup).
- Same suite passes under AddressSanitizer and UndefinedBehaviorSanitizer. Leak detection is disabled for the offscreen Qt platform, so these checks do not establish absence of all leaks.
- The suite covers skill families, HP bounds, party reference stability, label reuse, movement, duplicate actions, callback isolation, healthy battle entry, delayed-turn cancellation, new-game reset, real-map progression, scrollable parties, repeated battle menus, and forced switching with correct HP-bar range.

Headless tests exercise real Qt widgets and assets. Interactive Windows play, complete map exploration and exhaustive random battle outcomes are outside this verification record.
