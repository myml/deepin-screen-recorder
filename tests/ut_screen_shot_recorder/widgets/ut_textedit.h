#pragma once
#include <gtest/gtest.h>
#include <QTest>
#include <QDebug>
#include "stub.h"
#include "addr_pri.h"
#include "../../src/widgets/textedit.h"

using namespace testing;
ACCESS_PRIVATE_FUN(TextEdit, void(QMouseEvent *e), mousePressEvent);
ACCESS_PRIVATE_FUN(TextEdit, void(QMouseEvent *e), mouseMoveEvent);
ACCESS_PRIVATE_FUN(TextEdit, void(QMouseEvent *e), mouseReleaseEvent);
ACCESS_PRIVATE_FUN(TextEdit, void(QMouseEvent *e), mouseDoubleClickEvent);
ACCESS_PRIVATE_FUN(TextEdit, void(QInputMethodEvent *e),inputMethodEvent);
ACCESS_PRIVATE_FUN(TextEdit, void(QKeyEvent *e),keyPressEvent);
ACCESS_PRIVATE_FUN(TextEdit, void(QFocusEvent *e), focusInEvent);
class TextEditTest:public testing::Test{

public:
    TextEdit *m_textEdit;
    QString m_tips = QString("tips");
    virtual void SetUp() override{
        m_textEdit = new TextEdit(1,nullptr);
    }

    virtual void TearDown() override{
        if(nullptr != m_textEdit)
            delete m_textEdit;
    }
};

TEST_F(TextEditTest, mousePressEvent)
{
    QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    m_textEdit->setEditing(true);
    call_private_fun::TextEditmousePressEvent(*m_textEdit,e);

    m_textEdit->setReadOnly(true);
    m_textEdit->setEditing(false);
    call_private_fun::TextEditmousePressEvent(*m_textEdit,e);

    m_textEdit->setEditing(false);
    QMouseEvent *leftButtonEvent = new QMouseEvent(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::TextEditmousePressEvent(*m_textEdit,leftButtonEvent);
}

TEST_F(TextEditTest, mouseMoveEvent)
{
    m_textEdit->setEditing(true);
    QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::TextEditmouseMoveEvent(*m_textEdit,e);

    m_textEdit->setEditing(true);
    call_private_fun::TextEditmouseMoveEvent(*m_textEdit,e);
}

TEST_F(TextEditTest, mouseReleaseEvent)
{
    QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::TextEditmouseReleaseEvent(*m_textEdit,e);
}

TEST_F(TextEditTest, mouseDoubleClickEvent)
{
    QMouseEvent *e = new QMouseEvent(QEvent::MouseButtonPress, QPoint(10,10), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::TextEditmouseDoubleClickEvent(*m_textEdit,e);
}

TEST_F(TextEditTest, inputMethodEvent)
{
    QInputMethodEvent *e = new QInputMethodEvent();
    call_private_fun::TextEditinputMethodEvent(*m_textEdit,e);
}

//TEST_F(TextEditTest, keyPressEvent)
//{
//    QKeyEvent *e = new QKeyEvent(QKeyEvent::KeyPress, 'a', Qt::NoModifier);
//    call_private_fun::TextEditkeyPressEvent(*m_textEdit,e);
//    m_textEdit->setReadOnly(false);
//    QKeyEvent *escEvent = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Escape, Qt::NoModifier);
//    call_private_fun::TextEditkeyPressEvent(*m_textEdit,escEvent);
//}

//TEST_F(TextEditTest, focusInEvent)
//{
//    QFocusEvent *e = new QFocusEvent(QEvent::FocusIn);
//    call_private_fun::TextEditfocusInEvent(*m_textEdit,e);
//}

TEST_F(TextEditTest, setColor)
{
    m_textEdit->setColor(QColor());
}

TEST_F(TextEditTest, getIndex)
{
    m_textEdit->getIndex();
}

TEST_F(TextEditTest, updateCursor)
{
    m_textEdit->updateCursor();
}

TEST_F(TextEditTest, setCursorVisible)
{
    m_textEdit->setCursorVisible(true);
}

TEST_F(TextEditTest, setFontSize)
{
    m_textEdit->setFontSize(10);
}

TEST_F(TextEditTest, setEditing)
{
    m_textEdit->setEditing(true);
}

TEST_F(TextEditTest, setSelecting)
{
    m_textEdit->setSelecting(true);
}

TEST_F(TextEditTest, updateContentSize)
{
    m_textEdit->updateContentSize(QString("在此处添加文本"));
}
