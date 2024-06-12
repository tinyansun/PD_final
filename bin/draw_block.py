import matplotlib.pyplot as plt
import numpy as np
import json
import argparse

def draw_polygons_from_json(input_file, output_file):
    # 读取 JSON 文件
    with open(input_file, 'r') as f:
        polygons = json.load(f)
    
    with open('polygon_info.txt', 'w') as info:
        # 打印每个多边形的点到文件
        for i, poly in enumerate(polygons):
            points = poly['points']
            color = poly['color']
            info.write(f"Polygon {i}:\n")
            for point in points:
                info.write(f"  {point}\n")
            info.write(f"  Color: {color}\n\n")
    
    # 创建图像
    fig, ax = plt.subplots()

    # 绘制每个多边形
    for poly in polygons:
        points = np.array(poly['points'])
        color = poly['color']
        polygon = plt.Polygon(points, closed=True, edgecolor='black', facecolor=color)
        ax.add_patch(polygon)

    # 设置图像边界
    all_points = np.concatenate([np.array(poly['points']) for poly in polygons])
    min_x, max_x = np.min(all_points[:, 0]), np.max(all_points[:, 0])
    min_y, max_y = np.min(all_points[:, 1]), np.max(all_points[:, 1])
    plt.xlim(min_x - 1, max_x + 1)
    plt.ylim(min_y - 1, max_y + 1)

    # 设置背景颜色
    ax.set_facecolor('white')

    # 添加较粗略的坐标轴和网格线
    ax.grid(True, which='both', linestyle='--', linewidth=0.5)
    ax.axhline(y=0, color='k')
    ax.axvline(x=0, color='k')

    # 设置较粗略的坐标轴刻度
    x_ticks = np.linspace(min_x, max_x, num=5, dtype=int)
    y_ticks = np.linspace(min_y, max_y, num=5, dtype=int)
    ax.set_xticks(x_ticks)
    ax.set_yticks(y_ticks)

    # 保存图像到文件
    plt.gca().set_aspect('equal', adjustable='box')
    plt.savefig(output_file, bbox_inches='tight', pad_inches=0.1)
    plt.close()

    print(f"Polygon image saved as {output_file}")

if __name__ == "__main__":
    # 创建ArgumentParser对象
    parser = argparse.ArgumentParser(description='Generate grid image from CSV file.')
    
    # 添加参数
    parser.add_argument('--input', type=str, required=True, help='Path to the input CSV file.')
    parser.add_argument('--output', type=str, required=True, help='Path to the output image file.')

    # 解析参数
    args = parser.parse_args()

    # 绘制多边形
    draw_polygons_from_json(args.input, args.output)
