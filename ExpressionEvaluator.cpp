#include "ExpressionEvaluator.h"
#include <stdexcept>
#include <QStack>
#include <cmath>

double ExpressionEvaluator::evaluate(const QString& expression)
{
	QVector<Token> tokens = tokenize(expression);

	QVector<Token> postfix = toPostfix(tokens);

	return calculatePostfix(postfix);
}

QVector<Token> ExpressionEvaluator::tokenize(const QString& expression) const
{
	QVector<Token> tokens;//用于存储解析出来的 Token 对象
	int i = 0;

	while (i<expression.length())
	{
		QChar cur = expression[i];
		if (cur.isSpace())//如果当前字符是空格，则跳过
		{
			i++;
			continue;
		}

		if (cur.isDigit() || cur == '.')
		{
			QString number;//如果当前字符是数字或小数点，则开始解析一个数字（字符串格式）
			int decimalPointCount = 0;

			while (i < expression.length() && (expression[i].isDigit() || expression[i] == '.'))
			{
				if (expression[i] == '.') {

					decimalPointCount++;

					if (decimalPointCount > 1) {
						throw std::invalid_argument("Invalid number format");
					}
				}
				number += expression[i];
				i++;
			}
			tokens.append({ TokenType::Number, number });
			continue;
		}

		else if (expression.mid(i, 4) == QStringLiteral("sqrt"))// 如果从当前位置开始的4个字符是 "sqrt"，则识别为一个操作符
		{
			tokens.append({
				TokenType::Operator,
				QStringLiteral("sqrt")
				});

			i += 4;
			continue;
		}

		else if (cur == '-')//判断减号是二元减法还是一元负号
		{
			bool isUnaryMinus =
				tokens.isEmpty() ||
				tokens.last().type == TokenType::Operator ||
				tokens.last().type == TokenType::LeftParenthesis;//如果当前是表达式的开头，或者前一个 token 是操作符或左括号，则认为是负号

			if (isUnaryMinus)
			{
				tokens.append({
					TokenType::Operator,
					QStringLiteral("u-")
					});
			}
			else
			{
				tokens.append({
					TokenType::Operator,
					QStringLiteral("-")
					});
			}
			i++;
			continue;
		}

		else if (cur == '+' || cur == '*' || cur == '/' || cur == '^')
		{
			tokens.append({ TokenType::Operator, QString(cur) });
			i++;
			continue;
		}
		
		else if (cur == '(')
		{
			tokens.append({ TokenType::LeftParenthesis, QString(cur) });
			i++;
			continue;
		}

		else if (cur == ')')
		{
			tokens.append({ TokenType::RightParenthesis, QString(cur) });
			i++;
			continue;
		}

		else
		{
			throw std::invalid_argument("Invalid character in expression");
		}
	}

    return tokens;
}

int ExpressionEvaluator::precedence(const QString& op) const
{
	if (op == QStringLiteral("+") || op == QStringLiteral("-")) return 1;
	if (op == QStringLiteral("*") || op == QStringLiteral("/")) return 2;
	if (op == QStringLiteral("u-")) return 3; //一元负号的优先级高于乘除法
	if (op == QStringLiteral("^") ) return 4;
	if (op == QStringLiteral("sqrt")) return 5;
	return 0;
}

//数字       → 直接进 output
//运算符     → 根据优先级决定是否弹出旧运算符
//左括号     → 压入栈
//右括号     → 一直弹到左括号为止
QVector<Token> ExpressionEvaluator::toPostfix(
	const QVector<Token>& tokens) const
{
	QVector<Token> output;
	QStack<Token> operators;

	for (const Token& token : tokens)
	{
		if (token.type == TokenType::Number)
		{
			output.append(token);
		}

		else if (token.type == TokenType::Operator)
		{
			// u- 和 sqrt 都是前缀运算符，遇到时直接压栈
			if (token.text == QStringLiteral("u-") ||
				token.text == QStringLiteral("sqrt"))
			{
				operators.push(token);
			}
			else
			{
				//如果栈不为空
				//且栈顶是操作符
				//旦栈顶操作符的优先级大于当前操作符的优先级，或者二者优先级相等且当前操作符不是"^"(如果是"^"则不pop)(幂运算符"^"右结合)
				//则pop栈顶到输出队列，直到破坏上述条件为止
				while (!operators.isEmpty() &&
						operators.top().type == TokenType::Operator &&
						(
							precedence(operators.top().text) > precedence(token.text)
						||
							(
								precedence(operators.top().text) == precedence(token.text) &&
								token.text != QStringLiteral("^")
							)
						))
				{
					output.append(operators.pop());
				}

				operators.push(token);
			}
		}

		else if (token.type == TokenType::LeftParenthesis)
		{
			operators.push(token);
		}

		else if (token.type == TokenType::RightParenthesis)
		{
			while (!operators.isEmpty() &&
				operators.top().type != TokenType::LeftParenthesis)//一直把运算符弹出到 output，直到碰见对应的左括号
			{
				output.append(operators.pop());
			}

			if (operators.isEmpty())
			{
				throw std::invalid_argument("Mismatched parentheses");//如果栈为空，说明没有找到对应的左括号，抛出异常
			}

			// 弹出左括号，后缀表达式中不需要括号
			operators.pop();

			// 如果括号属于 sqrt(...)，把 sqrt 弹出到output
			if (!operators.isEmpty() &&
				operators.top().type == TokenType::Operator &&
				operators.top().text == QStringLiteral("sqrt"))
			{
				output.append(operators.pop());
			}
		}
	}

	while (!operators.isEmpty())
	{
		if (operators.top().type == TokenType::LeftParenthesis)
		{
			throw std::invalid_argument("Mismatched parentheses");//如果栈中还有左括号，说明括号不匹配，抛出异常
		}

		output.append(operators.pop());
	}

	return output;
}

double ExpressionEvaluator::calculatePostfix(//计算后缀表达式的值
    const QVector<Token>& postfix) const
{
    QStack<double> values;

    for (const Token& token : postfix)
    {
		if (token.type == TokenType::Number)//如果是数字，则将其转换为 double 并压入栈中
        {
            bool ok = false;
			double value = token.text.toDouble(&ok);//toDouble()函数将字符串转换为 double 类型，ok 用于指示转换是否成功

            if (!ok)
            {
                throw std::invalid_argument("Invalid number");
            }

            values.push(value);
        }

		else if (token.type == TokenType::Operator)//如果是操作符，则根据操作符类型进行计算
        {
            if (token.text == QStringLiteral("u-"))
            {
                if (values.isEmpty())
                {
                    throw std::invalid_argument("Invalid expression");
                }

                double value = values.pop();
                values.push(-value);
            }

            else if (token.text == QStringLiteral("sqrt"))
            {
                if (values.isEmpty())
                {
                    throw std::invalid_argument("Invalid expression");
                }

                double value = values.pop();

                if (value < 0)
                {
                    throw std::domain_error(
                        "Cannot take square root of a negative number");
                }

                values.push(std::sqrt(value));
            }

            else
            {
				if (values.size() < 2)//如果栈中没有足够的操作数，抛出异常
                {
                    throw std::invalid_argument("Invalid expression");
                }

                double right = values.pop();
                double left = values.pop();

                double result = 0.0;

                if (token.text == QStringLiteral("+"))
                {
                    result = left + right;
                }
                else if (token.text == QStringLiteral("-"))
                {
                    result = left - right;
                }
                else if (token.text == QStringLiteral("*"))
                {
                    result = left * right;
                }
                else if (token.text == QStringLiteral("/"))
                {
                    if (right == 0.0)
                    {
                        throw std::domain_error("Division by zero");
                    }

                    result = left / right;
                }
                else if (token.text == QStringLiteral("^"))
                {
                    result = std::pow(left, right);
                }
                else
                {
                    throw std::invalid_argument("Unknown operator");
                }

                values.push(result);
            }
        }
    }

    if (values.size() != 1)
    {
        throw std::invalid_argument("Invalid expression");
    }

    return values.top();
}