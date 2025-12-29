# 프레젠테이션 배포 가이드

이 HTML 프레젠테이션을 다른 컴퓨터에서 볼 수 있게 배포하는 방법입니다.

## 방법 1: Netlify Drop (가장 간단! ⭐ 추천)

1. https://app.netlify.com/drop 접속
2. `STM32_ADC_Lecture` 폴더 전체를 드래그 앤 드롭
3. 몇 초 후 URL이 생성됨 (예: `https://random-name-123.netlify.app`)
4. 해당 URL을 다른 사람에게 공유하면 끝!

**장점**: 계정 불필요, 무료, 즉시 배포

---

## 방법 2: GitHub Pages

1. GitHub에 새 저장소 생성 (예: `stm32-adc-lecture`)
2. `STM32_ADC_Lecture` 폴더의 파일들을 업로드
3. 저장소 Settings → Pages → Source를 `main` 브랜치, `/ (root)` 선택
4. 저장 후 `https://[사용자명].github.io/stm32-adc-lecture/01_프레젠테이션.html` 접속

**장점**: 무료, 안정적, 버전 관리 가능

---

## 방법 3: Python 간단 서버 (로컬 네트워크)

같은 네트워크(와이파이)에 있는 컴퓨터에서만 접근 가능합니다.

```bash
# STM32_ADC_Lecture 폴더로 이동
cd STM32_ADC_Lecture

# Python 3가 설치되어 있다면
python -m http.server 8000

# 또는 Python 2
python -m SimpleHTTPServer 8000
```

그러면 `http://[본인IP]:8000/01_프레젠테이션.html`로 접속 가능합니다.
본인 IP는 `ipconfig` (Windows) 또는 `ifconfig` (Mac/Linux)로 확인하세요.

---

## 방법 4: Vercel (추천)

1. https://vercel.com 접속하여 가입 (GitHub 계정으로 가능)
2. "Add New Project" → "Import Git Repository" 또는 직접 파일 업로드
3. `STM32_ADC_Lecture` 폴더 업로드
4. 자동으로 URL 생성됨

**장점**: 무료, 빠름, 자동 HTTPS

---

## 주의사항

- 이미지 파일(`.jpg`, `.png`)이 같은 폴더에 있어야 정상적으로 표시됩니다.
- HTML 파일 내에서 이미지 경로가 상대 경로로 되어 있는지 확인하세요.

