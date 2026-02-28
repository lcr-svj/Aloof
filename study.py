import sys
import io

sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')


def simple_calculator():
    """一个简单的计算器，可以执行加、减、乘、除运算。"""
    print("欢迎使用简易计算器！")
    print("可用操作: +, -, *, /")
    
    # 获取用户输入
    try:
        num1 = float(input("请输入第一个数字: "))
        operator = input("请输入运算符 (+, -, *, /): ")
        num2 = float(input("请输入第二个数字: "))
    except ValueError:
        print("输入错误：请确保输入有效的数字。")
        return

    # 执行计算
    result = None
    if operator == "+":
        result = num1 + num2
    elif operator == "-":
        result = num1 - num2
    elif operator == "*":
        result = num1 * num2
    elif operator == "/":
        if num2 == 0:
            print("错误：除数不能为零。")
            return
        result = num1 / num2
    else:
        print("无效的运算符，请使用 +, -, *, /")
        return

    # 输出结果
    print(f"结果: {num1} {operator} {num2} = {result}")

if __name__ == "__main__":
    simple_calculator()