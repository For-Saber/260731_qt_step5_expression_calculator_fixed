#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include <QMainWindow>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class CalculatorWindow;
}
QT_END_NAMESPACE

class CalculatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalculatorWindow(QWidget *parent = nullptr);
    ~CalculatorWindow() override;

private slots://slots声明
    void onInputButtonClicked();
    void onClearButtonClicked();
    void onBackspaceButtonClicked();
	void onEqualButtonClicked();

private:
	Ui::CalculatorWindow* ui;//指向UI界面的指针ui

};


#endif // CALCULATORWINDOW_H
