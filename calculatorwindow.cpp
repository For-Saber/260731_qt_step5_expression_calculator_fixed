#include "calculatorwindow.h"
#include "./ui_calculatorwindow.h"
#include <QList>
#include <QPushButton>
#include "ExpressionEvaluator.h"
#include <QMessageBox>
#include <exception>

CalculatorWindow::CalculatorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CalculatorWindow)
{
	ui->setupUi(this);//设置UI界面

    const QList<QPushButton*> inputButtons{//将所有按钮存入qlist中
    ui->digit0Button,//:: 是从一个作用域/类型里找东西，而-> 是通过一个指针访问某个对象的成员
    ui->digit1Button,
    ui->digit2Button,
    ui->digit3Button,
    ui->digit4Button,
    ui->digit5Button,
    ui->digit6Button,
    ui->digit7Button,
    ui->digit8Button,
    ui->digit9Button,

    ui->decimalPointButton,
    ui->addButton,
    ui->subtractButton,
    ui->multiplyButton,
    ui->divideButton,
    ui->powerButton,
    ui->squareRootButton,
    ui->leftParenthesisButton,
    ui->rightParenthesisButton
    };

    for (QPushButton* button : inputButtons)//inputButtons中的所有按钮与输入slot链接
    {
        connect(button,
            &QPushButton::clicked,
            this,
            &CalculatorWindow::onInputButtonClicked);
    }

    connect(ui->clearButton,
        &QPushButton::clicked,
        this,
        &CalculatorWindow::onClearButtonClicked);

    connect(ui->backspaceButton,
        &QPushButton::clicked,
        this,
        &CalculatorWindow::onBackspaceButtonClicked);
     
	connect(ui->expressionLineEdit,
        &QLineEdit::returnPressed,
        this,
        &CalculatorWindow::onEqualButtonClicked);//按下回车键时触发计算

	connect(ui->equalsButton, 
        &QPushButton::clicked, 
        this, 
		&CalculatorWindow::onEqualButtonClicked);//按下等号按钮时触发计算

	QRegularExpression expression(R"((?:[0-9+\-*/^().]|sqrt\()*)");//正则表达式，匹配数字、运算符、括号和sqrt函数
    QRegularExpressionValidator* validator =
        new QRegularExpressionValidator(expression, this);

	ui->expressionLineEdit->setValidator(validator);//设置输入验证器，阻止非法字符进入
}

CalculatorWindow::~CalculatorWindow()
{
    delete ui;
}

void CalculatorWindow::onInputButtonClicked()
{
    auto* clickedButton =
        qobject_cast<QPushButton*>(sender());//sender()返回本次信号的发送对象qobject，强制转换为QPushButton*

    if (clickedButton == nullptr)
    {
        return;
    }

    QString inputText = clickedButton->text();//根据输入按钮上屏运算字符

    if (inputText == QStringLiteral("×"))
    {
        inputText = QStringLiteral("*");
    }
    else if (inputText == QStringLiteral("÷"))
    {
        inputText = QStringLiteral("/");
    }
    else if (inputText == QStringLiteral("√"))
    {
        inputText = QStringLiteral("sqrt(");
    }

    ui->expressionLineEdit->insert(inputText);
    ui->expressionLineEdit->setFocus();//按下按钮之后焦点在按钮，setFocus()把键盘输入焦点重新交还给表达式输入框
}

void CalculatorWindow::onClearButtonClicked()//清空直接调用LineEdit库
{
    ui->expressionLineEdit->clear();
    ui->expressionLineEdit->setFocus();
}

void CalculatorWindow::onBackspaceButtonClicked()//退格直接调用LineEdit库
{
    ui->expressionLineEdit->backspace();
    ui->expressionLineEdit->setFocus();
}

void CalculatorWindow::onEqualButtonClicked()
{
    QString expression = ui->expressionLineEdit->text();

    if (expression.isEmpty())
    {
        return;
    }

    try
    {
		ExpressionEvaluator evaluator;//创建表达式计算器对象

        double result = evaluator.evaluate(expression);

        ui->expressionLineEdit->setText(
            QString::number(result, 'g', 15)
        );
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(
            this,
            QStringLiteral("Error"),
            QString::fromLocal8Bit(e.what())
        );
    }
}