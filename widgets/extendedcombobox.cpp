#include "widgets/extendedcombobox.h"
#include <QCompleter>
#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QDebug>
#include <QMessageBox>

ExtendedComboBox::ExtendedComboBox(QWidget *parent)
    : QComboBox(parent)
{
    // 基础设置
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);

    // 初始化代理模型和补全器
    setupFilter();
    connect(this->lineEdit(), &QLineEdit::editingFinished, this, &ExtendedComboBox::validateCurrentText);
}

void ExtendedComboBox::setupFilter() {
    // 创建代理模型
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(model());  // 使用原始模型作为源
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterRole(Qt::DisplayRole);  // 根据显示文本过滤

    // 配置自动补全
    m_customCompleter = new QCompleter(m_proxyModel, this);
    m_customCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_customCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    setCompleter(m_customCompleter);

    // 连接输入信号
    connect(lineEdit(), &QLineEdit::textEdited,
        [this](const QString &text) {
            m_proxyModel->setFilterFixedString(text);
        });
}

void ExtendedComboBox::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        // 尝试匹配当前文本

        const QString currentText = this->currentText();
        const int matchIndex = findText(currentText, Qt::MatchContains);

        if (matchIndex >= 0) {
            setCurrentIndex(matchIndex);  // 选择第一个匹配项
            this->hidePopup();                 // 关闭下拉框

            // 发送选择确认信号
            emit activated(matchIndex);
            e->accept();
            return;
        }
    }

    QComboBox::keyPressEvent(e);  // 其他按键交给父类处理
}

void ExtendedComboBox::validateCurrentText() {

    const QString currentText = this->currentText();
    const int matchIndex = findText(currentText);
    if (matchIndex < 0) {
        this->setCurrentText(this->itemText(3));
    }
}
