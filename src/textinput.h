#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class textinput; }
QT_END_NAMESPACE


class textinput : public QDialog
{
    Q_OBJECT

public:
    textinput(QWidget *parent = nullptr);
    ~textinput();
    void set_label(QString str);
    QString get_text(void);
    void set_text(QString text);
    void set_mode_key(void);
    void allow_encrypt_sel(bool on);
    int is_cb_encrypted(void);
protected:
    void changeEvent(QEvent *e);

private:
    Ui::textinput *m_ui;
};

#endif // TEXTINPUT_H
