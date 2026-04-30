# Void — 8번 과제 제출물 (NBC_JangSik_Assignment8)

<video src="https://github.com/user-attachments/assets/730361ee-2849-42ea-b6f8-6c4af645d97a" controls width="100%"></video>

> 📋 [마스터 과제 구현](./마스터.md)

3인칭 TPS 좀비 서바이벌 익스트랙션 게임 `Void` 의 **Tier 0 프로토타입**.
스파르타 언리얼 부트캠프 8번 과제 제출물이면서, Void 본편의 첫 구현 스냅샷.

---

## 컨셉 한 줄

> 좀비로 뒤덮인 도시에서 소음과 무게를 관리하며 부품을 파밍하고, 살아서 차량을 수리해 탈출하는 **TPS 서바이벌 익스트랙션**.

---

## 게임 흐름 (3 레벨)

| 레벨 | 역할 | 주요 메커닉 |
|---|---|---|
| **Lv_Main** | 메인 메뉴 | Start / Exit 버튼 |
| **Lv_VoidProto** | 본편 (3 웨이브 = 3층) | 좀비 처치 · 부품 파밍 · 차량 수리 · 시동 |
| **Lv_Escape** | 탈출 엔딩 | "ESCAPED" 화면 + Exit |

탈출 조건: **부품 3종(배터리·연료통·점화 플러그) → 차량 슬롯 3개 설치 → `Repair: 3/3` → 차량 앞 E 키**.
실패 조건: **HP 0 (사망) 또는 10분 탈출 타이머 만료** → GameOver 패널 → ReStart / Exit.

---

## 핵심 시스템

### 캐릭터 / 카메라
- TPS 숄더뷰 카메라 리그 — 오른쪽 어깨 기본
- ADS 조준 — FOV 축소 + SpringArm 보간 + 이동속도 곱셈 합성
- 이동 / 점프 / 시점 — Enhanced Input

### 무기 시스템
- **`UVOIDWeaponComponent`** 캡슐화 — Pawn 은 입력 → 컴포넌트 위임만 담당
- **`UVOIDWeaponConfig` (UDataAsset)** — 펠릿/스프레드/레인지/반동/소음/FX 전 파라미터 외부화
- **샷건 / 라이플** 두 무기 — 1/2 키 스왑, BP Defaults 슬롯에 DA 할당
- **반동 시스템** — 발사 시 Pitch/Yaw 누적, 매 Tick `FInterpTo` 로 0 보간
- **시작 무기 = 샷건** (Rifle 폴백)

### 인벤토리 / 무게
- 타르코프식 kg 단위 무게, **MaxCarry = 30kg**
- 같은 ItemData 자동 스택 (`MaxStackPerItem = 2`)
- 부품 슬롯 타입 자동 매칭 (`FindPartByType`)
- 무게비율(`WeightRatio`)이 이동·소음·디버프 단일 지표

### 좀비 AI
- **AIPerception 청각** — 발사음·픽업음 → 노이즈 방향 감지
- **비동기 가시선** — `AsyncLineTraceByChannel` + `FTraceDelegate` 콜백
  - 0.2초 주기 Tick, 게임 스레드 블로킹 없음
  - 콜백에서 BB `TargetActor` / `bHasTarget` 갱신
- BT/BB 추격 로직, 공격 데미지 10 / 사거리 100 / 시야 1200

### 디버프 시스템 (Bleeding / Fracture / Overweight)
| 디버프 | 트리거 | 효과 |
|---|---|---|
| **Bleeding (출혈)** | 피격 시 30% 확률, 5초 지속 | 1초마다 -1 HP |
| **Fracture (골절)** | 피격 시 30% 확률, 10초 지속 | 이동속도 -50% |
| **Overweight (과적)** | `WeightRatio ≥ 0.5` | 이동 -, 소음 +, 스태미나 - |

> 설계 결정: 디버프는 좀비(데미지 소스) 가 아닌 **플레이어 자신**이 자기 OnHealthChanged 를 듣고 발동. 데미지 소스 비종속 + 단일 책임 + 한 곳에서 튜닝.

### 차량 부품 슬롯 + 시동
- `IVOIDVehiclePartSlot` 인터페이스 (`GetRequiredPartType` / `TryInstallPart` / `IsInstalled`)
- 슬롯 3개 (`Battery` / `FuelTank` / `SparkPlug`) `BP_VoidVehicle` 자식 ChildActorComponent
- 슬롯 → 시동 → 픽업 우선순위 분기 (`Interact()` 단일 진입점)
- `bRepairComplete = true` 시 `TryStartEngine` → GameMode `HandleEscapeSuccess` → `OpenLevel(Lv_Escape)`

### 웨이브 / 스폰볼륨
- `AVOIDGameMode` 가 Preparing / InProgress / Ending / Completed 상태 머신
- `DT_VOIDWaveData` (FTableRowBase) — 좀비수·아이템수·시간·이벤트
- `AVOIDSpawnVolume` BoxComponent 내부 랜덤 + 라인트레이스 바닥 안착
- 층별 Spawn (Floor 1~3) 자동 진입

### 점수 / 타이머
- 좀비 처치 → `AVOIDGameState::AddScore(10)`
- **10분 탈출 타이머** (`EscapeTimeLimit = 600`) — 1초 Tick 으로 GameState `OnTimeChanged` Broadcast
- 만료 시 자동 GameOver

### HUD — 4페이지 모드 (단일 위젯)
| 모드 | 표시 |
|---|---|
| **MainMenu** | StartButton + ExitButton + 마우스 ON / UI Only |
| **InGame** | Health/Weight/Noise/Score/Wave/Time/Repair/Inventory/WeaponIcon |
| **GameOver** | "GAME OVER" 텍스트 + ReStartButton + ExitButton (레벨 전환 X) |
| **GameClear** | "ESCAPED" 텍스트 + ExitButton |

- `UVOIDHUDWidget::SetHUDMode(EVOIDHUDMode)` 가 Lv_1/Lv_2 가시성 + 마우스 + 입력 모드 일괄 토글
- `NativeConstruct` 가 현재 레벨명 → 모드 자동 결정 (PlayerController 설정 0개)
- 버튼 OnClicked 는 C++ `AddUniqueDynamic` 자동 바인딩 (BP 그래프 노드 0개)
- 바인딩 이전 Broadcast 보충 — 현재값으로 핸들러 1회 직접 호출

---

## 컨트롤

| 키 | 동작 |
|---|---|
| `WASD` | 이동 |
| `Mouse` | 시점 |
| `Space` | 점프 |
| `LMB` | 발사 |
| `RMB` | ADS 조준 (홀드) |
| `1` | 라이플 장착 |
| `2` | 샷건 장착 |
| `E` | 인터랙션 (슬롯 → 시동 → 픽업 우선순위) |

---

## 기술 스택

- **Unreal Engine 5.5+**
- **C++ 주도** + Blueprint (연출·UI·BP 디폴트 데이터)
- Enhanced Input System
- UMG (`BindWidget` + 델리게이트 구독 중심)
- `UDataAsset` · `UDataTable` · `USTRUCT` · 인터페이스 (BlueprintNativeEvent)
- `AsyncLineTraceByChannel` · `AIPerception` (청각)
- `FTimerManager` · `FTimerDelegate`

---

## 폴더 구조

```
Source/VoidUnreal/
├── VoidUnreal.Build.cs        ← EnhancedInput, UMG, AIModule 등
├── Public/
│   ├── Core/                  GameMode/GameState/PlayerController/GameInstance
│   ├── Characters/            BaseCharacter/PlayerCharacter/ZombieCharacter
│   ├── AI/                    ZombieAIController (비동기 가시선)
│   ├── Components/            Health/Inventory/Noise/Debuff/Weapon
│   ├── Items/                 ItemDataAsset/ItemInterface/PickupBase/VehiclePartSlot
│   ├── Weapon/                WeaponConfig (DataAsset)
│   ├── Vehicle/               VOIDVehicle/VehiclePartSlotActor
│   ├── Waves/                 WaveData/SpawnVolume/FloorTransitionTrigger
│   └── UI/                    HUD Widget (4 모드)
└── Private/                   (Public 과 대칭)

Content/VoidUnreal/
├── Maps/                      Lv_Main / Lv_VoidProto / Lv_Escape
├── Blueprints/                BP_VOIDGameMode / BP_VOIDPlayerController
├── Blueprints/UI/             WBP_HUD (Lv_1 + Lv_2 컨테이너)
├── Character/                 BP_VoidPlayerCharactor
├── AI/                        BP_VoidZombieAIController + BT/BB
├── Vehicle/                   BP_VoidVehicle (Body + StartEngineVolume + 슬롯 3)
├── Weapons/Data/              DA_Weapon_Rifle / DA_Weapon_Shotgun
├── Weapons/Images/            rifle / shotgun 아이콘
├── Item/                      DA_Part_Battery/FuelTank/SparkPlug
└── Data/                      DT_VOIDWaveData
```

클래스 접두사 **`VOID`** (예: `AVOIDPlayerCharacter`), 모듈 매크로 **`VOIDUNREAL_API`**, 포인터 기본형 `TObjectPtr<T>`.

---

## 빌드 / 실행

1. `VoidUnreal.uproject` 우클릭 → **Generate Visual Studio project files**
2. `VoidUnreal.sln` 열어서 **Development Editor / Win64** 로 빌드 (또는 Live Coding `Ctrl+Alt+F11`)
3. 에디터에서 `Lv_Main` PIE 시작 (Project Default Map)

C++ 멤버 / 생성자 / `UPROPERTY` 추가 시에는 Live Coding 으로는 반영 안 되므로 **Rebuild Solution + 에디터 재시작** 필요.

---

## 필수 과제 / 도전 과제 작업 내용


### 필수 과제

| 항목 | 작업 내용 | 코드/콘텐츠 |
|---|---|---|
| **3단계 웨이브 (진입·심층·탈출)** | `Wave 1/2/3` = 건물 1층/2층/옥상으로 재해석. `AVOIDGameMode` 가 Preparing/InProgress/Ending/Completed 상태 머신 운영 | `Core/VOIDGameMode.cpp::StartWave`, `DT_VOIDWaveData` (좀비수·아이템수·시간) |
| **층 자동 전환** | 계단 끝 트리거 박스(`AVOIDFloorTransitionTrigger`) Overlap → 다음 Floor 진입 | `Waves/VOIDFloorTransitionTrigger.cpp` |
| **스폰 / 타이머 초기화** | `AVOIDSpawnVolume` BoxComponent 내부 랜덤 + 라인트레이스 바닥 안착 / 10분 탈출 타이머 (`EscapeTimeLimit=600`) | `Waves/VOIDSpawnVolume.cpp`, `AVOIDGameState::OnTimeChanged` |
| **HUD 실시간** (체력·점수·시간 + 무게·소음) | `UVOIDHUDWidget` 단일 위젯, `BindWidget` 9종 + 컴포넌트 델리게이트 자동 구독. NativeConstruct 시 현재값 1회 직접 호출로 바인딩 이전 Broadcast 보충 | `UI/VOIDHUDWidget.cpp::BindToPlayer` |
| **메뉴 (시작·재시작·탈출·게임오버)** | 4페이지 모드 (`MainMenu/InGame/GameOver/GameClear`) — `SetHUDMode(EVOIDHUDMode)` 가 Lv_1/Lv_2 가시성 + 마우스 + 입력 모드 일괄 토글, 레벨명 → 모드 자동 결정 | `UI/VOIDHUDWidget.cpp::SetHUDMode` |
| **입력 모드 전환** | InGame=`GameOnly` / Menu·GameOver·GameClear=`UIOnly` + 마우스 표시 자동 토글 | `UI/VOIDHUDWidget.cpp` 모드 토글 |
| **층별 난이도 증가** | 좀비 수·아이템 수 `DT_VOIDWaveData` 행별 차등. Floor 3 옥상은 부품 3종 + 호드 진입 | `Data/DT_VOIDWaveData` |
| **샷건 + 반동 시스템** *(1차 마감 연장으로 필수 과제 합산)* | `UVOIDWeaponComponent` 캡슐화 — 샷건 펠릿 산탄(스프레드/펠릿 카운트) + Pitch/Yaw 누적 반동 → 매 Tick `FInterpTo` 0 보간. 1/2 키 라이플/샷건 스왑, BP Default 슬롯에 `UVOIDWeaponConfig` DA 할당 | `Components/VOIDWeaponComponent.cpp`, `Weapon/VOIDWeaponConfig` (DataAsset) |
| **TPS 숄더뷰 + ADS 조준** | 오른쪽 어깨 SpringArm 기본, RMB 홀드 시 FOV 축소 + SocketOffset 보간 + 이동속도 곱셈 | `Characters/VOIDPlayerCharacter.cpp::AimDownSights` |
| **레벨 전환 (Lv_Main / Lv_VoidProto / Lv_Escape)** | `AVOIDVehicle::TryStartEngine` → `AVOIDGameMode::HandleEscapeSuccess` → `OpenLevel("Lv_Escape")`. `bRepairComplete=false` 가드 | `Vehicle/VOIDVehicle.cpp:34`, `Core/VOIDGameMode.cpp::HandleEscapeSuccess` |
| **게임오버 흐름** | `UVOIDHealthComponent::OnDeath` → `AVOIDPlayerController::HandlePlayerDeath` → `AVOIDGameMode::HandleGameOver` (위젯 표시 + `GameOverReturnDelay` 후 `Lv_Main` 로딩) | `Core/VOIDGameMode.cpp::HandleGameOver` |

### 도전 과제

| 항목 | 작업 내용 | 코드/콘텐츠 |
|---|---|---|
| **디버프 시스템 3종** | 출혈(피격 시 30% 확률, 1초마다 -1 HP) / 골절(피격 시 30% 확률, 이속 -50%) / **과적**(`WeightRatio≥0.5`, 이속 -·소음 +·스태미나 -). 좀비가 아닌 **플레이어 자신**이 `OnHealthChanged` 를 듣고 발동 — 데미지 소스 비종속 + 단일 책임 | `Components/VOIDDebuffComponent.cpp` |
| **차량 부품 슬롯 + 시동** | `IVOIDVehiclePartSlot` 인터페이스(`GetRequiredPartType` / `TryInstallPart` / `IsInstalled`) + 슬롯 3개(Battery/FuelTank/SparkPlug) ChildActorComponent. `Interact()` 단일 진입점에서 슬롯 → 시동 → 픽업 우선순위 분기. 슬롯 `bInstalled` 가드 + 5m 차량 자동 검색 fallback | `Items/VOIDVehiclePartSlot.h`, `Vehicle/VOIDVehiclePartSlotActor.cpp`, `Characters/VOIDPlayerCharacter.cpp::Interact` |
| **타르코프식 무게 시스템** | kg 단위 `MaxCarry=30kg`, `WeightRatio` 단일 지표가 이동·소음·디버프 동시 제어. `MaxStackPerItem=2` 자동 스택, `FindPartByType` 슬롯 자동 매칭 | `Components/VOIDInventoryComponent.cpp` |
| **AIPerception 청각 + 비동기 가시선** | 발사음·픽업음 → 노이즈 방향 감지(AIPerception 청각). `AsyncLineTraceByChannel` + `FTraceDelegate` 콜백 0.2초 주기 — 게임 스레드 블로킹 없이 BB `TargetActor` / `bHasTarget` 갱신 | `AI/VOIDZombieAIController.cpp` |
| **샷건 펠릿 산탄 + 무기 커스터마이징** | `UVOIDWeaponConfig` (UDataAsset) — 펠릿 수/스프레드/레인지/반동/소음/FX/아이콘/표시명 전 파라미터 외부화, BP 노드 0개로 무기 추가 가능 | `Weapon/VOIDWeaponConfig.h`, `DA_Weapon_Rifle/Shotgun` |
| **HUD WeaponIcon / RepairText** | 1/2 키 무기 스왑 시 `OnWeaponEquipped(UVOIDWeaponConfig*)` → 아이콘/이름 토글. 슬롯 설치 시 `0/3 → 3/3 ✓ Press E to Start` 갱신 (`OnSlotInstalled`/`OnRepairComplete`) | `UI/VOIDHUDWidget.cpp::HandleWeaponEquipped/HandleSlotInstalled/HandleRepairComplete` |

> 상세 채점용 `file:line` 매핑은 [`마스터.md`](./마스터.md) 참조.

---

## 5 파트별 커밋 (2026-04-29 기준)

| # | 해시 | 범위 |
|---|---|---|
| 1 | `8bc8fe2` | Finish vehicle part slot system — engine start branch + auto-find OwnerVehicle |
| 2 | `8d73ff7` | GameMode flow — escape/gameover level transitions + 10min escape timer |
| 3 | `405a083` | HUD system — 4-mode panel (Menu/InGame/GameOver/GameClear) + auto level routing |
| 4 | `c9f5238` | Inventory stacking + score system + zombie balance pass |
| 5 | `96961c1` | Debuff system trigger + health logging + interact sweep fallback + default shotgun |

---

## 개인프로젝트 와의 관계

이 레포는 Void 프로젝트의 **Tier 0 프로토타입**이며, 과제 제출 시점 스냅샷은 `v0.1-assignment8` 태그로 고정 예정. 이후 Void 본편 개발은 동일 레포 `main` 브랜치에서 지속되며, 05-01 부터 시작될 4주 팀프로젝트(Ch3) 의 TPS 타격감 자산을 5/28 이후 이식 예정.
