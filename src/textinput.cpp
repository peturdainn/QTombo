#include "textinput.h"
#include "ui_textinput.h"

textinput::textinput(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::textinput)
{
    m_ui->setupUi(this);
    m_ui->text->setFocus();
}

textinput::~textinput()
{
    m_ui->text->clear();
    delete m_ui;
}

void textinput::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void textinput::set_label(QString str)
{
    m_ui->label->setText(str);
}

QString textinput::get_text(void)
{
    return m_ui->text->text();
}

void textinput::set_text(QString text)
{
    m_ui->text->setText(text);
}

void textinput::set_mode_key(void)
{
    m_ui->text->setEchoMode(QLineEdit::Password);
    // password input mode -> no file encryption option
    m_ui->checkbox_encrypted->hide();
}

void textinput::allow_encrypt_sel(bool on)
{
    if(on)
        m_ui->checkbox_encrypted->show();
    else
        m_ui->checkbox_encrypted->hide();
}

int textinput::is_cb_encrypted(void)
{
    return m_ui->checkbox_encrypted->isChecked() ? 1 : 0;
}
