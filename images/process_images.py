#!/usr/bin/env python3
"""
이미지 배경 제거 및 JPEG 변환 스크립트
"""
import os
import io
from PIL import Image
import sys

try:
    from rembg import remove
    REMBG_AVAILABLE = True
except ImportError as e:
    REMBG_AVAILABLE = False
    print(f"rembg 라이브러리를 찾을 수 없습니다: {e}")
    print("간단한 배경 제거 방법을 사용합니다.")

def remove_background_rembg(input_path):
    """
    rembg를 사용한 AI 기반 배경 제거
    """
    try:
        with open(input_path, 'rb') as input_file:
            input_data = input_file.read()
            output_data = remove(input_data)
        return Image.open(io.BytesIO(output_data))
    except Exception as e:
        print(f"  rembg 오류: {str(e)}")
        return None

def remove_background_improved(image):
    """
    개선된 배경 제거 (흰색/밝은 배경 + 가장자리 감지)
    """
    # RGBA 모드로 변환
    if image.mode != 'RGBA':
        image = image.convert('RGBA')
    
    width, height = image.size
    pixels = image.load()
    
    # 가장자리 픽셀을 배경으로 간주
    edge_pixels = []
    for x in range(width):
        edge_pixels.append((x, 0))
        edge_pixels.append((x, height - 1))
    for y in range(height):
        edge_pixels.append((0, y))
        edge_pixels.append((width - 1, y))
    
    # 가장자리 색상 분석
    edge_colors = []
    for x, y in edge_pixels:
        r, g, b, a = pixels[x, y]
        edge_colors.append((r, g, b))
    
    # 가장자리 평균 색상 계산
    avg_r = sum(c[0] for c in edge_colors) // len(edge_colors)
    avg_g = sum(c[1] for c in edge_colors) // len(edge_colors)
    avg_b = sum(c[2] for c in edge_colors) // len(edge_colors)
    
    # 배경 색상 임계값 (평균 색상 기준 ±30)
    threshold = 30
    
    # 새 이미지 생성
    new_image = Image.new('RGBA', (width, height), (0, 0, 0, 0))
    new_pixels = new_image.load()
    
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            
            # 배경 색상과 유사한지 확인
            if (abs(r - avg_r) < threshold and 
                abs(g - avg_g) < threshold and 
                abs(b - avg_b) < threshold):
                # 배경이면 투명하게
                new_pixels[x, y] = (255, 255, 255, 0)
            else:
                # 전경이면 유지
                new_pixels[x, y] = (r, g, b, a)
    
    return new_image

def remove_background_simple(image):
    """
    간단한 배경 제거 (흰색/밝은 배경 기준)
    """
    return remove_background_improved(image)

def process_image(input_path, output_path, use_rembg=False, force_overwrite=True):
    """
    이미지 처리: 배경 제거 후 JPEG 변환
    """
    try:
        # 출력 파일이 이미 존재하고 덮어쓰기 안 함
        if os.path.exists(output_path) and not force_overwrite:
            print(f"  건너뜀: {output_path} (이미 존재)")
            return True
        
        # 이미지 열기
        print(f"처리 중: {input_path}")
        img = Image.open(input_path)
        original_size = img.size
        print(f"  원본 크기: {original_size[0]}x{original_size[1]}")
        
        # rembg 사용 시도 (기본적으로 비활성화 - 시간이 오래 걸림)
        img_rgba = None
        if use_rembg and REMBG_AVAILABLE:
            print("  rembg를 사용하여 배경 제거 중... (시간이 걸릴 수 있습니다)")
            try:
                img_rgba = remove_background_rembg(input_path)
            except Exception as e:
                print(f"  rembg 실패: {str(e)}")
                print("  개선된 배경 제거 방법으로 전환...")
        
        # rembg 실패하거나 사용 불가 시 개선된 방법 사용
        if img_rgba is None:
            print("  개선된 배경 제거 방법 사용 중...")
            img_rgba = remove_background_improved(img)
        else:
            print(f"  처리된 크기: {img_rgba.size[0]}x{img_rgba.size[1]}")
        
        # RGBA 모드 확인
        if img_rgba.mode != 'RGBA':
            img_rgba = img_rgba.convert('RGBA')
        
        # 흰색 배경에 합성 (JPEG는 투명도 지원 안 함)
        # RGB 모드로 변환하면서 흰색 배경에 합성
        rgb_img = Image.new('RGB', img_rgba.size, (255, 255, 255))
        if img_rgba.mode == 'RGBA':
            rgb_img.paste(img_rgba, mask=img_rgba.split()[3])  # 알파 채널을 마스크로 사용
        else:
            rgb_img.paste(img_rgba)
        
        # JPEG로 저장 (고품질)
        rgb_img.save(output_path, 'JPEG', quality=95, optimize=True)
        print(f"  완료: {output_path}")
        
        return True
    except Exception as e:
        print(f"  오류 발생: {str(e)}")
        import traceback
        traceback.print_exc()
        return False

def main():
    # 현재 디렉토리의 이미지 파일들 처리
    image_files = [
        'nucleo-f401re.avif',
        'capacitive-soil-moisture-sensor-v1.2.avif'
    ]
    
    output_files = [
        'nucleo-f401re.jpg',
        'capacitive-soil-moisture-sensor-v1.2.jpg'
    ]
    
    print("=" * 50)
    print("이미지 배경 제거 및 JPEG 변환 시작")
    print("=" * 50)
    
    success_count = 0
    for input_file, output_file in zip(image_files, output_files):
        if os.path.exists(input_file):
            # rembg 사용하여 AI 기반 배경 제거
            if process_image(input_file, output_file, use_rembg=True, force_overwrite=True):
                success_count += 1
        else:
            print(f"파일을 찾을 수 없습니다: {input_file}")
    
    print("=" * 50)
    print(f"처리 완료: {success_count}/{len(image_files)} 파일")
    print("=" * 50)

if __name__ == '__main__':
    main()

