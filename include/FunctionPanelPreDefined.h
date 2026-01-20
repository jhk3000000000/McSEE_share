#pragma once

// UI 관련 기본
#include <QPushButton>
#include <QDialog>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QValidator>

// 이벤트 관련
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QEnterEvent> // Qt6 필수
#include <QCloseEvent>

// 기타 도구
#include <QMenu>
#include <QAction>
#include <QScreen>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QMessageBox>

// [중요] 앱 로직 헤더(ETHuman3DApp.h)는 여기서 include 하지 않습니다!

using namespace utils::ui;

// ============================================================================
// CustomButton
// ============================================================================
class CustomButton : public QPushButton {
    Q_OBJECT
public:
    explicit CustomButton(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    void showContextMenu(const QPoint &pos);
    void performAction1();
    void performAction2();

private:
    QPoint dragStartPos;
};

// ============================================================================
// LeftButton
// ============================================================================
class LeftButton : public QPushButton {
    Q_OBJECT
public:
    explicit LeftButton(QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QIcon defaultIcon;
    QIcon hoverIcon;
};

// ============================================================================
// RightButton
// ============================================================================
class RightButton : public QPushButton {
    Q_OBJECT
public:
    explicit RightButton(QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    QIcon defaultIcon;
    QIcon hoverIcon;
};

// ============================================================================
// MultipleButtonScorllArea
// ============================================================================
class MultipleButtonScorllArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit MultipleButtonScorllArea(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
};

// ============================================================================
// CalculationSettingDialog
// ============================================================================
class CalculationSettingDialog : public QDialog {
    Q_OBJECT
public:
    explicit CalculationSettingDialog(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
};

// ============================================================================
// MultipleUIDialog
// ============================================================================
class MultipleUIDialog : public QDialog {
    Q_OBJECT
public:
    explicit MultipleUIDialog(QWidget *parent = nullptr);

protected: // public -> protected 권장 (이벤트 핸들러이므로)
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    // 필요하다면 시그널 추가 가능
};

// ============================================================================
// ObjectMaterialTextWindow
// ============================================================================
class ObjectMaterialTextWindow : public QDialog
{
    Q_OBJECT
public:
    explicit ObjectMaterialTextWindow(QWidget *parent = nullptr);

    QString getText() const;
    void setText(const QString &text);

    QTextEdit *textEdit;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QString savedText = "";
};

// ============================================================================
// FlexibleValidator
// ============================================================================
class FlexibleValidator : public QValidator
{
    Q_OBJECT
public:
    enum Rule : quint32 {
        AllowPositiveInt = 0x01,   // 1,2,3…
        AllowReal = 0x02,   // 실수(±, e표기 포함)
        ReplaceSpace = 0x04,   // 공백 → '_'
        AllowSignedInt = 0x08,   // 0, +정수, -정수 모두
        AllowPositiveReal = 0x10,  //  0 제외한 양의 실수만
        NoRule = 0x00
    };
    Q_DECLARE_FLAGS(Rules, Rule)

    explicit FlexibleValidator(Rules rules, QObject *parent = nullptr);

    State validate(QString &input, int &pos) const override;

private:
    /* 정규식 헬퍼 함수들 */
    static bool matchPosIntAccept(const QString &s);
    static bool matchPosIntInter(const QString &s);
    static bool matchSignedIntAccept(const QString &s);
    static bool matchSignedIntInter(const QString &s);
    static bool matchRealAccept(const QString &s);
    static bool matchRealInter(const QString &s);
    static bool matchPosRealAccept(const QString &s);
    static bool matchPosRealInter(const QString &s);

    Rules m_rules;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(FlexibleValidator::Rules)

// ValFactory는 인라인 함수들이므로 헤더에 남겨두는 것이 사용하기 편합니다.
namespace ValFactory {

    inline QValidator* makePositiveInt(QObject *p) {
        return new FlexibleValidator(FlexibleValidator::AllowPositiveInt, p);
    }
    inline QValidator* makeInt(QObject *p) {
        return new FlexibleValidator(FlexibleValidator::AllowSignedInt, p);
    }
    inline QValidator* makeReal(QObject *p) {
        return new FlexibleValidator(FlexibleValidator::AllowReal, p);
    }
    inline QValidator* makeNoSpace(QObject *p) {
        return new FlexibleValidator(FlexibleValidator::ReplaceSpace, p);
    }
    inline QValidator* makePositiveReal(QObject *p) {
        return new FlexibleValidator(FlexibleValidator::AllowPositiveReal, p);
    }

} // namespace ValFactory