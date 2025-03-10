import os
import re
import sys
import pandas as pd

# Kiểm tra tham số dòng lệnh
if len(sys.argv) < 3:
    print("Vui lòng cung cấp tên tệp xuất và ít nhất một thư mục chứa tệp dữ liệu.")
    sys.exit(1)

# Lấy đường dẫn tệp xuất và danh sách thư mục đầu vào
output_file = sys.argv[1]
data_folders = sys.argv[2:]  # Nhận nhiều thư mục

# Danh sách để lưu dữ liệu
data_list = []

# Duyệt qua tất cả các thư mục được cung cấp
for data_folder in data_folders:
    if not os.path.isdir(data_folder):
        print(f"Cảnh báo: Thư mục '{data_folder}' không tồn tại. Bỏ qua...")
        continue

    # Duyệt qua tất cả các tệp trong thư mục và thư mục con
    for root, _, files in os.walk(data_folder):  # os.walk để duyệt đệ quy thư mục con
        for filename in files:
            if filename.endswith(".out"):  # Chỉ xử lý tệp .out
                file_path = os.path.join(root, filename)

                # Đọc nội dung tệp
                with open(file_path, "r", encoding="utf-8") as file:
                    content = file.read()

                # Trích xuất thông tin bằng regex
                problem_match = re.search(r"([\w-]+)\.mtx\.rnd", content)
                time_limit_match = re.search(r"time limit:\s+(\d+)\s+seconds", content)
                real_time_match = re.search(r"real time limit:\s+(\d+)\s+seconds", content)
                memory_limit_match = re.search(r"space limit:\s+(\d+)\s+MB", content)
                symmetry_match = re.search(r"\[runlim\] argv\[\d+\]:\s+-symmetry-break\s+\[runlim\] argv\[\d+\]:\s+(\w+)", content)
                lb_match = re.search(r"\[runlim\] argv\[\d+\]:\s+-set-lb\s+\[runlim\] argv\[\d+\]:\s+(\d+)", content)
                sat_match = re.search(r"s\s+SAT\s+\(w\s*=\s*\d+\)", content)
                unsat_match = re.search(r"s\s+UNSAT\s+\(w\s*=\s*\d+\)", content)
                memory_usage_match = re.search(r"space:\s+([\d.]+)\s+MB", content)
                time_consumed_match = re.search(r"time:\s+([\d.]+)\s+seconds", content)

                # Lấy giá trị nếu tìm thấy, nếu không thì đặt None
                problem = problem_match.group(1) if problem_match else None
                time_limit = int(time_limit_match.group(1)) if time_limit_match else None
                real_time = int(real_time_match.group(1)) if real_time_match else None
                memory_limit = int(memory_limit_match.group(1)) if memory_limit_match else None
                symmetry = symmetry_match.group(1) if symmetry_match else None
                lb = int(lb_match.group(1)) if lb_match else None
                feasibility = "SAT" if sat_match else ("UNSAT" if unsat_match else "-")
                memory_usage = float(memory_usage_match.group(1)) if memory_usage_match else None
                time_consumed = float(time_consumed_match.group(1)) if time_consumed_match else None

                # Lưu vào danh sách
                data_list.append([problem, time_limit, real_time, memory_limit, symmetry, lb, feasibility, memory_usage, time_consumed])

# Kiểm tra nếu có dữ liệu mới xuất ra Excel
if data_list:
    df = pd.DataFrame(data_list, columns=["Problem", "CPU time limit", "Real time limit", "Memory limit", "Symmetry", "Bandwidth", "Feasibility", "Memory usage (MB)", "Time consumed (s)"])

    # Sắp xếp theo "Bandwidth" (lb), giữ nguyên thứ tự nếu có giá trị None
    df = df.sort_values(by=["Problem", "Bandwidth"], ascending=[True, True], na_position="last")

    print(df)
    df.to_excel(output_file, index=False)
    print(f"Xuất dữ liệu thành công vào {output_file}")
else:
    print("Không tìm thấy dữ liệu hợp lệ để xuất.")
