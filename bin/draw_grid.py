import matplotlib.pyplot as plt
import numpy as np
import argparse

def main():
    # 创建ArgumentParser对象
    parser = argparse.ArgumentParser(description='Generate grid image from CSV file.')
    
    # 添加参数
    parser.add_argument('--input', type=str, required=True, help='Path to the input CSV file.')
    parser.add_argument('--output', type=str, required=True, help='Path to the output image file.')

    # 解析参数
    args = parser.parse_args()

    # 读取CSV文件
    grid = np.loadtxt(args.input, delimiter=',')

    # 创建颜色映射
    color_map = {
        0: [0.5, 0.5, 0.5],  # 灰色 不可穿越区域
        1: [1.0, 0.65, 0.0], # 橘色 可穿越区域
        2: [1.0, 1.0, 0.0],  # 黄色 特殊区域
    }

    # 创建图像数组
    image = np.ones((grid.shape[0], grid.shape[1], 3))  # 初始化为白色
    for key, color in color_map.items():
        image[grid == key] = color

    # 保存图像到文件
    plt.imshow(image, aspect='equal')
    plt.axis('off')
    plt.savefig(args.output, bbox_inches='tight', pad_inches=0.1)

    print(f"Grid image saved as {args.output}")

if __name__ == "__main__":
    main()
