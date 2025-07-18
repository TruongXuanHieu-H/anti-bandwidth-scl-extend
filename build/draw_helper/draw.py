import matplotlib.pyplot as plt
import numpy as np

# Dữ liệu từ hình ảnh
problems = ["M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X"]
encodings = ["Duplex", "SCL_seq", "CP-CPLEX", "SCL_par", "CP-SAT"]  # Đổi thứ tự để SCL_par vẽ sau CP-CPLEX

data = {
    "Duplex": [35, 35, 101, 0, 0, 64, 0, 0, 220, 0, 136, 0],
    "CP-CPLEX": [30, 30, 121, 79, 220, 56, 0, 104, 220, 0, 136, 113],
    "CP-SAT": [33, 32, 93, 0, 0, 48, 0, 0, 0, 0, 136, 0],
    "SCL_seq": [35, 35, 102, 79, 220, 64, 0, 104, 220, 0, 136, 113],
    "SCL_par": [35, 35, 106, 81, 220, 64, 0, 104, 220, 0, 136, 116],
}

# Danh sách các marker khác nhau
markers = ['o', 's', 'D', '^', 'x']  # Cập nhật thứ tự marker phù hợp với thứ tự encodings

# Màu sắc theo yêu cầu
colors = {
    "Duplex": "magenta",
    "SCL_seq": "black",
    "SCL_par": "dimgray",  # Màu gần giống SCL_seq
    "CP-CPLEX": "blue",  # Màu tương phản với SCL
    "CP-SAT": "red"  # Màu tương phản với SCL
}

# Vẽ biểu đồ
plt.figure(figsize=(10, 6))
for encoding, marker in zip(encodings, markers):
    plt.plot(problems, data[encoding], marker=marker, label=encoding, linestyle='-', color=colors[encoding])

plt.xlabel("Problem")
plt.ylabel("Width Solved")
plt.title("Comparison of Encodings Across Problems")
plt.legend()
plt.grid(True)

# Đổi giá trị 0 trên trục tung thành 'NaN'
y_ticks = plt.yticks()[0]
y_ticks = ["NaN" if y == 0 else y for y in y_ticks]
plt.yticks(plt.yticks()[0], y_ticks)

plt.show()
