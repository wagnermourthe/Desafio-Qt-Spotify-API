#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QOAuth2AuthorizationCodeFlow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void granted();

    void authStatusChanged (QAbstractOAuth::Status status);

    void GetDevices();

    void GetUserInfo();

    void on_SearchButton_clicked();

    void on_PreviousButton_clicked();

    void on_NextButton_clicked();

    void on_PauseButton_clicked();

    void on_PlayButton_clicked();

    void on_SearchText_returnPressed();

    void on_newPlaylistButton_clicked();

    void on_addButton_clicked();

    void on_removeButton_clicked();

private:
    Ui::MainWindow *ui;

    QOAuth2AuthorizationCodeFlow spotify;
    bool isGranted;
    QString userName;
    QString deviceID[10];
    QString deviceName[10]; //max 10 devices
    unsigned int devicesNmbr;
};


#endif // MAINWINDOW_H
