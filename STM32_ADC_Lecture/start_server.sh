#!/bin/bash

echo "========================================"
echo "STM32 ADC Lecture 로컬 서버 시작"
echo "========================================"
echo ""
echo "브라우저에서 다음 주소로 접속하세요:"
echo "http://localhost:8000/01_프레젠테이션.html"
echo ""
echo "같은 네트워크의 다른 컴퓨터에서 접속하려면:"
echo "http://[이 컴퓨터의 IP주소]:8000/01_프레젠테이션.html"
echo ""
echo "IP 주소 확인: ifconfig 또는 ip addr 명령어 사용"
echo ""
echo "서버 중지: Ctrl+C"
echo "========================================"
echo ""

python3 -m http.server 8000

