#include "FunctionPanelPreDefined.h"

// [핵심] 이곳에서 필요한 헤더들을 모두 가져옵니다.
#include "ETHuman3DApp.h"        // theApp 사용을 위해
#include "FunctionPanelRight.h"  // FunctionPanelRight 클래스(pRt) 정의를 알기 위해

// ============================================================================
// CustomButton
// ============================================================================
CustomButton::CustomButton(QWidget *parent) : QPushButton(parent) {}

void CustomButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        showContextMenu(event->pos());
        return;
    }
    if (event->button() == Qt::LeftButton) {
        dragStartPos = event->pos();
    }
    QPushButton::mousePressEvent(event);
}

void CustomButton::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - dragStartPos).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {
            // [theApp 사용] 헤더 분리 덕분에 문제 없음
            theApp.b_IsButtonDragging = true;
            move(mapToParent(event->pos() - dragStartPos));
        }
    }
}

void CustomButton::showContextMenu(const QPoint &pos) {
    QMenu contextMenu;
    QAction *action1 = contextMenu.addAction("Action 1");
    QAction *action2 = contextMenu.addAction("Action 2");

    QAction *selectedAction = contextMenu.exec(mapToGlobal(pos));
    if (selectedAction) {
        if (selectedAction == action1) performAction1();
        else if (selectedAction == action2) performAction2();
    }
}

void CustomButton::performAction1() {
    theApp.SetMessageBox("action1");
}

void CustomButton::performAction2() {
    theApp.SetMessageBox("action2");
}

// ============================================================================
// LeftButton
// ============================================================================
LeftButton::LeftButton(QWidget *parent) : QPushButton(parent) {
    defaultIcon = QIcon("./data/image/left.png");
    hoverIcon = QIcon("./data/image/left_clicked.png");
    setIcon(defaultIcon);
}

void LeftButton::enterEvent(QEnterEvent *event) {
    setIcon(hoverIcon);
    QPushButton::enterEvent(event);
}

void LeftButton::leaveEvent(QEvent *event) {
    setIcon(defaultIcon);
    QPushButton::leaveEvent(event);
}

// ============================================================================
// RightButton
// ============================================================================
RightButton::RightButton(QWidget *parent) : QPushButton(parent) {
    defaultIcon = QIcon("./data/image/right.png");
    hoverIcon = QIcon("./data/image/right_clicked.png");
    setIcon(defaultIcon);
}

void RightButton::enterEvent(QEnterEvent *event) {
    setIcon(hoverIcon);
    QPushButton::enterEvent(event);
}

void RightButton::leaveEvent(QEvent *event) {
    setIcon(defaultIcon);
    QPushButton::leaveEvent(event);
}

// ============================================================================
// MultipleButtonScorllArea
// ============================================================================
MultipleButtonScorllArea::MultipleButtonScorllArea(QWidget *parent) : QScrollArea(parent) {}

void MultipleButtonScorllArea::wheelEvent(QWheelEvent *event) {
    if (event->angleDelta().y() != 0) {
        event->ignore();
    } else {
        QScrollArea::wheelEvent(event);
    }
}

// ============================================================================
// CalculationSettingDialog
// ============================================================================
CalculationSettingDialog::CalculationSettingDialog(QWidget *parent) : QDialog(parent) {}

void CalculationSettingDialog::closeEvent(QCloseEvent *event) {
    // [theApp 사용]
    if (theApp.SettingContentsChangeCheck()) {
        if (QMessageBox::Yes == theApp.SetMessageBox_YesNo("Do you want to exit without saving the settings?")) {
            event->accept();
        } else {
            event->ignore();
        }
    }
}

// ============================================================================
// MultipleUIDialog
// ============================================================================
MultipleUIDialog::MultipleUIDialog(QWidget *parent) : QDialog(parent) {}

void MultipleUIDialog::closeEvent(QCloseEvent *event) {
    // [theApp + FunctionPanelRight 사용]
    // cpp 파일 상단에 "FunctionPanelRight.h"를 include 했으므로 pRt의 내부를 알 수 있음!
    theApp.pRt->MultipleUICloseTrigger();
}

void MultipleUIDialog::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QDialog::keyPressEvent(event);
    }
}

// ============================================================================
// ObjectMaterialTextWindow
// ============================================================================
ObjectMaterialTextWindow::ObjectMaterialTextWindow(QWidget *parent) : QDialog(parent) {
    setStyleSheet("background-color: white;");

    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    if (primaryScreen) {
        resize(primaryScreen->geometry().width() * 0.15, primaryScreen->geometry().height() * 0.45);
    } else {
        resize(300, 500);
    }

    setWindowTitle("Material elemental composition edit");
    QVBoxLayout *layout = new QVBoxLayout(this);
    textEdit = new QTextEdit(this);
    layout->addWidget(textEdit);
    setLayout(layout);
}

QString ObjectMaterialTextWindow::getText() const {
    return savedText;
}

void ObjectMaterialTextWindow::setText(const QString &text) {
    savedText = text;
    textEdit->setText(savedText);
}

void ObjectMaterialTextWindow::closeEvent(QCloseEvent *event) {
    savedText = textEdit->toPlainText();
    QDialog::closeEvent(event);
}

// ============================================================================
// FlexibleValidator
// ============================================================================
FlexibleValidator::FlexibleValidator(Rules rules, QObject *parent)
    : QValidator(parent), m_rules(rules) {}

QValidator::State FlexibleValidator::validate(QString &input, int &) const {
    if (m_rules.testFlag(ReplaceSpace))
        input.replace(' ', '_');

    const bool needsNumberCheck = m_rules & (AllowPositiveInt | AllowSignedInt | AllowReal | AllowPositiveReal);
    if (!needsNumberCheck) return Acceptable;

    if (m_rules.testFlag(AllowPositiveInt) && matchPosIntAccept(input)) return Acceptable;
    if (m_rules.testFlag(AllowSignedInt) && matchSignedIntAccept(input)) return Acceptable;
    if (m_rules.testFlag(AllowReal) && matchRealAccept(input)) return Acceptable;
    if (m_rules.testFlag(AllowPositiveReal) && matchPosRealAccept(input)) return Acceptable;

    if (m_rules.testFlag(AllowPositiveInt) && matchPosIntInter(input)) return Intermediate;
    if (m_rules.testFlag(AllowSignedInt) && matchSignedIntInter(input)) return Intermediate;
    if (m_rules.testFlag(AllowReal) && matchRealInter(input)) return Intermediate;
    if (m_rules.testFlag(AllowPositiveReal) && matchPosRealInter(input)) return Intermediate;

    return Invalid;
}

// Static Helpers Implementation
bool FlexibleValidator::matchPosIntAccept(const QString &s) {
    static const QRegularExpression re(R"(^[1-9]\d*$)"); return re.match(s).hasMatch();
}
bool FlexibleValidator::matchPosIntInter(const QString &s) {
    static const QRegularExpression re(R"(^\d*$)"); return re.match(s).hasMatch();
}
bool FlexibleValidator::matchSignedIntAccept(const QString &s) {
    static const QRegularExpression re(R"(^-?(0|[1-9]\d*)$)"); return re.match(s).hasMatch();
}
bool FlexibleValidator::matchSignedIntInter(const QString &s) {
    static const QRegularExpression re(R"(^-?\d*$)"); return re.match(s).hasMatch();
}
bool FlexibleValidator::matchRealAccept(const QString &s) {
    static const QRegularExpression re(R"(^[+-]?((\d+(\.\d*)?)|(\.\d+))([eE][+-]?\d+)?$)"); return re.match(s).hasMatch();
}
bool FlexibleValidator::matchRealInter(const QString &s) {
    static const QRegularExpression re(R"(^[+-]?(\d*(\.)?\d*)?([eE][+-]?\d*)?$)"); return re.match(s).hasMatch();
}
bool FlexibleValidator::matchPosRealAccept(const QString &s) {
    bool ok = false;
    const double v = s.toDouble(&ok);
    return ok && v > 0.0;
}
bool FlexibleValidator::matchPosRealInter(const QString &s) {
    static const QRegularExpression re(R"(^\+?(\d*(\.)?\d*)?([eE][+-]?\d*)?$)");
    return re.match(s).hasMatch();
}