/**
 *
 * @file virtual_machine.cc
 * @brief 解释执行程序
 *
 * @author Keyi Li
 *
 */

#include "../include/interpreter.h"


Interpreter::Interpreter() = default;


void Interpreter::execute(vector<Quadruple> _code) {
    code = _code;
    index = 0;

    // TODO 我们需要动态区域
    t_stack.resize(100);
    v_stack.resize(100);

    int code_len = code.size();
    while (index < code_len)
        _execute();
}


void Interpreter::_execute() {
    int op = int(code[index].op);

    switch (op) {
        case int(INTER_CODE_OP_ENUM::MOV):
            _assign();
            index ++;
            break;
        case int(INTER_CODE_OP_ENUM::ADD):
        case int(INTER_CODE_OP_ENUM::SUB):
        case int(INTER_CODE_OP_ENUM::MUL):
        case int(INTER_CODE_OP_ENUM::DIV):
            _calc(op);
            index ++;
            break;
        case int(INTER_CODE_OP_ENUM::PRINT):
            _print();
            index ++;
            break;
            // TODO;
        default:
            break;
    }
}


void Interpreter::_print() {
    string value_str = code[index].arg1;
    if (value_str[0] == '\"' || value_str[0] == '\'')
        cout << value_str.substr(1, value_str.size() - 2) << endl;
    else
        cout << _getValue(value_str) << endl;
}


void Interpreter::_calc(int op) {
    double a = _getValue(code[index].arg1);
    double b = _getValue(code[index].arg2);
    double value;

    switch (op) {
        case int(INTER_CODE_OP_ENUM::ADD):
            value = a + b;
            break;
        case int(INTER_CODE_OP_ENUM::SUB):
            value = a - b;
            break;
        case int(INTER_CODE_OP_ENUM::MUL):
            value = a * b;
            break;
        case int(INTER_CODE_OP_ENUM::DIV):
            value = a / b;
            break;
    }

    string res = code[index].res;
    int temp_index = _getAddress(res);
    // 读取res
    if (res[0] == 'v')
        v_stack[temp_index] = value;
    else
        t_stack[temp_index] = value;
}


void Interpreter::_assign() {
    string res = code[index].res;

    // 先读取右值
    double r_value = _getValue(code[index].arg1);

    int temp_index = _getAddress(res);
    // 再读取左值
    if (res[0] == 'v')
        v_stack[temp_index] = r_value;
    else
        t_stack[temp_index] = r_value;
}


int Interpreter::_getAddress(string value_str) {
    if (value_str[0] == 'v') {
        int len = value_str.size();
        for (int i = 0; i < len; i ++)
            if (value_str[i] == '[') {
                // 相对寻址
                int offset = _getValue(value_str.substr(i + 1, len - i - 2));
                int base = _getValue(value_str.substr(1, i - 1));
                return offset + base;
            }
    }
    else {
        return string2int(value_str.substr(1));
    }
}


double Interpreter::_getValue(string value_str) {
    // 寻址
    if (value_str[0] == 'v') {
        int len = value_str.size();
        for (int i = 0; i < len; i ++)
            if (value_str[i] == '[') {
                // 相对寻址
                int offset = _getValue(value_str.substr(i + 1, len - i - 2));
                int base = _getValue(value_str.substr(1, i - 1));
                return v_stack[offset + base];
            }

        // 立即数寻址
        int temp_index = string2int(value_str.substr(1));
        return v_stack[temp_index];
    }
    // 临时变量
    if (value_str[0] == 't') {
        int temp_index = string2int(value_str.substr(1));
        return t_stack[temp_index];
    }
    // 立即数
    else
        return string2double(value_str);
}