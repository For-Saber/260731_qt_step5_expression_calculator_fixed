#ifndef EXPRESSIONEVALUATOR_H
#define EXPRESSIONEVALUATOR_H

#include <QString>
#include <QVector>

enum class TokenType//枚举类TokenType，表示不同类型的符号
{
    Number,
    Operator,
    LeftParenthesis,
    RightParenthesis
};

struct Token//结构体Token，表示一个真正识别出来的元素
{
    TokenType type;
    QString text;
};

class ExpressionEvaluator
{
public:
	double evaluate(const QString& expression);//计算输入的表达式字符串的值

private:
	QVector<Token> tokenize(const QString& expression) const;//将输入的表达式字符串分解为一系列的 Token 对象
    //对象只需要读取、不需要修改时，经常使用 const 类型 &; 第二个 const 表示 tokenize() 这个成员函数不会修改 ExpressionEvaluator 对象自身的成员变量
    QVector<Token> toPostfix(const QVector<Token>& tokens) const;//将中缀表达式转换为后缀表达式
	double calculatePostfix(const QVector<Token>& postfix) const;//计算后缀表达式的值
	int precedence(const QString& op) const;//用于返回操作符的优先级
};

#endif // EXPRESSIONEVALUATOR_H
