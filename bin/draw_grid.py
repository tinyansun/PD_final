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

    image_rotated = np.rot90(image)

    # 保存图像到文件
    fig, ax = plt.subplots()
    ax.imshow(image_rotated, aspect='equal')

    # 设置刻度
    num_rows, num_cols = grid.shape
    x_ticks = np.linspace(0, num_cols - 1, num=5)
    y_ticks = np.linspace(0, num_rows - 1, num=5)
    ax.set_xticks(x_ticks)
    ax.set_yticks(y_ticks)

    # 设置刻度标签为0到1的比例
    ax.set_xticklabels([f'{x / (num_cols - 1):.2f}' for x in x_ticks])
    ax.set_yticklabels([f'{(num_rows - 1 - y) / (num_rows - 1):.2f}' for y in y_ticks])

    # 隐藏坐标轴边框
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['left'].set_visible(False)
    ax.spines['bottom'].set_visible(False)

    # 隐藏刻度线
    ax.tick_params(left=False, bottom=False)

    plt.savefig(args.output, bbox_inches='tight', pad_inches=0.1)

if __name__ == "__main__":
    main()
