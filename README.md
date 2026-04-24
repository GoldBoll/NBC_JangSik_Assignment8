# Void — 8번 과제 제출물 (NBC_JangSik_Assignment8)

3인칭 TPS 좀비 서바이벌 익스트랙션 게임 `Void`의 **Tier 0 프로토타입**.
스파르타 언리얼 부트캠프 8번 과제 제출물이면서, Void 본편의 첫 구현 스냅샷.

## 컨셉 한 줄

> 좀비로 뒤덮인 도시에서 소음을 관리하며 자원을 파밍하고, 살아서 탈출해 이동형 차량 은신처를 강화하는 **TPS 서바이벌 익스트랙션**.

## 8번 과제 해석 — Void 익스트랙션 루프 3단계

| Wave | Void 해석 | 시간 |
|---|---|---|
| **Wave 1: 진입** | 주택가 저위험 파밍, 좀비 소수 | 3분 |
| **Wave 2: 심층** | 상가 중위험 파밍, 좀비 증가, 사이렌 이벤트 | 4분 |
| **Wave 3: 탈출 (차량 수리)** | 부품 3개 수집 → 차량 장착 → 시동 탈출 | 4~5분 |

점수는 **탈출 성공 시에만** 인정 (Void 익스트랙션 철학).

## 주요 시스템

- **TPS 숄더뷰 카메라 리그** — 오른쪽 어깨 기본, ADS 조준 시 FOV 축소
- **무게 기반 과적 디버프** — 타르코프식 kg 단위, `WeightRatio`로 이동속도↓·소음↑·스태미나↑
- **소음 기반 좀비 AI 인식** — 행동별 소음 수치 → 좀비 Perception
- **웨이브 상태 머신** — `AVOIDGameMode`가 Preparing/InProgress/Ending/Completed 관리
- **Wave 3 차량 수리 탈출 루프** — 부품 3종(배터리 8kg / 연료통 12kg / 점화 플러그 2kg)
- **디버프 3종** — 출혈·골절·과적

## 기술 스택

- Unreal Engine 5.4+
- C++ 주도 + Blueprint (연출·UI)
- Enhanced Input System
- UMG (BindWidget 중심)
- DataAsset · DataTable · UStruct 활용

## 폴더 구조 (소스)

```
Source/VoidUnreal/
├── Public/
│   ├── Core/          ← GameMode/GameState/PlayerController/GameInstance
│   ├── Characters/    ← Base/Player/Zombie
│   ├── AI/            ← Zombie AIController
│   ├── Components/    ← Health/Inventory/Noise/Debuff
│   ├── Items/         ← ItemDataAsset/Interface/Pickup/VehiclePart
│   ├── Waves/         ← WaveData/SpawnVolume
│   └── UI/            ← HUD Widget
└── Private/           ← Public과 대칭
```

클래스 접두사 **`VOID`** (예: `AVOIDPlayerCharacter`), 모듈 매크로 **`VOIDUNREAL_API`**.

## 빌드

1. `VoidUnreal.uproject` 우클릭 → **Generate Visual Studio project files**
2. `VoidUnreal.sln` 열어서 **Development Editor / Win64** 로 빌드
3. 에디터에서 PIE 실행

## 평가 기준 대응

- **완성도**: 3웨이브 · HUD · 메뉴 · TPS 숄더뷰 + ADS · 타르코프식 무게 HUD
- **이해도**: 웨이브 상태 머신 · Component 재사용 · Enhanced Input · UMG BindWidget + Delegate
- **우수성(도전)**: Wave 3 차량 수리 · 디버프 3종 · UI 애니메이션 · 3D 위젯 부품 인디케이터 · ADS 트레이드오프

## 구현 계획서

상세 일정과 진행 상태는 구현계획 문서 참고:
<https://github.com/GoldBoll/Bootcamp-TIL/blob/main/scrum/8번과제_구현계획.md>

## Void 본편 관계

이 레포는 Void 프로젝트의 **Tier 0 프로토타입**이며, 과제 제출 시점 스냅샷은 `v0.1-assignment8` 태그로 고정됨. 이후 Void 본편 개발은 동일 레포 `main` 브랜치에서 지속되며, 팀프로젝트(Ch3)에서 배운 TPS 타격감 자산을 5/28 이후 이식 예정.
